//
// Created by jamal on 03/12/2025.
//

#include <catch2/catch_test_macros.hpp>
#include "TechSystem26a1.h"

// Small helper to read points safely
static int getPointsOrMinusOne(TechSystem &sys, int studentId, StatusType expectedStatus)
{
    auto res = sys.getStudentPoints(studentId);
    REQUIRE(res.status() == expectedStatus);
    if (expectedStatus == StatusType::SUCCESS) {
        return res.ans();
    }
    return -1;
}

/******************************
 * addStudent + getStudentPoints
 ******************************/
TEST_CASE("addStudent and getStudentPoints - input validation and basic behavior")
{
    TechSystem sys;

    SECTION("addStudent - INVALID_INPUT for non-positive IDs")
    {
        REQUIRE(sys.addStudent(0) == StatusType::INVALID_INPUT);
        REQUIRE(sys.addStudent(-1) == StatusType::INVALID_INPUT);

        auto r0   = sys.getStudentPoints(0);
        auto rNeg = sys.getStudentPoints(-5);
        REQUIRE(r0.status()   == StatusType::INVALID_INPUT);
        REQUIRE(rNeg.status() == StatusType::INVALID_INPUT);
    }

    SECTION("getStudentPoints - FAILURE when student does not exist")
    {
        auto res = sys.getStudentPoints(42);
        REQUIRE(res.status() == StatusType::FAILURE);
    }

    SECTION("addStudent - SUCCESS for fresh ID, initial points 0")
    {
        REQUIRE(sys.addStudent(1) == StatusType::SUCCESS);
        auto res = sys.getStudentPoints(1);
        REQUIRE(res.status() == StatusType::SUCCESS);
        REQUIRE(res.ans() == 0);
    }

    SECTION("addStudent - FAILURE on duplicate ID, state unchanged")
    {
        REQUIRE(sys.addStudent(1) == StatusType::SUCCESS);
        REQUIRE(sys.addStudent(1) == StatusType::FAILURE);

        auto res = sys.getStudentPoints(1);
        REQUIRE(res.status() == StatusType::SUCCESS);
        REQUIRE(res.ans() == 0); // unchanged
    }
}

/******************************
 * addCourse
 ******************************/
TEST_CASE("addCourse - input validation and duplicates")
{
    TechSystem sys;

    SECTION("addCourse - INVALID_INPUT for non-positive ids or points")
    {
        REQUIRE(sys.addCourse(0, 3) == StatusType::INVALID_INPUT);
        REQUIRE(sys.addCourse(-1, 3) == StatusType::INVALID_INPUT);
        REQUIRE(sys.addCourse(1, 0) == StatusType::INVALID_INPUT);
        REQUIRE(sys.addCourse(1, -5) == StatusType::INVALID_INPUT);
    }

    SECTION("addCourse - SUCCESS then FAILURE on duplicate id")
    {
        REQUIRE(sys.addCourse(1, 3) == StatusType::SUCCESS);
        REQUIRE(sys.addCourse(1, 3) == StatusType::FAILURE);
    }
}

/******************************
 * removeStudent
 ******************************/
TEST_CASE("removeStudent - all cases")
{
    TechSystem sys;
    REQUIRE(sys.addStudent(1) == StatusType::SUCCESS);
    REQUIRE(sys.addCourse(10, 3) == StatusType::SUCCESS);

    SECTION("removeStudent - INVALID_INPUT")
    {
        REQUIRE(sys.removeStudent(0) == StatusType::INVALID_INPUT);
        REQUIRE(sys.removeStudent(-1) == StatusType::INVALID_INPUT);

        // Student 1 should still exist
        auto res = sys.getStudentPoints(1);
        REQUIRE(res.status() == StatusType::SUCCESS);
    }

    SECTION("removeStudent - FAILURE for non-existent student")
    {
        REQUIRE(sys.removeStudent(2) == StatusType::FAILURE);
        // student 1 still exists
        auto res = sys.getStudentPoints(1);
        REQUIRE(res.status() == StatusType::SUCCESS);
    }

    SECTION("removeStudent - FAILURE when student is enrolled")
    {
        REQUIRE(sys.enrollStudent(1, 10) == StatusType::SUCCESS);
        REQUIRE(sys.removeStudent(1) == StatusType::FAILURE);

        // Still enrolled; completing should still work
        REQUIRE(sys.completeCourse(1, 10) == StatusType::SUCCESS);
    }

    SECTION("removeStudent - SUCCESS when not enrolled, student disappears")
    {
        // Not enrolled at this point
        auto before = sys.getStudentPoints(1);
        REQUIRE(before.status() == StatusType::SUCCESS);

        REQUIRE(sys.removeStudent(1) == StatusType::SUCCESS);

        auto after = sys.getStudentPoints(1);
        REQUIRE(after.status() == StatusType::FAILURE);

        // Removing again fails
        REQUIRE(sys.removeStudent(1) == StatusType::FAILURE);
    }
}

