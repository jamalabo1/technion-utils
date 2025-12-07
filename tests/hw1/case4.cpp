//
// Created by jamal on 05/12/2025.
//
#define CATCH_CONFIG_MAIN
#include "catch2/catch_test_macros.hpp"
#include "TechSystem26a1.h"

// ============================================================================
// HELPER MACROS
// ============================================================================
#define REQUIRE_SUCCESS(expr) REQUIRE((expr) == StatusType::SUCCESS)
#define REQUIRE_FAILURE(expr) REQUIRE((expr) == StatusType::FAILURE)
#define REQUIRE_INVALID(expr) REQUIRE((expr) == StatusType::INVALID_INPUT)

// ============================================================================
// ADD STUDENT TESTS
// ============================================================================
TEST_CASE("addStudent - Basic Operations", "[addStudent]") {
    TechSystem sys;

    SECTION("Add valid student") {
        REQUIRE_SUCCESS(sys.addStudent(1));
        auto result = sys.getStudentPoints(1);
        REQUIRE(result.status() == StatusType::SUCCESS);
        REQUIRE(result.ans() == 0);
    }

    SECTION("Add multiple students") {
        REQUIRE_SUCCESS(sys.addStudent(1));
        REQUIRE_SUCCESS(sys.addStudent(2));
        REQUIRE_SUCCESS(sys.addStudent(100));
        REQUIRE_SUCCESS(sys.addStudent(999999));
    }

    SECTION("Student starts with 0 points") {
        REQUIRE_SUCCESS(sys.addStudent(42));
        auto result = sys.getStudentPoints(42);
        REQUIRE(result.status() == StatusType::SUCCESS);
        REQUIRE(result.ans() == 0);
    }
}

TEST_CASE("addStudent - Invalid Input", "[addStudent]") {
    TechSystem sys;

    SECTION("Negative student ID") {
        REQUIRE_INVALID(sys.addStudent(-1));
        REQUIRE_INVALID(sys.addStudent(-100));
    }

    SECTION("Zero student ID") {
        REQUIRE_INVALID(sys.addStudent(0));
    }
}

TEST_CASE("addStudent - Duplicate Detection", "[addStudent]") {
    TechSystem sys;

    SECTION("Add same student twice") {
        REQUIRE_SUCCESS(sys.addStudent(1));
        REQUIRE_FAILURE(sys.addStudent(1));
    }

    SECTION("Add multiple duplicates") {
        REQUIRE_SUCCESS(sys.addStudent(5));
        REQUIRE_FAILURE(sys.addStudent(5));
        REQUIRE_FAILURE(sys.addStudent(5));
    }
}

// ============================================================================
// REMOVE STUDENT TESTS
// ============================================================================
TEST_CASE("removeStudent - Basic Operations", "[removeStudent]") {
    TechSystem sys;

    SECTION("Remove existing student") {
        REQUIRE_SUCCESS(sys.addStudent(1));
        REQUIRE_SUCCESS(sys.removeStudent(1));
        REQUIRE_FAILURE(sys.getStudentPoints(1).status());
    }

    SECTION("Remove and re-add student") {
        REQUIRE_SUCCESS(sys.addStudent(1));
        REQUIRE_SUCCESS(sys.removeStudent(1));
        REQUIRE_SUCCESS(sys.addStudent(1));
    }
}

TEST_CASE("removeStudent - Invalid Input", "[removeStudent]") {
    TechSystem sys;

    SECTION("Negative student ID") {
        REQUIRE_INVALID(sys.removeStudent(-1));
    }

    SECTION("Zero student ID") {
        REQUIRE_INVALID(sys.removeStudent(0));
    }
}

TEST_CASE("removeStudent - Non-existent Student", "[removeStudent]") {
    TechSystem sys;

    SECTION("Remove from empty system") {
        REQUIRE_FAILURE(sys.removeStudent(1));
    }

    SECTION("Remove non-existent student") {
        REQUIRE_SUCCESS(sys.addStudent(1));
        REQUIRE_FAILURE(sys.removeStudent(2));
    }
}

