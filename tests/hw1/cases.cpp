//
// Created by jamal on 03/12/2025.
//
// test_techsystem.cpp
// Catch2 unit tests for TechSystem (Winter 2026 wet #1)

#include <catch2/catch_test_macros.hpp>

// Adjust this include if your header name is different:
#include "TechSystem26a1.h"
// TechSystem26WINTER_wet1.h already includes wet1util.h, which defines
// StatusType and template output_t<T> with fields: status, ans.

TEST_CASE("addStudent: invalid, success, duplicate") {
    TechSystem sys;

    // Invalid IDs
    REQUIRE(sys.addStudent(0)  == StatusType::INVALID_INPUT);
    REQUIRE(sys.addStudent(-5) == StatusType::INVALID_INPUT);

    // First time is OK
    REQUIRE(sys.addStudent(1) == StatusType::SUCCESS);

    // Duplicate studentId -> FAILURE
    REQUIRE(sys.addStudent(1) == StatusType::FAILURE);

    // Different student is still OK
    REQUIRE(sys.addStudent(2) == StatusType::SUCCESS);
}

TEST_CASE("removeStudent: invalid, non-existing, enrolled constraint") {
    TechSystem sys;

    // Invalid input
    REQUIRE(sys.removeStudent(0)  == StatusType::INVALID_INPUT);
    REQUIRE(sys.removeStudent(-1) == StatusType::INVALID_INPUT);

    // No students yet: non-existing id -> FAILURE
    REQUIRE(sys.removeStudent(1) == StatusType::FAILURE);

    // Add a student and remove successfully
    REQUIRE(sys.addStudent(10)      == StatusType::SUCCESS);
    REQUIRE(sys.removeStudent(10)   == StatusType::SUCCESS);

    // Add again and enroll to a course
    REQUIRE(sys.addStudent(20)        == StatusType::SUCCESS);
    REQUIRE(sys.addCourse(100, 3)     == StatusType::SUCCESS);
    REQUIRE(sys.enrollStudent(20,100) == StatusType::SUCCESS);

    // Student is enrolled -> removal must FAIL
    REQUIRE(sys.removeStudent(20) == StatusType::FAILURE);
}

TEST_CASE("addCourse: invalid, success, duplicate") {
    TechSystem sys;

    // Invalid inputs (courseId <=0 or points <=0)
    REQUIRE(sys.addCourse(0, 3)   == StatusType::INVALID_INPUT);
    REQUIRE(sys.addCourse(-7, 3)  == StatusType::INVALID_INPUT);
    REQUIRE(sys.addCourse(1, 0)   == StatusType::INVALID_INPUT);
    REQUIRE(sys.addCourse(1, -10) == StatusType::INVALID_INPUT);

    // Valid course
    REQUIRE(sys.addCourse(1, 3) == StatusType::SUCCESS);

    // Duplicate course
    REQUIRE(sys.addCourse(1, 3) == StatusType::FAILURE);

    // Different course
    REQUIRE(sys.addCourse(2, 5) == StatusType::SUCCESS);
}

TEST_CASE("removeCourse: invalid, non-existing, has students constraint") {
    TechSystem sys;

    // Invalid input
    REQUIRE(sys.removeCourse(0)  == StatusType::INVALID_INPUT);
    REQUIRE(sys.removeCourse(-1) == StatusType::INVALID_INPUT);

    // Non-existing
    REQUIRE(sys.removeCourse(1) == StatusType::FAILURE);

    // Add course with no students -> can remove
    REQUIRE(sys.addCourse(10, 3)  == StatusType::SUCCESS);
    REQUIRE(sys.removeCourse(10)  == StatusType::SUCCESS);

    // Re-add course and student, enroll, then removal must fail
    REQUIRE(sys.addCourse(20, 4)    == StatusType::SUCCESS);
    REQUIRE(sys.addStudent(5)       == StatusType::SUCCESS);
    REQUIRE(sys.enrollStudent(5,20) == StatusType::SUCCESS);

    REQUIRE(sys.removeCourse(20) == StatusType::FAILURE);
}