/******************************
 * removeCourse
 ******************************/
TEST_CASE("removeCourse - all cases")
{
    TechSystem sys;
    REQUIRE(sys.addCourse(10, 3) == StatusType::SUCCESS);
    REQUIRE(sys.addStudent(1) == StatusType::SUCCESS);

    SECTION("removeCourse - INVALID_INPUT")
    {
        REQUIRE(sys.removeCourse(0) == StatusType::INVALID_INPUT);
        REQUIRE(sys.removeCourse(-7) == StatusType::INVALID_INPUT);

        // course 10 should still exist; enrolling should succeed
        REQUIRE(sys.enrollStudent(1, 10) == StatusType::SUCCESS);
    }

    SECTION("removeCourse - FAILURE for non-existent course")
    {
        REQUIRE(sys.removeCourse(20) == StatusType::FAILURE);
        // existing course still there
        REQUIRE(sys.enrollStudent(1, 10) == StatusType::SUCCESS);
    }

    SECTION("removeCourse - FAILURE when students are enrolled")
    {
        REQUIRE(sys.enrollStudent(1, 10) == StatusType::SUCCESS);

        REQUIRE(sys.removeCourse(10) == StatusType::FAILURE);

        // Course & enrollment still alive
        REQUIRE(sys.completeCourse(1, 10) == StatusType::SUCCESS);
    }

    SECTION("removeCourse - SUCCESS when no students are enrolled")
    {
        // No one enrolled in 10 yet
        REQUIRE(sys.removeCourse(10) == StatusType::SUCCESS);

        // second removal fails
        REQUIRE(sys.removeCourse(10) == StatusType::FAILURE);

        // can't enroll or complete on removed course
        REQUIRE(sys.enrollStudent(1, 10) == StatusType::FAILURE);
        REQUIRE(sys.completeCourse(1, 10) == StatusType::FAILURE);
    }
}

/******************************
 * enrollStudent
 ******************************/
TEST_CASE("enrollStudent - invalid input and failure reasons")
{
    TechSystem sys;
    REQUIRE(sys.addStudent(1) == StatusType::SUCCESS);
    REQUIRE(sys.addCourse(10, 3) == StatusType::SUCCESS);

    SECTION("enrollStudent - INVALID_INPUT for non-positive ids, no side effects")
    {
        REQUIRE(sys.enrollStudent(0, 10) == StatusType::INVALID_INPUT);
        REQUIRE(sys.enrollStudent(1, 0) == StatusType::INVALID_INPUT);
        REQUIRE(sys.enrollStudent(0, 0) == StatusType::INVALID_INPUT);

        // Valid enroll still works afterwards
        REQUIRE(sys.enrollStudent(1, 10) == StatusType::SUCCESS);
    }

    SECTION("enrollStudent - FAILURE when student does not exist")
    {
        REQUIRE(sys.enrollStudent(2, 10) == StatusType::FAILURE);
    }

    SECTION("enrollStudent - FAILURE when course does not exist")
    {
        REQUIRE(sys.enrollStudent(1, 20) == StatusType::FAILURE);
    }

    SECTION("enrollStudent - SUCCESS then FAILURE on duplicate enrollment")
    {
        REQUIRE(sys.enrollStudent(1, 10) == StatusType::SUCCESS);
        REQUIRE(sys.enrollStudent(1, 10) == StatusType::FAILURE);

        // Completing the course should give points only once
        REQUIRE(sys.completeCourse(1, 10) == StatusType::SUCCESS);
        auto res = sys.getStudentPoints(1);
        REQUIRE(res.status() == StatusType::SUCCESS);
        REQUIRE(res.ans() == 3);

        // second complete attempt fails and doesn't add points
        REQUIRE(sys.completeCourse(1, 10) == StatusType::FAILURE);
        REQUIRE(sys.getStudentPoints(1).ans() == 3);
    }
}

/******************************
 * completeCourse
 ******************************/