TEST_CASE("removeStudent - Enrolled in Courses", "[removeStudent]") {
    TechSystem sys;

    SECTION("Cannot remove student enrolled in one course") {
        REQUIRE_SUCCESS(sys.addStudent(1));
        REQUIRE_SUCCESS(sys.addCourse(101, 3));
        REQUIRE_SUCCESS(sys.enrollStudent(1, 101));
        REQUIRE_FAILURE(sys.removeStudent(1));
    }

    SECTION("Cannot remove student enrolled in multiple courses") {
        REQUIRE_SUCCESS(sys.addStudent(1));
        REQUIRE_SUCCESS(sys.addCourse(101, 3));
        REQUIRE_SUCCESS(sys.addCourse(102, 5));
        REQUIRE_SUCCESS(sys.enrollStudent(1, 101));
        REQUIRE_SUCCESS(sys.enrollStudent(1, 102));
        REQUIRE_FAILURE(sys.removeStudent(1));
    }

    SECTION("Can remove after completing all courses") {
        REQUIRE_SUCCESS(sys.addStudent(1));
        REQUIRE_SUCCESS(sys.addCourse(101, 3));
        REQUIRE_SUCCESS(sys.enrollStudent(1, 101));
        REQUIRE_SUCCESS(sys.completeCourse(1, 101));
        REQUIRE_SUCCESS(sys.removeStudent(1));
    }
}

// ============================================================================
// ADD COURSE TESTS
// ============================================================================
TEST_CASE("addCourse - Basic Operations", "[addCourse]") {
    TechSystem sys;

    SECTION("Add valid course") {
        REQUIRE_SUCCESS(sys.addCourse(101, 3));
    }

    SECTION("Add multiple courses") {
        REQUIRE_SUCCESS(sys.addCourse(101, 3));
        REQUIRE_SUCCESS(sys.addCourse(102, 5));
        REQUIRE_SUCCESS(sys.addCourse(103, 2));
    }

    SECTION("Add courses with various point values") {
        REQUIRE_SUCCESS(sys.addCourse(101, 1));
        REQUIRE_SUCCESS(sys.addCourse(102, 100));
        REQUIRE_SUCCESS(sys.addCourse(103, 999999));
    }
}

TEST_CASE("addCourse - Invalid Input", "[addCourse]") {
    TechSystem sys;

    SECTION("Negative course ID") {
        REQUIRE_INVALID(sys.addCourse(-1, 3));
    }

    SECTION("Zero course ID") {
        REQUIRE_INVALID(sys.addCourse(0, 3));
    }

    SECTION("Negative points") {
        REQUIRE_INVALID(sys.addCourse(101, -1));
    }

    SECTION("Zero points") {
        REQUIRE_INVALID(sys.addCourse(101, 0));
    }

    SECTION("Both invalid") {
        REQUIRE_INVALID(sys.addCourse(-1, -1));
        REQUIRE_INVALID(sys.addCourse(0, 0));
    }
}

TEST_CASE("addCourse - Duplicate Detection", "[addCourse]") {
    TechSystem sys;

    SECTION("Add same course twice") {
        REQUIRE_SUCCESS(sys.addCourse(101, 3));
        REQUIRE_FAILURE(sys.addCourse(101, 5));
    }

    SECTION("Same ID, different points") {
        REQUIRE_SUCCESS(sys.addCourse(101, 3));
        REQUIRE_FAILURE(sys.addCourse(101, 10));
    }
}

// ============================================================================
// REMOVE COURSE TESTS
// ============================================================================
TEST_CASE("removeCourse - Basic Operations", "[removeCourse]") {
    TechSystem sys;

    SECTION("Remove existing course") {
        REQUIRE_SUCCESS(sys.addCourse(101, 3));
        REQUIRE_SUCCESS(sys.removeCourse(101));
    }

    SECTION("Remove and re-add course") {
        REQUIRE_SUCCESS(sys.addCourse(101, 3));
        REQUIRE_SUCCESS(sys.removeCourse(101));
        REQUIRE_SUCCESS(sys.addCourse(101, 5));
    }
}

TEST_CASE("removeCourse - Invalid Input", "[removeCourse]") {
    TechSystem sys;

    SECTION("Negative course ID") {
        REQUIRE_INVALID(sys.removeCourse(-1));
    }

    SECTION("Zero course ID") {
        REQUIRE_INVALID(sys.removeCourse(0));
    }
}

TEST_CASE("removeCourse - Non-existent Course", "[removeCourse]") {
    TechSystem sys;

    SECTION("Remove from empty system") {
        REQUIRE_FAILURE(sys.removeCourse(101));
    }

    SECTION("Remove non-existent course") {
        REQUIRE_SUCCESS(sys.addCourse(101, 3));
        REQUIRE_FAILURE(sys.removeCourse(102));
    }
}