TEST_CASE("enrollStudent: invalid, missing entities, duplicates") {
    TechSystem sys;

    // Invalid inputs
    REQUIRE(sys.enrollStudent(0, 1)  == StatusType::INVALID_INPUT);
    REQUIRE(sys.enrollStudent(1, 0)  == StatusType::INVALID_INPUT);
    REQUIRE(sys.enrollStudent(-1, 1) == StatusType::INVALID_INPUT);
    REQUIRE(sys.enrollStudent(1, -1) == StatusType::INVALID_INPUT);

    // No student or course yet
    REQUIRE(sys.enrollStudent(1, 1) == StatusType::FAILURE);

    // Add only student
    REQUIRE(sys.addStudent(1) == StatusType::SUCCESS);
    REQUIRE(sys.enrollStudent(1, 1) == StatusType::FAILURE); // no course yet

    // Add course and enroll successfully
    REQUIRE(sys.addCourse(1, 3)       == StatusType::SUCCESS);
    REQUIRE(sys.enrollStudent(1, 1)   == StatusType::SUCCESS);

    // Duplicate enroll to same course -> FAILURE
    REQUIRE(sys.enrollStudent(1, 1) == StatusType::FAILURE);

    // Enroll second student to same course
    REQUIRE(sys.addStudent(2)        == StatusType::SUCCESS);
    REQUIRE(sys.enrollStudent(2, 1)  == StatusType::SUCCESS);
}

TEST_CASE("completeCourse: invalid, missing, not enrolled, success & double complete") {
    TechSystem sys;

    REQUIRE(sys.completeCourse(0, 1)  == StatusType::INVALID_INPUT);
    REQUIRE(sys.completeCourse(1, 0)  == StatusType::INVALID_INPUT);
    REQUIRE(sys.completeCourse(-1, 1) == StatusType::INVALID_INPUT);
    REQUIRE(sys.completeCourse(1, -1) == StatusType::INVALID_INPUT);

    // Missing student / course
    REQUIRE(sys.completeCourse(1, 1) == StatusType::FAILURE);

    REQUIRE(sys.addStudent(1) == StatusType::SUCCESS);
    REQUIRE(sys.addCourse(1, 5) == StatusType::SUCCESS);

    // Not enrolled yet
    REQUIRE(sys.completeCourse(1, 1) == StatusType::FAILURE);

    // Enroll and then complete
    REQUIRE(sys.enrollStudent(1, 1)   == StatusType::SUCCESS);
    output_t<int> res0 = sys.getStudentPoints(1);
    REQUIRE(res0.status() == StatusType::SUCCESS);
    int before = res0.ans();

    REQUIRE(sys.completeCourse(1, 1)  == StatusType::SUCCESS);

    // Points should have increased by course points (5)
    output_t<int> res1 = sys.getStudentPoints(1);
    REQUIRE(res1.status() ==  StatusType::SUCCESS);
    REQUIRE(res1.ans() == before + 5);

    // Student should no longer be enrolled in that course -> completing again fails
    REQUIRE(sys.completeCourse(1, 1) == StatusType::FAILURE);
}

TEST_CASE("removeStudent and removeCourse after completion") {
    TechSystem sys;

    REQUIRE(sys.addStudent(1) == StatusType::SUCCESS);
    REQUIRE(sys.addCourse(10, 3) == StatusType::SUCCESS);
    REQUIRE(sys.enrollStudent(1,10) == StatusType::SUCCESS);

    // While enrolled, cannot remove student or course
    REQUIRE(sys.removeStudent(1) == StatusType::FAILURE);
    REQUIRE(sys.removeCourse(10) == StatusType::FAILURE);

    // Complete course -> student leaves course
    REQUIRE(sys.completeCourse(1,10) == StatusType::SUCCESS);

    // Now course has no students -> can remove
    REQUIRE(sys.removeCourse(10) == StatusType::SUCCESS);

    // Student is not enrolled anywhere -> can remove
    REQUIRE(sys.removeStudent(1) == StatusType::SUCCESS);
}

TEST_CASE("awardAcademicPoints: invalid, empty system, new vs existing students") {
    TechSystem sys;

    // Invalid input
    REQUIRE(sys.awardAcademicPoints(0)  == StatusType::INVALID_INPUT);
    REQUIRE(sys.awardAcademicPoints(-5) == StatusType::INVALID_INPUT);

    // No students yet: still SUCCESS (just a no-op logically)
    REQUIRE(sys.awardAcademicPoints(3) == StatusType::SUCCESS);

    // Student added after previous award should NOT get those old points
    REQUIRE(sys.addStudent(1) == StatusType::SUCCESS);
    output_t<int> r1 = sys.getStudentPoints(1);
    REQUIRE(r1.status() ==  StatusType::SUCCESS);
    REQUIRE(r1.ans() == 0);

    // Award points to existing students
    REQUIRE(sys.awardAcademicPoints(10) == StatusType::SUCCESS);

    output_t<int> r2 = sys.getStudentPoints(1);
    REQUIRE(r2.status() ==  StatusType::SUCCESS);
    REQUIRE(r2.ans() == 10);

    // Add another student AFTER the award: should start at 0
    REQUIRE(sys.addStudent(2) == StatusType::SUCCESS);
    output_t<int> r3 = sys.getStudentPoints(2);
    REQUIRE(r3.status() ==  StatusType::SUCCESS);
    REQUIRE(r3.ans() == 0);

    // Another global award affects both current students
    REQUIRE(sys.awardAcademicPoints(2) == StatusType::SUCCESS);

    output_t<int> r4 = sys.getStudentPoints(1);
    output_t<int> r5 = sys.getStudentPoints(2);
    REQUIRE(r4.status() ==  StatusType::SUCCESS);
    REQUIRE(r5.status() ==  StatusType::SUCCESS);
    REQUIRE(r4.ans() == 12); // 10 + 2
    REQUIRE(r5.ans() == 2);  // 0 + 2
}

