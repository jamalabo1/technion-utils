//
// Created by jamal on 05/12/2025.
//
#include <catch2/catch_test_macros.hpp>
#include "TechSystem26a1.h"

TEST_CASE("Course does not remove on non-empty") {
    TechSystem sys;

    REQUIRE(sys.addStudent(1) == StatusType::SUCCESS);

    REQUIRE(sys.addCourse(1, 10) == StatusType::SUCCESS);

    REQUIRE(sys.enrollStudent(1, 1) == StatusType::SUCCESS);

    REQUIRE(sys.removeCourse(1) == StatusType::FAILURE);
}

TEST_CASE("Points reset on student re-add") {
    TechSystem sys;

    REQUIRE(sys.addStudent(1) == StatusType::SUCCESS);

    REQUIRE(sys.awardAcademicPoints(1) == StatusType::SUCCESS);

    REQUIRE(sys.getStudentPoints(1).ans() == 1);

    REQUIRE(sys.removeStudent(1) == StatusType::SUCCESS);

    REQUIRE(sys.addStudent(1) == StatusType::SUCCESS);

    REQUIRE(sys.getStudentPoints(1).ans() == 0);
}