TEST_CASE("removeCourse - Students Enrolled", "[removeCourse]") {
    TechSystem sys;

    SECTION("Cannot remove course with one student") {
        REQUIRE_SUCCESS(sys.addStudent(1));
        REQUIRE_SUCCESS(sys.addCourse(101, 3));
        REQUIRE_SUCCESS(sys.enrollStudent(1, 101));
        REQUIRE_FAILURE(sys.removeCourse(101));
    }

    SECTION("Cannot remove course with multiple students") {
        REQUIRE_SUCCESS(sys.addStudent(1));
        REQUIRE_SUCCESS(sys.addStudent(2));
        REQUIRE_SUCCESS(sys.addCourse(101, 3));
        REQUIRE_SUCCESS(sys.enrollStudent(1, 101));
        REQUIRE_SUCCESS(sys.enrollStudent(2, 101));
        REQUIRE_FAILURE(sys.removeCourse(101));
    }

    SECTION("Can remove after all students complete") {
        REQUIRE_SUCCESS(sys.addStudent(1));
        REQUIRE_SUCCESS(sys.addCourse(101, 3));
        REQUIRE_SUCCESS(sys.enrollStudent(1, 101));
        REQUIRE_SUCCESS(sys.completeCourse(1, 101));
        REQUIRE_SUCCESS(sys.removeCourse(101));
    }
}

// ============================================================================
// ENROLL STUDENT TESTS
// ============================================================================
TEST_CASE("enrollStudent - Basic Operations", "[enrollStudent]") {
    TechSystem sys;

    SECTION("Enroll student in course") {
        REQUIRE_SUCCESS(sys.addStudent(1));
        REQUIRE_SUCCESS(sys.addCourse(101, 3));
        REQUIRE_SUCCESS(sys.enrollStudent(1, 101));
    }

    SECTION("Enroll multiple students in one course") {
        REQUIRE_SUCCESS(sys.addStudent(1));
        REQUIRE_SUCCESS(sys.addStudent(2));
        REQUIRE_SUCCESS(sys.addCourse(101, 3));
        REQUIRE_SUCCESS(sys.enrollStudent(1, 101));
        REQUIRE_SUCCESS(sys.enrollStudent(2, 101));
    }

    SECTION("Enroll one student in multiple courses") {
        REQUIRE_SUCCESS(sys.addStudent(1));
        REQUIRE_SUCCESS(sys.addCourse(101, 3));
        REQUIRE_SUCCESS(sys.addCourse(102, 5));
        REQUIRE_SUCCESS(sys.enrollStudent(1, 101));
        REQUIRE_SUCCESS(sys.enrollStudent(1, 102));
    }
}

TEST_CASE("enrollStudent - Invalid Input", "[enrollStudent]") {
    TechSystem sys;

    SECTION("Invalid student ID") {
        REQUIRE_SUCCESS(sys.addCourse(101, 3));
        REQUIRE_INVALID(sys.enrollStudent(-1, 101));
        REQUIRE_INVALID(sys.enrollStudent(0, 101));
    }

    SECTION("Invalid course ID") {
        REQUIRE_SUCCESS(sys.addStudent(1));
        REQUIRE_INVALID(sys.enrollStudent(1, -1));
        REQUIRE_INVALID(sys.enrollStudent(1, 0));
    }

    SECTION("Both invalid") {
        REQUIRE_INVALID(sys.enrollStudent(-1, -1));
        REQUIRE_INVALID(sys.enrollStudent(0, 0));
    }
}

TEST_CASE("enrollStudent - Non-existent Entities", "[enrollStudent]") {
    TechSystem sys;

    SECTION("Student doesn't exist") {
        REQUIRE_SUCCESS(sys.addCourse(101, 3));
        REQUIRE_FAILURE(sys.enrollStudent(1, 101));
    }

    SECTION("Course doesn't exist") {
        REQUIRE_SUCCESS(sys.addStudent(1));
        REQUIRE_FAILURE(sys.enrollStudent(1, 101));
    }

    SECTION("Neither exist") {
        REQUIRE_FAILURE(sys.enrollStudent(1, 101));
    }
}

