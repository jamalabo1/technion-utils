//
// Created by jamal on 03/12/2025.
//
// test_techsystem_extended.cpp
#include <catch2/catch_test_macros.hpp>

// Adjust if your header is named differently:
#include "TechSystem26a1.h"

// -----------------------------------------------------------------------------
// BASIC BEHAVIOR (from previous answer, kept for completeness)
// -----------------------------------------------------------------------------

TEST_CASE("v2 addStudent: invalid, success, duplicate") {
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

TEST_CASE("v2 removeStudent: invalid, non-existing, enrolled constraint") {
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

TEST_CASE("v2 addCourse: invalid, success, duplicate") {
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

TEST_CASE("v2 removeCourse: invalid, non-existing, has students constraint") {
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

TEST_CASE("v2 enrollStudent: invalid, missing entities, duplicates") {
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

TEST_CASE("v2 completeCourse: invalid, missing, not enrolled, success & double complete") {
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
    auto res0 = sys.getStudentPoints(1);
    REQUIRE(res0.status() == StatusType::SUCCESS);
    int before = res0.ans();

    REQUIRE(sys.completeCourse(1, 1)  == StatusType::SUCCESS);

    // Points should have increased by course points (5)
    auto res1 = sys.getStudentPoints(1);
    REQUIRE(res1.status() == StatusType::SUCCESS);
    REQUIRE(res1.ans() == before + 5);

    // Student should no longer be enrolled in that course -> completing again fails
    REQUIRE(sys.completeCourse(1, 1) == StatusType::FAILURE);
}

TEST_CASE("v2 removeStudent and removeCourse after completion") {
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

TEST_CASE("v2 awardAcademicPoints: invalid, empty system, new vs existing students") {
    TechSystem sys;

    // Invalid input
    REQUIRE(sys.awardAcademicPoints(0)  == StatusType::INVALID_INPUT);
    REQUIRE(sys.awardAcademicPoints(-5) == StatusType::INVALID_INPUT);

    // No students yet: still SUCCESS (just a no-op logically)
    REQUIRE(sys.awardAcademicPoints(3) == StatusType::SUCCESS);

    // Student added after previous award should NOT get those old points
    REQUIRE(sys.addStudent(1) == StatusType::SUCCESS);
    auto r1 = sys.getStudentPoints(1);
    REQUIRE(r1.status() == StatusType::SUCCESS);
    REQUIRE(r1.ans() == 0);

    // Award points to existing students
    REQUIRE(sys.awardAcademicPoints(10) == StatusType::SUCCESS);

    auto r2 = sys.getStudentPoints(1);
    REQUIRE(r2.status() == StatusType::SUCCESS);
    REQUIRE(r2.ans() == 10);

    // Add another student AFTER the award: should start at 0
    REQUIRE(sys.addStudent(2) == StatusType::SUCCESS);
    auto r3 = sys.getStudentPoints(2);
    REQUIRE(r3.status() == StatusType::SUCCESS);
    REQUIRE(r3.ans() == 0);

    // Another global award affects both current students
    REQUIRE(sys.awardAcademicPoints(2) == StatusType::SUCCESS);

    auto r4 = sys.getStudentPoints(1);
    auto r5 = sys.getStudentPoints(2);
    REQUIRE(r4.status() == StatusType::SUCCESS);
    REQUIRE(r5.status() == StatusType::SUCCESS);
    REQUIRE(r4.ans() == 12); // 10 + 2
    REQUIRE(r5.ans() == 2);  // 0 + 2
}

TEST_CASE("v2 getStudentPoints: invalid, missing, and basic progression") {
    TechSystem sys;

    // Invalid ids
    {
        auto res0 = sys.getStudentPoints(0);
        REQUIRE(res0.status() == StatusType::INVALID_INPUT);
        auto res1 = sys.getStudentPoints(-1);
        REQUIRE(res1.status() == StatusType::INVALID_INPUT);
    }

    // Non-existing student
    {
        auto res = sys.getStudentPoints(5);
        REQUIRE(res.status() == StatusType::FAILURE);
    }

    // Add course and student, then simulate basic progression
    REQUIRE(sys.addCourse(1, 3) == StatusType::SUCCESS);
    REQUIRE(sys.addStudent(1)   == StatusType::SUCCESS);

    // Initial points = 0
    auto res2 = sys.getStudentPoints(1);
    REQUIRE(res2.status() == StatusType::SUCCESS);
    REQUIRE(res2.ans() == 0);

    // Enroll and complete
    REQUIRE(sys.enrollStudent(1,1)     == StatusType::SUCCESS);
    REQUIRE(sys.completeCourse(1,1)    == StatusType::SUCCESS);

    auto res3 = sys.getStudentPoints(1);
    REQUIRE(res3.status() == StatusType::SUCCESS);
    REQUIRE(res3.ans() == 3);

    // Apply academic bonus
    REQUIRE(sys.awardAcademicPoints(4) == StatusType::SUCCESS);
    auto res4 = sys.getStudentPoints(1);
    REQUIRE(res4.status() == StatusType::SUCCESS);
    REQUIRE(res4.ans() == 7);
}

// -----------------------------------------------------------------------------
// EXAMPLE RUN FROM SPEC (sanity check) :contentReference[oaicite:0]{index=0}
// -----------------------------------------------------------------------------

TEST_CASE("v2 Full example sequence from spec") {
    TechSystem sys;

    REQUIRE(sys.addCourse(1, 3)  == StatusType::SUCCESS);
    REQUIRE(sys.addCourse(2, 5)  == StatusType::SUCCESS);
    REQUIRE(sys.addStudent(1)    == StatusType::SUCCESS);
    REQUIRE(sys.addStudent(5)    == StatusType::SUCCESS);

    {
        auto res = sys.getStudentPoints(1);
        REQUIRE(res.status() == StatusType::SUCCESS);
        REQUIRE(res.ans() == 0);
    }

    REQUIRE(sys.enrollStudent(1, 1) == StatusType::SUCCESS);
    REQUIRE(sys.removeStudent(1)    == StatusType::FAILURE);
    REQUIRE(sys.removeCourse(1)     == StatusType::FAILURE);

    {
        auto res = sys.getStudentPoints(1);
        REQUIRE(res.status() == StatusType::SUCCESS);
        REQUIRE(res.ans() == 0);
    }

    REQUIRE(sys.completeCourse(1, 1) == StatusType::SUCCESS);
    REQUIRE(sys.completeCourse(1, 2) == StatusType::FAILURE);
    REQUIRE(sys.removeCourse(1)      == StatusType::SUCCESS);

    {
        auto res = sys.getStudentPoints(1);
        REQUIRE(res.status() == StatusType::SUCCESS);
        REQUIRE(res.ans() == 3);
    }

    REQUIRE(sys.awardAcademicPoints(10) == StatusType::SUCCESS);

    {
        auto res = sys.getStudentPoints(1);
        REQUIRE(res.status() == StatusType::SUCCESS);
        REQUIRE(res.ans() == 13);
    }

    REQUIRE(sys.removeStudent(1) == StatusType::SUCCESS);
    REQUIRE(sys.addStudent(100)  == StatusType::SUCCESS);

    {
        auto res = sys.getStudentPoints(100);
        REQUIRE(res.status() == StatusType::SUCCESS);
        REQUIRE(res.ans() == 0);
    }
}

// -----------------------------------------------------------------------------
// EXTRA EDGE CASES
// -----------------------------------------------------------------------------

TEST_CASE("v2 Student enrolled in multiple courses; independent completions and removals") {
    TechSystem sys;

    REQUIRE(sys.addStudent(1)      == StatusType::SUCCESS);
    REQUIRE(sys.addCourse(10, 2)   == StatusType::SUCCESS);
    REQUIRE(sys.addCourse(20, 5)   == StatusType::SUCCESS);

    // Enroll in both courses
    REQUIRE(sys.enrollStudent(1,10) == StatusType::SUCCESS);
    REQUIRE(sys.enrollStudent(1,20) == StatusType::SUCCESS);

    // While enrolled in ANY course, removeStudent must fail
    REQUIRE(sys.removeStudent(1) == StatusType::FAILURE);

    // Complete only course 10
    REQUIRE(sys.completeCourse(1,10) == StatusType::SUCCESS);

    auto r1 = sys.getStudentPoints(1);
    REQUIRE(r1.status() == StatusType::SUCCESS);
    REQUIRE(r1.ans() == 2);

    // Still enrolled in course 20 => removeCourse(20) must fail, but removeCourse(10) must succeed
    REQUIRE(sys.removeCourse(10) == StatusType::SUCCESS);
    REQUIRE(sys.removeCourse(20) == StatusType::FAILURE);

    // Complete the remaining course 20
    REQUIRE(sys.completeCourse(1,20) == StatusType::SUCCESS);

    auto r2 = sys.getStudentPoints(1);
    REQUIRE(r2.status() == StatusType::SUCCESS);
    REQUIRE(r2.ans() == 7); // 2 + 5

    // Now course 20 has no students -> can remove
    REQUIRE(sys.removeCourse(20) == StatusType::SUCCESS);

    // Student not enrolled anywhere -> can remove
    REQUIRE(sys.removeStudent(1) == StatusType::SUCCESS);
}

TEST_CASE("v2 Many students in one course; removal only when last leaves") {
    TechSystem sys;

    REQUIRE(sys.addCourse(10, 1) == StatusType::SUCCESS);

    // Add several students and enroll them all
    for (int s = 1; s <= 5; ++s) {
        REQUIRE(sys.addStudent(s) == StatusType::SUCCESS);
        REQUIRE(sys.enrollStudent(s,10) == StatusType::SUCCESS);
    }

    // Course cannot be removed while anyone is enrolled
    REQUIRE(sys.removeCourse(10) == StatusType::FAILURE);

    // Complete for first 4 students
    for (int s = 1; s <= 4; ++s) {
        REQUIRE(sys.completeCourse(s,10) == StatusType::SUCCESS);
        auto res = sys.getStudentPoints(s);
        REQUIRE(res.status() == StatusType::SUCCESS);
        REQUIRE(res.ans() == 1);
    }

    // Still one student (5) enrolled -> removal must fail
    REQUIRE(sys.removeCourse(10) == StatusType::FAILURE);

    // Complete last student
    REQUIRE(sys.completeCourse(5,10) == StatusType::SUCCESS);

    // Now no one is enrolled -> removal must succeed
    REQUIRE(sys.removeCourse(10) == StatusType::SUCCESS);
}

TEST_CASE("v2 Failure operations must not change points or enrollment") {
    TechSystem sys;

    REQUIRE(sys.addStudent(1)    == StatusType::SUCCESS);
    REQUIRE(sys.addCourse(10, 7) == StatusType::SUCCESS);

    // Initial: not enrolled, 0 points
    auto base = sys.getStudentPoints(1);
    REQUIRE(base.status() == StatusType::SUCCESS);
    REQUIRE(base.ans() == 0);

    // Failing completeCourse: wrong course / not enrolled
    REQUIRE(sys.completeCourse(1,10) == StatusType::FAILURE);

    auto afterFail1 = sys.getStudentPoints(1);
    REQUIRE(afterFail1.status() == StatusType::SUCCESS);
    REQUIRE(afterFail1.ans() == 0);

    // Enroll correctly
    REQUIRE(sys.enrollStudent(1,10) == StatusType::SUCCESS);

    // Failing enroll: duplicate enrollment
    REQUIRE(sys.enrollStudent(1,10) == StatusType::FAILURE);

    // Points and enrollment are unchanged:
    auto afterFail2 = sys.getStudentPoints(1);
    REQUIRE(afterFail2.status() == StatusType::SUCCESS);
    REQUIRE(afterFail2.ans() == 0);

    // Completing correctly should still work
    REQUIRE(sys.completeCourse(1,10) == StatusType::SUCCESS);
    auto afterComplete = sys.getStudentPoints(1);
    REQUIRE(afterComplete.status() == StatusType::SUCCESS);
    REQUIRE(afterComplete.ans() == 7);
}

TEST_CASE("v2 Re-add a removed student with same ID: new student must not inherit old bonuses") {
    TechSystem sys;

    // Add student and some structure
    REQUIRE(sys.addStudent(1) == StatusType::SUCCESS);
    REQUIRE(sys.addCourse(10, 4) == StatusType::SUCCESS);

    REQUIRE(sys.enrollStudent(1,10) == StatusType::SUCCESS);
    REQUIRE(sys.completeCourse(1,10) == StatusType::SUCCESS); // +4

    REQUIRE(sys.awardAcademicPoints(6) == StatusType::SUCCESS); // +6

    auto beforeRemove = sys.getStudentPoints(1);
    REQUIRE(beforeRemove.status() == StatusType::SUCCESS);
    REQUIRE(beforeRemove.ans() == 10);

    // Student not enrolled anywhere -> can remove
    REQUIRE(sys.removeStudent(1) == StatusType::SUCCESS);

    // Re-add student with same ID -> should behave as a FRESH student (0 points)
    REQUIRE(sys.addStudent(1) == StatusType::SUCCESS);
    auto newStu = sys.getStudentPoints(1);
    REQUIRE(newStu.status() == StatusType::SUCCESS);
    REQUIRE(newStu.ans() == 0);

    // New bonus should affect only this new one
    REQUIRE(sys.awardAcademicPoints(3) == StatusType::SUCCESS);
    auto newStu2 = sys.getStudentPoints(1);
    REQUIRE(newStu2.status() == StatusType::SUCCESS);
    REQUIRE(newStu2.ans() == 3);
}

TEST_CASE("v2 awardAcademicPoints interleaved with enroll/complete on multiple students") {
    TechSystem sys;

    REQUIRE(sys.addCourse(10, 3) == StatusType::SUCCESS);
    REQUIRE(sys.addCourse(20, 4) == StatusType::SUCCESS);

    REQUIRE(sys.addStudent(1) == StatusType::SUCCESS);
    REQUIRE(sys.addStudent(2) == StatusType::SUCCESS);

    // First global award: affects 1 & 2
    REQUIRE(sys.awardAcademicPoints(5) == StatusType::SUCCESS);

    auto p1 = sys.getStudentPoints(1);
    auto p2 = sys.getStudentPoints(2);
    REQUIRE(p1.status() == StatusType::SUCCESS);
    REQUIRE(p2.status() == StatusType::SUCCESS);
    REQUIRE(p1.ans() == 5);
    REQUIRE(p2.ans() == 5);

    // Enroll & complete different courses
    REQUIRE(sys.enrollStudent(1,10) == StatusType::SUCCESS);
    REQUIRE(sys.enrollStudent(2,20) == StatusType::SUCCESS);

    REQUIRE(sys.completeCourse(1,10) == StatusType::SUCCESS); // +3
    REQUIRE(sys.completeCourse(2,20) == StatusType::SUCCESS); // +4

    auto afterCourses1 = sys.getStudentPoints(1);
    auto afterCourses2 = sys.getStudentPoints(2);
    REQUIRE(afterCourses1.status() == StatusType::SUCCESS);
    REQUIRE(afterCourses2.status() == StatusType::SUCCESS);
    REQUIRE(afterCourses1.ans() == 8); // 5 + 3
    REQUIRE(afterCourses2.ans() == 9); // 5 + 4

    // Add a third student AFTER all of this; they don't get past bonuses
    REQUIRE(sys.addStudent(3) == StatusType::SUCCESS);
    auto p3 = sys.getStudentPoints(3);
    REQUIRE(p3.status() == StatusType::SUCCESS);
    REQUIRE(p3.ans() == 0);

    // New global award: affects 1,2,3
    REQUIRE(sys.awardAcademicPoints(2) == StatusType::SUCCESS);

    auto f1 = sys.getStudentPoints(1);
    auto f2 = sys.getStudentPoints(2);
    auto f3 = sys.getStudentPoints(3);
    REQUIRE(f1.status() == StatusType::SUCCESS);
    REQUIRE(f2.status() == StatusType::SUCCESS);
    REQUIRE(f3.status() == StatusType::SUCCESS);

    REQUIRE(f1.ans() == 10); // 8 + 2
    REQUIRE(f2.ans() == 11); // 9 + 2
    REQUIRE(f3.ans() == 2);  // 0 + 2
}

TEST_CASE("v2 getStudentPoints is pure-observer and doesn't change anything") {
    TechSystem sys;

    REQUIRE(sys.addStudent(1) == StatusType::SUCCESS);
    REQUIRE(sys.addCourse(10, 3) == StatusType::SUCCESS);
    REQUIRE(sys.enrollStudent(1,10) == StatusType::SUCCESS);

    // Repeated queries shouldn't affect enrollment or points
    for (int i = 0; i < 10; ++i) {
        auto r = sys.getStudentPoints(1);
        REQUIRE(r.status() == StatusType::SUCCESS);
        REQUIRE(r.ans() == 0);
    }

    // Completing still works as expected
    REQUIRE(sys.completeCourse(1,10) == StatusType::SUCCESS);
    auto after = sys.getStudentPoints(1);
    REQUIRE(after.status() == StatusType::SUCCESS);
    REQUIRE(after.ans() == 3);
}

TEST_CASE("v2 Boundary-ish IDs and mix of failures & successes") {
    TechSystem sys;

    const int bigId = 1'000'000'007; // big but still valid int

    // Big IDs for both student and course
    REQUIRE(sys.addStudent(bigId)       == StatusType::SUCCESS);
    REQUIRE(sys.addCourse(bigId, 2)     == StatusType::SUCCESS);
    REQUIRE(sys.enrollStudent(bigId, bigId) == StatusType::SUCCESS);

    // INVALID_INPUT with different fields should not touch valid bigId student or course
    REQUIRE(sys.enrollStudent(0, bigId)      == StatusType::INVALID_INPUT);
    REQUIRE(sys.completeCourse(-1, bigId)    == StatusType::INVALID_INPUT);
    REQUIRE(sys.enrollStudent(bigId, 0)      == StatusType::INVALID_INPUT);
    REQUIRE(sys.completeCourse(bigId, -1)    == StatusType::INVALID_INPUT);

    // Actual completion still works
    REQUIRE(sys.completeCourse(bigId, bigId) == StatusType::SUCCESS);
    auto r = sys.getStudentPoints(bigId);
    REQUIRE(r.status() == StatusType::SUCCESS);
    REQUIRE(r.ans() == 2);
}

// You can keep adding more very specific sequences if you want to stress-test
// your implementation logic and invariants.