TEST_CASE("getStudentPoints: invalid, missing, and basic progression") {
    TechSystem sys;

    // Invalid ids
    {
        auto res0 = sys.getStudentPoints(0);
        REQUIRE(res0.status() ==  StatusType::INVALID_INPUT);
        auto res1 = sys.getStudentPoints(-1);
        REQUIRE(res1.status() ==  StatusType::INVALID_INPUT);
    }

    // Non-existing student
    {
        auto res = sys.getStudentPoints(5);
        REQUIRE(res.status() ==  StatusType::FAILURE);
    }

    // Add course and student, then simulate basic progression
    REQUIRE(sys.addCourse(1, 3) == StatusType::SUCCESS);
    REQUIRE(sys.addStudent(1)   == StatusType::SUCCESS);

    // Initial points = 0
    auto res2 = sys.getStudentPoints(1);
    REQUIRE(res2.status() ==  StatusType::SUCCESS);
    REQUIRE(res2.ans() == 0);

    // Enroll and complete
    REQUIRE(sys.enrollStudent(1,1)     == StatusType::SUCCESS);
    REQUIRE(sys.completeCourse(1,1)    == StatusType::SUCCESS);

    auto res3 = sys.getStudentPoints(1);
    REQUIRE(res3.status() ==  StatusType::SUCCESS);
    REQUIRE(res3.ans() == 3);

    // Apply academic bonus
    REQUIRE(sys.awardAcademicPoints(4) == StatusType::SUCCESS);
    auto res4 = sys.getStudentPoints(1);
    REQUIRE(res4.status() ==  StatusType::SUCCESS);
    REQUIRE(res4.ans() == 7);
}

TEST_CASE("Full example sequence from spec") {
    TechSystem sys;

    // From the PDF example run (order and results must match):contentReference[oaicite:1]{index=1}
    REQUIRE(sys.addCourse(1, 3)  == StatusType::SUCCESS);
    REQUIRE(sys.addCourse(2, 5)  == StatusType::SUCCESS);
    REQUIRE(sys.addStudent(1)    == StatusType::SUCCESS);
    REQUIRE(sys.addStudent(5)    == StatusType::SUCCESS);

    {
        auto res = sys.getStudentPoints(1);
        REQUIRE(res.status() ==  StatusType::SUCCESS);
        REQUIRE(res.ans() == 0);
    }

    REQUIRE(sys.enrollStudent(1, 1) == StatusType::SUCCESS);
    REQUIRE(sys.removeStudent(1)    == StatusType::FAILURE);
    REQUIRE(sys.removeCourse(1)     == StatusType::FAILURE);

    {
        auto res = sys.getStudentPoints(1);
        REQUIRE(res.status() ==  StatusType::SUCCESS);
        REQUIRE(res.ans() == 0);
    }

    REQUIRE(sys.completeCourse(1, 1) == StatusType::SUCCESS);
    REQUIRE(sys.completeCourse(1, 2) == StatusType::FAILURE);
    REQUIRE(sys.removeCourse(1)      == StatusType::SUCCESS);

    {
        auto res = sys.getStudentPoints(1);
        REQUIRE(res.status() ==  StatusType::SUCCESS);
        REQUIRE(res.ans() == 3);
    }

    REQUIRE(sys.awardAcademicPoints(10) == StatusType::SUCCESS);

    {
        auto res = sys.getStudentPoints(1);
        REQUIRE(res.status() ==  StatusType::SUCCESS);
        REQUIRE(res.ans() == 13);
    }

    REQUIRE(sys.removeStudent(1) == StatusType::SUCCESS);
    REQUIRE(sys.addStudent(100)  == StatusType::SUCCESS);

    {
        auto res = sys.getStudentPoints(100);
        REQUIRE(res.status() ==  StatusType::SUCCESS);
        REQUIRE(res.ans() == 0);
    }
}