TEST_CASE("enrollStudent - Already Enrolled", "[enrollStudent]") {
    TechSystem sys;

    SECTION("Enroll twice in same course") {
        REQUIRE_SUCCESS(sys.addStudent(1));
        REQUIRE_SUCCESS(sys.addCourse(101, 3));
        REQUIRE_SUCCESS(sys.enrollStudent(1, 101));
        REQUIRE_FAILURE(sys.enrollStudent(1, 101));
    }

    SECTION("Enroll, complete, try to enroll again") {
        REQUIRE_SUCCESS(sys.addStudent(1));
        REQUIRE_SUCCESS(sys.addCourse(101, 3));
        REQUIRE_SUCCESS(sys.enrollStudent(1, 101));
        REQUIRE_SUCCESS(sys.completeCourse(1, 101));
        REQUIRE_SUCCESS(sys.enrollStudent(1, 101));
    }
}

// ============================================================================
// COMPLETE COURSE TESTS
// ============================================================================
TEST_CASE("completeCourse - Basic Operations", "[completeCourse]") {
    TechSystem sys;

    SECTION("Complete course and receive points") {
        REQUIRE_SUCCESS(sys.addStudent(1));
        REQUIRE_SUCCESS(sys.addCourse(101, 3));
        REQUIRE_SUCCESS(sys.enrollStudent(1, 101));
        REQUIRE_SUCCESS(sys.completeCourse(1, 101));

        auto result = sys.getStudentPoints(1);
        REQUIRE(result.status() == StatusType::SUCCESS);
        REQUIRE(result.ans() == 3);
    }

    SECTION("Complete multiple courses") {
        REQUIRE_SUCCESS(sys.addStudent(1));
        REQUIRE_SUCCESS(sys.addCourse(101, 3));
        REQUIRE_SUCCESS(sys.addCourse(102, 5));
        REQUIRE_SUCCESS(sys.enrollStudent(1, 101));
        REQUIRE_SUCCESS(sys.enrollStudent(1, 102));
        REQUIRE_SUCCESS(sys.completeCourse(1, 101));
        REQUIRE_SUCCESS(sys.completeCourse(1, 102));

        auto result = sys.getStudentPoints(1);
        REQUIRE(result.status() == StatusType::SUCCESS);
        REQUIRE(result.ans() == 8);
    }
}

TEST_CASE("completeCourse - Invalid Input", "[completeCourse]") {
    TechSystem sys;

    SECTION("Invalid student ID") {
        REQUIRE_SUCCESS(sys.addCourse(101, 3));
        REQUIRE_INVALID(sys.completeCourse(-1, 101));
        REQUIRE_INVALID(sys.completeCourse(0, 101));
    }

    SECTION("Invalid course ID") {
        REQUIRE_SUCCESS(sys.addStudent(1));
        REQUIRE_INVALID(sys.completeCourse(1, -1));
        REQUIRE_INVALID(sys.completeCourse(1, 0));
    }

    SECTION("Both invalid") {
        REQUIRE_INVALID(sys.completeCourse(-1, -1));
        REQUIRE_INVALID(sys.completeCourse(0, 0));
    }
}

TEST_CASE("completeCourse - Non-existent Entities", "[completeCourse]") {
    TechSystem sys;

    SECTION("Student doesn't exist") {
        REQUIRE_SUCCESS(sys.addCourse(101, 3));
        REQUIRE_FAILURE(sys.completeCourse(1, 101));
    }

    SECTION("Course doesn't exist") {
        REQUIRE_SUCCESS(sys.addStudent(1));
        REQUIRE_FAILURE(sys.completeCourse(1, 101));
    }

    SECTION("Neither exist") {
        REQUIRE_FAILURE(sys.completeCourse(1, 101));
    }
}

TEST_CASE("completeCourse - Not Enrolled", "[completeCourse]") {
    TechSystem sys;

    SECTION("Try to complete without enrollment") {
        REQUIRE_SUCCESS(sys.addStudent(1));
        REQUIRE_SUCCESS(sys.addCourse(101, 3));
        REQUIRE_FAILURE(sys.completeCourse(1, 101));
    }

    SECTION("Complete twice") {
        REQUIRE_SUCCESS(sys.addStudent(1));
        REQUIRE_SUCCESS(sys.addCourse(101, 3));
        REQUIRE_SUCCESS(sys.enrollStudent(1, 101));
        REQUIRE_SUCCESS(sys.completeCourse(1, 101));
        REQUIRE_FAILURE(sys.completeCourse(1, 101));
    }
}