TEST_CASE("completeCourse - all status cases")
{
    TechSystem sys;
    REQUIRE(sys.addStudent(1) == StatusType::SUCCESS);
    REQUIRE(sys.addCourse(10, 3) == StatusType::SUCCESS);

    SECTION("completeCourse - INVALID_INPUT")
    {
        REQUIRE(sys.completeCourse(0, 10) == StatusType::INVALID_INPUT);
        REQUIRE(sys.completeCourse(1, 0) == StatusType::INVALID_INPUT);
        REQUIRE(sys.completeCourse(0, 0) == StatusType::INVALID_INPUT);
    }

    SECTION("completeCourse - FAILURE when student does not exist")
    {
        REQUIRE(sys.completeCourse(2, 10) == StatusType::FAILURE);
    }

    SECTION("completeCourse - FAILURE when course does not exist")
    {
        REQUIRE(sys.completeCourse(1, 20) == StatusType::FAILURE);
    }

    SECTION("completeCourse - FAILURE when student not enrolled")
    {
        REQUIRE(sys.completeCourse(1, 10) == StatusType::FAILURE);
    }

    SECTION("completeCourse - SUCCESS adds points and removes from course")
    {
        REQUIRE(sys.enrollStudent(1, 10) == StatusType::SUCCESS);

        int before = getPointsOrMinusOne(sys, 1, StatusType::SUCCESS);
        REQUIRE(before == 0);

        REQUIRE(sys.completeCourse(1, 10) == StatusType::SUCCESS);

        int after = getPointsOrMinusOne(sys, 1, StatusType::SUCCESS);
        REQUIRE(after == before + 3);

        // student is no longer enrolled -> second complete should fail
        REQUIRE(sys.completeCourse(1, 10) == StatusType::FAILURE);
        REQUIRE(getPointsOrMinusOne(sys, 1, StatusType::SUCCESS) == after);
    }
}

/******************************
 * awardAcademicPoints
 ******************************/
TEST_CASE("awardAcademicPoints - invalid inputs and empty system")
{
    TechSystem sys;

    SECTION("awardAcademicPoints - INVALID_INPUT for non-positive points, no effect")
    {
        REQUIRE(sys.addStudent(1) == StatusType::SUCCESS);
        int before = getPointsOrMinusOne(sys, 1, StatusType::SUCCESS);
        REQUIRE(before == 0);

        REQUIRE(sys.awardAcademicPoints(0) == StatusType::INVALID_INPUT);
        REQUIRE(sys.awardAcademicPoints(-10) == StatusType::INVALID_INPUT);

        int after = getPointsOrMinusOne(sys, 1, StatusType::SUCCESS);
        REQUIRE(after == before);
    }

    SECTION("awardAcademicPoints - SUCCESS when there are no students")
    {
        REQUIRE(sys.awardAcademicPoints(10) == StatusType::SUCCESS);

        // Students added after the award should NOT get the bonus retroactively
        REQUIRE(sys.addStudent(1) == StatusType::SUCCESS);
        int pts = getPointsOrMinusOne(sys, 1, StatusType::SUCCESS);
        REQUIRE(pts == 0);
    }
}

TEST_CASE("awardAcademicPoints - only affects students present at time of call")
{
    TechSystem sys;
    REQUIRE(sys.addStudent(1) == StatusType::SUCCESS);
    REQUIRE(sys.addStudent(2) == StatusType::SUCCESS);

    // First bonus
    REQUIRE(sys.awardAcademicPoints(10) == StatusType::SUCCESS);

    int s1 = getPointsOrMinusOne(sys, 1, StatusType::SUCCESS);
    int s2 = getPointsOrMinusOne(sys, 2, StatusType::SUCCESS);
    REQUIRE(s1 == 10);
    REQUIRE(s2 == 10);

    // Remove student 2, add student 3
    REQUIRE(sys.removeStudent(2) == StatusType::SUCCESS);
    REQUIRE(sys.addStudent(3) == StatusType::SUCCESS);

    // Second bonus
    REQUIRE(sys.awardAcademicPoints(5) == StatusType::SUCCESS);

    s1 = getPointsOrMinusOne(sys, 1, StatusType::SUCCESS);
    int s3 = getPointsOrMinusOne(sys, 3, StatusType::SUCCESS);

    REQUIRE(s1 == 15); // 10 + 5
    REQUIRE(s3 == 5);  // only the second bonus
}

TEST_CASE("awardAcademicPoints with enrolled students and courses")
{
    TechSystem sys;
    REQUIRE(sys.addStudent(1) == StatusType::SUCCESS);
    REQUIRE(sys.addCourse(10, 3) == StatusType::SUCCESS);

    REQUIRE(sys.enrollStudent(1, 10) == StatusType::SUCCESS);
    REQUIRE(sys.awardAcademicPoints(10) == StatusType::SUCCESS);

    // Student is still enrolled and should successfully complete the course
    REQUIRE(sys.completeCourse(1, 10) == StatusType::SUCCESS);

    int pts = getPointsOrMinusOne(sys, 1, StatusType::SUCCESS);
    REQUIRE(pts == 3 + 10); // course points + bonus
}