TEST_CASE("completeCourse - After Completion", "[completeCourse]") {
    TechSystem sys;

    SECTION("Student can be removed after completing all courses") {
        REQUIRE_SUCCESS(sys.addStudent(1));
        REQUIRE_SUCCESS(sys.addCourse(101, 3));
        REQUIRE_SUCCESS(sys.enrollStudent(1, 101));
        REQUIRE_SUCCESS(sys.completeCourse(1, 101));
        REQUIRE_SUCCESS(sys.removeStudent(1));
    }

    SECTION("Course can be removed after all students complete") {
        REQUIRE_SUCCESS(sys.addStudent(1));
        REQUIRE_SUCCESS(sys.addCourse(101, 3));
        REQUIRE_SUCCESS(sys.enrollStudent(1, 101));
        REQUIRE_SUCCESS(sys.completeCourse(1, 101));
        REQUIRE_SUCCESS(sys.removeCourse(101));
    }
}

// ============================================================================
// AWARD ACADEMIC POINTS TESTS
// ============================================================================
TEST_CASE("awardAcademicPoints - Basic Operations", "[awardAcademicPoints]") {
    TechSystem sys;

    SECTION("Award points to single student") {
        REQUIRE_SUCCESS(sys.addStudent(1));
        REQUIRE_SUCCESS(sys.awardAcademicPoints(10));

        auto result = sys.getStudentPoints(1);
        REQUIRE(result.status() == StatusType::SUCCESS);
        REQUIRE(result.ans() == 10);
    }

    SECTION("Award points to multiple students") {
        REQUIRE_SUCCESS(sys.addStudent(1));
        REQUIRE_SUCCESS(sys.addStudent(2));
        REQUIRE_SUCCESS(sys.awardAcademicPoints(10));

        auto result1 = sys.getStudentPoints(1);
        auto result2 = sys.getStudentPoints(2);
        REQUIRE(result1.ans() == 10);
        REQUIRE(result2.ans() == 10);
    }

    SECTION("Award multiple times") {
        REQUIRE_SUCCESS(sys.addStudent(1));
        REQUIRE_SUCCESS(sys.awardAcademicPoints(5));
        REQUIRE_SUCCESS(sys.awardAcademicPoints(7));

        auto result = sys.getStudentPoints(1);
        REQUIRE(result.ans() == 12);
    }
}

TEST_CASE("awardAcademicPoints - Invalid Input", "[awardAcademicPoints]") {
    TechSystem sys;

    SECTION("Negative points") {
        REQUIRE_INVALID(sys.awardAcademicPoints(-1));
    }

    SECTION("Zero points") {
        REQUIRE_INVALID(sys.awardAcademicPoints(0));
    }
}

TEST_CASE("awardAcademicPoints - New Students", "[awardAcademicPoints]") {
    TechSystem sys;

    SECTION("New students don't get retroactive points") {
        REQUIRE_SUCCESS(sys.addStudent(1));
        REQUIRE_SUCCESS(sys.awardAcademicPoints(10));
        REQUIRE_SUCCESS(sys.addStudent(2));

        auto result1 = sys.getStudentPoints(1);
        auto result2 = sys.getStudentPoints(2);
        REQUIRE(result1.ans() == 10);
        REQUIRE(result2.ans() == 0);
    }

    SECTION("Award before and after adding students") {
        REQUIRE_SUCCESS(sys.addStudent(1));
        REQUIRE_SUCCESS(sys.awardAcademicPoints(5));
        REQUIRE_SUCCESS(sys.addStudent(2));
        REQUIRE_SUCCESS(sys.awardAcademicPoints(3));

        auto result1 = sys.getStudentPoints(1);
        auto result2 = sys.getStudentPoints(2);
        REQUIRE(result1.ans() == 8);
        REQUIRE(result2.ans() == 3);
    }
}

TEST_CASE("awardAcademicPoints - Combined with Course Points", "[awardAcademicPoints]") {
    TechSystem sys;

    SECTION("Award points then complete course") {
        REQUIRE_SUCCESS(sys.addStudent(1));
        REQUIRE_SUCCESS(sys.addCourse(101, 3));
        REQUIRE_SUCCESS(sys.awardAcademicPoints(10));
        REQUIRE_SUCCESS(sys.enrollStudent(1, 101));
        REQUIRE_SUCCESS(sys.completeCourse(1, 101));

        auto result = sys.getStudentPoints(1);
        REQUIRE(result.ans() == 13);
    }

    SECTION("Complete course then award points") {
        REQUIRE_SUCCESS(sys.addStudent(1));
        REQUIRE_SUCCESS(sys.addCourse(101, 3));
        REQUIRE_SUCCESS(sys.enrollStudent(1, 101));
        REQUIRE_SUCCESS(sys.completeCourse(1, 101));
        REQUIRE_SUCCESS(sys.awardAcademicPoints(10));

        auto result = sys.getStudentPoints(1);
        REQUIRE(result.ans() == 13);
    }
}

// ============================================================================
// GET STUDENT POINTS TESTS
// ============================================================================
TEST_CASE("getStudentPoints - Basic Operations", "[getStudentPoints]") {
    TechSystem sys;

    SECTION("Get points for new student") {
        REQUIRE_SUCCESS(sys.addStudent(1));
        auto result = sys.getStudentPoints(1);
        REQUIRE(result.status() == StatusType::SUCCESS);
        REQUIRE(result.ans() == 0);
    }

    SECTION("Get points after completing courses") {
        REQUIRE_SUCCESS(sys.addStudent(1));
        REQUIRE_SUCCESS(sys.addCourse(101, 3));
        REQUIRE_SUCCESS(sys.addCourse(102, 5));
        REQUIRE_SUCCESS(sys.enrollStudent(1, 101));
        REQUIRE_SUCCESS(sys.enrollStudent(1, 102));
        REQUIRE_SUCCESS(sys.completeCourse(1, 101));
        REQUIRE_SUCCESS(sys.completeCourse(1, 102));

        auto result = sys.getStudentPoints(1);
        REQUIRE(result.ans() == 8);
    }
}

TEST_CASE("getStudentPoints - Invalid Input", "[getStudentPoints]") {
    TechSystem sys;

    SECTION("Negative student ID") {
        auto result = sys.getStudentPoints(-1);
        REQUIRE(result.status() == StatusType::INVALID_INPUT);
    }

    SECTION("Zero student ID") {
        auto result = sys.getStudentPoints(0);
        REQUIRE(result.status() == StatusType::INVALID_INPUT);
    }
}

TEST_CASE("getStudentPoints - Non-existent Student", "[getStudentPoints]") {
    TechSystem sys;

    SECTION("Query empty system") {
        auto result = sys.getStudentPoints(1);
        REQUIRE(result.status() == StatusType::FAILURE);
    }

    SECTION("Query non-existent student") {
        REQUIRE_SUCCESS(sys.addStudent(1));
        auto result = sys.getStudentPoints(2);
        REQUIRE(result.status() == StatusType::FAILURE);
    }
}

// ============================================================================
// INTEGRATION TESTS
// ============================================================================
TEST_CASE("Integration - Example from Assignment", "[integration]") {
    TechSystem sys;

    REQUIRE_SUCCESS(sys.addCourse(1, 3));
    REQUIRE_SUCCESS(sys.addCourse(2, 5));
    REQUIRE_SUCCESS(sys.addStudent(1));
    REQUIRE_SUCCESS(sys.addStudent(5));

    auto result = sys.getStudentPoints(1);
    REQUIRE(result.status() == StatusType::SUCCESS);
    REQUIRE(result.ans() == 0);

    REQUIRE_SUCCESS(sys.enrollStudent(1, 1));
    REQUIRE_FAILURE(sys.removeStudent(1));
    REQUIRE_FAILURE(sys.removeCourse(1));

    auto result2 = sys.getStudentPoints(1);
    REQUIRE(result2.ans() == 0);

    REQUIRE_SUCCESS(sys.completeCourse(1, 1));
    REQUIRE_FAILURE(sys.completeCourse(1, 2));
    REQUIRE_SUCCESS(sys.removeCourse(1));

    auto result3 = sys.getStudentPoints(1);
    REQUIRE(result3.ans() == 3);

    REQUIRE_SUCCESS(sys.awardAcademicPoints(10));

    auto result4 = sys.getStudentPoints(1);
    REQUIRE(result4.ans() == 13);

    REQUIRE_SUCCESS(sys.removeStudent(1));
    REQUIRE_SUCCESS(sys.addStudent(100));

    auto result5 = sys.getStudentPoints(100);
    REQUIRE(result5.status() == StatusType::SUCCESS);
    REQUIRE(result5.ans() == 0);
}