/******************************
 * getStudentPoints - status coverage
 ******************************/
TEST_CASE("getStudentPoints - all statuses")
{
    TechSystem sys;

    SECTION("INVALID_INPUT for non-positive studentId")
    {
        REQUIRE(sys.getStudentPoints(0).status()   == StatusType::INVALID_INPUT);
        REQUIRE(sys.getStudentPoints(-4).status()  == StatusType::INVALID_INPUT);
    }

    SECTION("FAILURE for missing student")
    {
        REQUIRE(sys.getStudentPoints(1).status() == StatusType::FAILURE);
    }

    SECTION("SUCCESS and correct value for existing student (with courses and bonuses)")
    {
        REQUIRE(sys.addStudent(1) == StatusType::SUCCESS);
        REQUIRE(sys.addCourse(10, 3) == StatusType::SUCCESS);

        // Start at 0
        int pts = getPointsOrMinusOne(sys, 1, StatusType::SUCCESS);
        REQUIRE(pts == 0);

        // Complete a course
        REQUIRE(sys.enrollStudent(1, 10) == StatusType::SUCCESS);
        REQUIRE(sys.completeCourse(1, 10) == StatusType::SUCCESS);
        pts = getPointsOrMinusOne(sys, 1, StatusType::SUCCESS);
        REQUIRE(pts == 3);

        // Add a bonus
        REQUIRE(sys.awardAcademicPoints(7) == StatusType::SUCCESS);
        pts = getPointsOrMinusOne(sys, 1, StatusType::SUCCESS);
        REQUIRE(pts == 10);
    }
}

/******************************
 * Integration scenario from the spec
 ******************************/
TEST_CASE("Integration scenario - matches example from homework PDF")
{
    TechSystem sys;

    // addCourse(1, 3): SUCCESS
    REQUIRE(sys.addCourse(1, 3) == StatusType::SUCCESS);
    // addCourse(2, 5): SUCCESS
    REQUIRE(sys.addCourse(2, 5) == StatusType::SUCCESS);

    // addStudent(1): SUCCESS
    REQUIRE(sys.addStudent(1) == StatusType::SUCCESS);
    // addStudent(5): SUCCESS
    REQUIRE(sys.addStudent(5) == StatusType::SUCCESS);

    // getStudentPoints(1): SUCCESS, Value: 0
    auto s1 = sys.getStudentPoints(1);
    REQUIRE(s1.status() == StatusType::SUCCESS);
    REQUIRE(s1.ans() == 0);

    // enrollStudent(1, 1): SUCCESS
    REQUIRE(sys.enrollStudent(1, 1) == StatusType::SUCCESS);

    // removeStudent(1): FAILURE  (enrolled)
    REQUIRE(sys.removeStudent(1) == StatusType::FAILURE);
    // removeCourse(1): FAILURE   (has students)
    REQUIRE(sys.removeCourse(1) == StatusType::FAILURE);

    // getStudentPoints(1): SUCCESS, Value: 0 (still only global points)
    auto s2 = sys.getStudentPoints(1);
    REQUIRE(s2.status() == StatusType::SUCCESS);
    REQUIRE(s2.ans() == 0);

    // completeCourse(1, 1): SUCCESS
    REQUIRE(sys.completeCourse(1, 1) == StatusType::SUCCESS);
    // completeCourse(1, 2): FAILURE (not enrolled in course 2)
    REQUIRE(sys.completeCourse(1, 2) == StatusType::FAILURE);

    // removeCourse(1): SUCCESS   (no more enrolled students)
    REQUIRE(sys.removeCourse(1) == StatusType::SUCCESS);

    // getStudentPoints(1): SUCCESS, Value: 3
    auto s3 = sys.getStudentPoints(1);
    REQUIRE(s3.status() == StatusType::SUCCESS);
    REQUIRE(s3.ans() == 3);

    // awardAcademicPoints(10): SUCCESS
    REQUIRE(sys.awardAcademicPoints(10) == StatusType::SUCCESS);

    // getStudentPoints(1): SUCCESS, Value: 13
    auto s4 = sys.getStudentPoints(1);
    REQUIRE(s4.status() == StatusType::SUCCESS);
    REQUIRE(s4.ans() == 13);

    // removeStudent(1): SUCCESS
    REQUIRE(sys.removeStudent(1) == StatusType::SUCCESS);

    // addStudent(100): SUCCESS
    REQUIRE(sys.addStudent(100) == StatusType::SUCCESS);

    // getStudentPoints(100): SUCCESS, Value: 0
    auto s100 = sys.getStudentPoints(100);
    REQUIRE(s100.status() == StatusType::SUCCESS);
    REQUIRE(s100.ans() == 0);
}