TEST_CASE("Integration - Complex Scenario", "[integration]") {
    TechSystem sys;

    // Setup
    for (int i = 1; i <= 10; i++) {
        REQUIRE_SUCCESS(sys.addStudent(i));
    }
    for (int i = 101; i <= 105; i++) {
        REQUIRE_SUCCESS(sys.addCourse(i, i % 3 + 1));
    }

    // Award points before any enrollments
    REQUIRE_SUCCESS(sys.awardAcademicPoints(4));

    // Enroll students in various courses
    REQUIRE_SUCCESS(sys.enrollStudent(1, 101));
    REQUIRE_SUCCESS(sys.enrollStudent(1, 102));
    REQUIRE_SUCCESS(sys.enrollStudent(2, 101));
    REQUIRE_SUCCESS(sys.enrollStudent(3, 103));

    // Complete some courses
    REQUIRE_SUCCESS(sys.completeCourse(1, 101));
    auto result = sys.getStudentPoints(1);
    REQUIRE(result.ans() == 7); // 4 (award) + 3 (course 101: 101%3+1)

    // Award more points
    REQUIRE_SUCCESS(sys.awardAcademicPoints(3));
    auto result2 = sys.getStudentPoints(1);
    REQUIRE(result2.ans() == 10);

    // Add new student (shouldn't get previous awards)
    REQUIRE_SUCCESS(sys.addStudent(11));
    auto result11 = sys.getStudentPoints(11);
    REQUIRE(result11.ans() == 0);

    // Award again
    REQUIRE_SUCCESS(sys.awardAcademicPoints(2));
    auto result3 = sys.getStudentPoints(1);
    auto result113 = sys.getStudentPoints(11);
    REQUIRE(result3.ans() == 12);
    REQUIRE(result113.ans() == 2);
}

TEST_CASE("Integration - Stress Test", "[integration][stress]") {
    TechSystem sys;

    const int NUM_STUDENTS = 100;
    const int NUM_COURSES = 50;

    // Add all students
    for (int i = 1; i <= NUM_STUDENTS; i++) {
        REQUIRE_SUCCESS(sys.addStudent(i));
    }

    // Add all courses
    for (int i = 1; i <= NUM_COURSES; i++) {
        REQUIRE_SUCCESS(sys.addCourse(i, i));
    }

    // Enroll students
    for (int s = 1; s <= NUM_STUDENTS; s++) {
        for (int c = 1; c <= std::min(5, NUM_COURSES); c++) {
            REQUIRE_SUCCESS(sys.enrollStudent(s, c));
        }
    }

    // Complete courses for some students
    for (int s = 1; s <= NUM_STUDENTS / 2; s++) {
        for (int c = 1; c <= std::min(3, NUM_COURSES); c++) {
            REQUIRE_SUCCESS(sys.completeCourse(s, c));
        }
    }

    // Award points
    REQUIRE_SUCCESS(sys.awardAcademicPoints(100));

    // Verify points
    auto result1 = sys.getStudentPoints(1);
    REQUIRE(result1.status() == StatusType::SUCCESS);
    REQUIRE(result1.ans() == 106); // 100 + (1+2+3)
}

TEST_CASE("Edge Cases - Boundary Values", "[edge]") {
    TechSystem sys;

    SECTION("Large IDs") {
        REQUIRE_SUCCESS(sys.addStudent(2147483647));
        REQUIRE_SUCCESS(sys.addCourse(2147483647, 1));
    }

    SECTION("Large points") {
        REQUIRE_SUCCESS(sys.addCourse(1, 2147483647));
        REQUIRE_SUCCESS(sys.addStudent(1));
        REQUIRE_SUCCESS(sys.enrollStudent(1, 1));
        REQUIRE_SUCCESS(sys.completeCourse(1, 1));
        auto result = sys.getStudentPoints(1);
        REQUIRE(result.ans() == 2147483647);
    }
}

TEST_CASE("Edge Cases - Empty Operations", "[edge]") {
    TechSystem sys;

    SECTION("Award points to empty system") {
        REQUIRE_SUCCESS(sys.awardAcademicPoints(10));
    }

    SECTION("Operations on empty system") {
        REQUIRE_FAILURE(sys.removeStudent(1));
        REQUIRE_FAILURE(sys.removeCourse(1));
        REQUIRE_FAILURE(sys.enrollStudent(1, 1));
        REQUIRE_FAILURE(sys.completeCourse(1, 1));
    }
}

TEST_CASE("Memory Management - No Leaks", "[memory]") {
    TechSystem sys;

    SECTION("Add and remove many students") {
        for (int i = 1; i <= 1000; i++) {
            REQUIRE_SUCCESS(sys.addStudent(i));
        }
        for (int i = 1; i <= 1000; i++) {
            REQUIRE_SUCCESS(sys.removeStudent(i));
        }
    }

    SECTION("Add and remove many courses") {
        for (int i = 1; i <= 1000; i++) {
            REQUIRE_SUCCESS(sys.addCourse(i, 3));
        }
        for (int i = 1; i <= 1000; i++) {
            REQUIRE_SUCCESS(sys.removeCourse(i));
        }
    }

    SECTION("Complex enrollment scenario") {
        for (int i = 1; i <= 100; i++) {
            REQUIRE_SUCCESS(sys.addStudent(i));
            REQUIRE_SUCCESS(sys.addCourse(i, 3));
        }

        for (int i = 1; i <= 100; i++) {
            for (int j = 1; j <= 10 && j <= 100; j++) {
                REQUIRE_SUCCESS(sys.enrollStudent(i, j));
            }
        }

        for (int i = 1; i <= 100; i++) {
            for (int j = 1; j <= 10 && j <= 100; j++) {
                REQUIRE_SUCCESS(sys.completeCourse(i, j));
            }
        }

        for (int i = 1; i <= 100; i++) {
            REQUIRE_SUCCESS(sys.removeStudent(i));
            REQUIRE_SUCCESS(sys.removeCourse(i));
        }
    }
}

TEST_CASE("Award Points - Complex Scenarios", "[awardPoints]") {
    TechSystem sys;

    SECTION("Multiple awards with interleaved student additions") {
        REQUIRE_SUCCESS(sys.addStudent(1));
        REQUIRE_SUCCESS(sys.awardAcademicPoints(10));

        REQUIRE_SUCCESS(sys.addStudent(2));
        REQUIRE_SUCCESS(sys.awardAcademicPoints(5));

        REQUIRE_SUCCESS(sys.addStudent(3));
        REQUIRE_SUCCESS(sys.awardAcademicPoints(7));

        auto r1 = sys.getStudentPoints(1);
        auto r2 = sys.getStudentPoints(2);
        auto r3 = sys.getStudentPoints(3);

        REQUIRE(r1.ans() == 22); // 10 + 5 + 7
        REQUIRE(r2.ans() == 12); // 5 + 7
        REQUIRE(r3.ans() == 7);  // 7
    }

    SECTION("Award with course completion") {
        REQUIRE_SUCCESS(sys.addStudent(1));
        REQUIRE_SUCCESS(sys.addCourse(101, 3));
        REQUIRE_SUCCESS(sys.addCourse(102, 5));

        REQUIRE_SUCCESS(sys.awardAcademicPoints(10));
        REQUIRE_SUCCESS(sys.enrollStudent(1, 101));
        REQUIRE_SUCCESS(sys.completeCourse(1, 101));
        REQUIRE_SUCCESS(sys.awardAcademicPoints(7));
        REQUIRE_SUCCESS(sys.enrollStudent(1, 102));
        REQUIRE_SUCCESS(sys.completeCourse(1, 102));

        auto result = sys.getStudentPoints(1);
        REQUIRE(result.ans() == 25); // 10 + 3 + 7 + 5
    }
}

TEST_CASE("Ordering - Multiple Operations", "[ordering]") {
    TechSystem sys;

    SECTION("Enroll, remove course should fail, complete, remove should succeed") {
        REQUIRE_SUCCESS(sys.addStudent(1));
        REQUIRE_SUCCESS(sys.addCourse(101, 3));
        REQUIRE_SUCCESS(sys.enrollStudent(1, 101));
        REQUIRE_FAILURE(sys.removeCourse(101));
        REQUIRE_SUCCESS(sys.completeCourse(1, 101));
        REQUIRE_SUCCESS(sys.removeCourse(101));
    }

    SECTION("Multiple students, remove order matters") {
        REQUIRE_SUCCESS(sys.addStudent(1));
        REQUIRE_SUCCESS(sys.addStudent(2));
        REQUIRE_SUCCESS(sys.addCourse(101, 3));

        REQUIRE_SUCCESS(sys.enrollStudent(1, 101));
        REQUIRE_SUCCESS(sys.enrollStudent(2, 101));

        REQUIRE_FAILURE(sys.removeCourse(101));

        REQUIRE_SUCCESS(sys.completeCourse(1, 101));
        REQUIRE_FAILURE(sys.removeCourse(101));

        REQUIRE_SUCCESS(sys.completeCourse(2, 101));
        REQUIRE_SUCCESS(sys.removeCourse(101));
    }
}