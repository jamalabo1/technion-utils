//
// Created by jamal on 14/01/2026.
//
// test_huntech_gtest.cpp
// GoogleTest port of the provided Catch2 tests.

#include <gtest/gtest.h>

#include <sstream>
#include <string>

#include "Huntech26a2.h"

using std::string;

static string nenToString(const NenAbility& n) {
    std::ostringstream oss;
    oss << n;
    return oss.str();
}

static NenAbility nen(const char* type) {
    return NenAbility(std::string(type));
}

template <typename T>
static void expectOutput(output_t<T> out, StatusType expectedStatus, const T& expectedAns) {
    EXPECT_EQ(out.status(), expectedStatus);
    if (expectedStatus == StatusType::SUCCESS) {
        EXPECT_EQ(out.ans(), expectedAns);
    }
}

// For NenAbility, equality operator is non-standard (matchup-based). Compare by printed vector.
static void expectOutputNen(output_t<NenAbility> out, StatusType expectedStatus, const NenAbility& expectedAns) {
    EXPECT_EQ(out.status(), expectedStatus);
    if (expectedStatus == StatusType::SUCCESS) {
        EXPECT_EQ(nenToString(out.ans()), nenToString(expectedAns));
    }
}

// -------------------------
// cases_2: Empty system
// -------------------------

TEST(EmptySystem, GetIthCollectiveAuraSquad_FailsWhenNoSquads) {
    Huntech h;
    auto out = h.get_ith_collective_aura_squad(1);
    EXPECT_EQ(out.status(), StatusType::FAILURE);
}

TEST(EmptySystem, RemoveSquad_FailureAndInvalidInput) {
    Huntech h;
    EXPECT_EQ(h.remove_squad(1), StatusType::FAILURE);
    EXPECT_EQ(h.remove_squad(-7), StatusType::INVALID_INPUT);
    EXPECT_EQ(h.remove_squad(0), StatusType::INVALID_INPUT);
}

TEST(EmptySystem, GetSquadExperience_FailureAndInvalidInput) {
    Huntech h;
    EXPECT_EQ(h.get_squad_experience(1).status(), StatusType::FAILURE);
    EXPECT_EQ(h.get_squad_experience(0).status(), StatusType::INVALID_INPUT);
    EXPECT_EQ(h.get_squad_experience(-1).status(), StatusType::INVALID_INPUT);
}

TEST(EmptySystem, GetHunterFightsNumber_FailureAndInvalidInput) {
    Huntech h;
    EXPECT_EQ(h.get_hunter_fights_number(1).status(), StatusType::FAILURE);
    EXPECT_EQ(h.get_hunter_fights_number(0).status(), StatusType::INVALID_INPUT);
    EXPECT_EQ(h.get_hunter_fights_number(-1).status(), StatusType::INVALID_INPUT);
}

TEST(EmptySystem, GetPartialNenAbility_FailureAndInvalidInput) {
    Huntech h;
    EXPECT_EQ(h.get_partial_nen_ability(123).status(), StatusType::FAILURE);
    EXPECT_EQ(h.get_partial_nen_ability(0).status(), StatusType::INVALID_INPUT);
    EXPECT_EQ(h.get_partial_nen_ability(-1).status(), StatusType::INVALID_INPUT);
}

TEST(EmptySystem, SquadDuel_InvalidInputAndMissingSquads) {
    Huntech h;

    EXPECT_EQ(h.squad_duel(1, 1).status(), StatusType::INVALID_INPUT);
    EXPECT_EQ(h.squad_duel(0, 1).status(), StatusType::INVALID_INPUT);
    EXPECT_EQ(h.squad_duel(1, 0).status(), StatusType::INVALID_INPUT);
    EXPECT_EQ(h.squad_duel(-1, 2).status(), StatusType::INVALID_INPUT);

    EXPECT_EQ(h.squad_duel(1, 2).status(), StatusType::FAILURE);
}

TEST(EmptySystem, ForceJoin_InvalidInputAndMissingSquads) {
    Huntech h;

    EXPECT_EQ(h.force_join(1, 1), StatusType::INVALID_INPUT);
    EXPECT_EQ(h.force_join(0, 1), StatusType::INVALID_INPUT);
    EXPECT_EQ(h.force_join(1, 0), StatusType::INVALID_INPUT);
    EXPECT_EQ(h.force_join(-1, 2), StatusType::INVALID_INPUT);

    EXPECT_EQ(h.force_join(1, 2), StatusType::FAILURE);
}

// -------------------------
// cases_2: add_squad / remove_squad basics
// -------------------------

TEST(SquadBasics, AddSquad_InvalidInput) {
    Huntech h;
    EXPECT_EQ(h.add_squad(0), StatusType::INVALID_INPUT);
    EXPECT_EQ(h.add_squad(-5), StatusType::INVALID_INPUT);
}

TEST(SquadBasics, AddSquad_SuccessThenDuplicateFails) {
    Huntech h;
    EXPECT_EQ(h.add_squad(10), StatusType::SUCCESS);
    EXPECT_EQ(h.add_squad(10), StatusType::FAILURE);
}

TEST(SquadBasics, RemoveSquad_SuccessAndIdReusableAsNewSquad) {
    Huntech h;

    EXPECT_EQ(h.add_squad(7), StatusType::SUCCESS);
    EXPECT_EQ(h.remove_squad(7), StatusType::SUCCESS);

    // After deletion, the same ID may be added again as a new active squad (fresh exp=0, empty)
    EXPECT_EQ(h.add_squad(7), StatusType::SUCCESS);
    expectOutput<int>(h.get_squad_experience(7), StatusType::SUCCESS, 0);
}

// -------------------------
// cases_2: add_hunter validation / uniqueness
// -------------------------

TEST(AddHunter, InvalidInputs) {
    Huntech h;
    ASSERT_EQ(h.add_squad(1), StatusType::SUCCESS);

    EXPECT_EQ(h.add_hunter(0, 1, nen("Enhancer"), 0, 0), StatusType::INVALID_INPUT);
    EXPECT_EQ(h.add_hunter(-1, 1, nen("Enhancer"), 0, 0), StatusType::INVALID_INPUT);
    EXPECT_EQ(h.add_hunter(1, 0, nen("Enhancer"), 0, 0), StatusType::INVALID_INPUT);
    EXPECT_EQ(h.add_hunter(1, -3, nen("Enhancer"), 0, 0), StatusType::INVALID_INPUT);

    EXPECT_EQ(h.add_hunter(1, 1, NenAbility("NotAType"), 0, 0), StatusType::INVALID_INPUT);
    EXPECT_EQ(h.add_hunter(1, 1, nen("Enhancer"), -1, 0), StatusType::INVALID_INPUT);
    EXPECT_EQ(h.add_hunter(1, 1, nen("Enhancer"), 0, -1), StatusType::INVALID_INPUT);
}

TEST(AddHunter, FailsWhenSquadDoesNotExist) {
    Huntech h;
    ASSERT_EQ(h.add_squad(1), StatusType::SUCCESS);
    EXPECT_EQ(h.add_hunter(1, 999, nen("Enhancer"), 0, 0), StatusType::FAILURE);
}

TEST(AddHunter, SuccessAndHunterIdCannotBeReusedEvenDifferentSquad) {
    Huntech h;
    ASSERT_EQ(h.add_squad(1), StatusType::SUCCESS);

    EXPECT_EQ(h.add_hunter(100, 1, nen("Enhancer"), 5, 2), StatusType::SUCCESS);

    EXPECT_EQ(h.add_hunter(100, 1, nen("Emitter"), 3, 0), StatusType::FAILURE);

    ASSERT_EQ(h.add_squad(2), StatusType::SUCCESS);
    EXPECT_EQ(h.add_hunter(100, 2, nen("Emitter"), 3, 0), StatusType::FAILURE);

    expectOutput<int>(h.get_hunter_fights_number(100), StatusType::SUCCESS, 2);
    expectOutputNen(h.get_partial_nen_ability(100), StatusType::SUCCESS, nen("Enhancer"));
}

// -------------------------
// cases_2: get_ith_collective_aura_squad ordering
// -------------------------

class IthAuraOrdering : public ::testing::Test {
protected:
    Huntech h;
    void SetUp() override {
        ASSERT_EQ(h.add_squad(1), StatusType::SUCCESS);
        ASSERT_EQ(h.add_squad(2), StatusType::SUCCESS);
        ASSERT_EQ(h.add_squad(3), StatusType::SUCCESS);
    }
};

TEST_F(IthAuraOrdering, InitiallyAllZero_TieBreakBySquadId_BoundsFailure) {
    expectOutput<int>(h.get_ith_collective_aura_squad(1), StatusType::SUCCESS, 1);
    expectOutput<int>(h.get_ith_collective_aura_squad(2), StatusType::SUCCESS, 2);
    expectOutput<int>(h.get_ith_collective_aura_squad(3), StatusType::SUCCESS, 3);

    EXPECT_EQ(h.get_ith_collective_aura_squad(0).status(), StatusType::FAILURE);
    EXPECT_EQ(h.get_ith_collective_aura_squad(-7).status(), StatusType::FAILURE);
    EXPECT_EQ(h.get_ith_collective_aura_squad(4).status(), StatusType::FAILURE);
}

TEST_F(IthAuraOrdering, OrderingUpdatesAfterAddingHunters_TieBreakById) {
    ASSERT_EQ(h.add_hunter(10, 1, nen("Enhancer"), 5, 0), StatusType::SUCCESS); // aura1=5
    ASSERT_EQ(h.add_hunter(20, 2, nen("Conjurer"), 5, 0), StatusType::SUCCESS); // aura2=5
    // squad3 aura=0

    // by total aura ascending: squad3(0), squad1(5), squad2(5; id bigger)
    expectOutput<int>(h.get_ith_collective_aura_squad(1), StatusType::SUCCESS, 3);
    expectOutput<int>(h.get_ith_collective_aura_squad(2), StatusType::SUCCESS, 1);
    expectOutput<int>(h.get_ith_collective_aura_squad(3), StatusType::SUCCESS, 2);

    ASSERT_EQ(h.add_hunter(21, 2, nen("Emitter"), 1, 0), StatusType::SUCCESS); // aura2=6

    expectOutput<int>(h.get_ith_collective_aura_squad(1), StatusType::SUCCESS, 3);
    expectOutput<int>(h.get_ith_collective_aura_squad(2), StatusType::SUCCESS, 1);
    expectOutput<int>(h.get_ith_collective_aura_squad(3), StatusType::SUCCESS, 2);
}

TEST_F(IthAuraOrdering, RemovingSquadRemovesItFromOrdering) {
    ASSERT_EQ(h.add_hunter(10, 1, nen("Enhancer"), 5, 0), StatusType::SUCCESS);
    ASSERT_EQ(h.add_hunter(20, 2, nen("Conjurer"), 1, 0), StatusType::SUCCESS);

    ASSERT_EQ(h.remove_squad(2), StatusType::SUCCESS);

    // remaining: squads 1 (aura 5), 3 (aura 0)
    expectOutput<int>(h.get_ith_collective_aura_squad(1), StatusType::SUCCESS, 3);
    expectOutput<int>(h.get_ith_collective_aura_squad(2), StatusType::SUCCESS, 1);
    EXPECT_EQ(h.get_ith_collective_aura_squad(3).status(), StatusType::FAILURE);
}

// -------------------------
// cases_2: squad_duel
// -------------------------

class SquadDuel : public ::testing::Test {
protected:
    Huntech h;
    void SetUp() override {
        ASSERT_EQ(h.add_squad(1), StatusType::SUCCESS);
        ASSERT_EQ(h.add_squad(2), StatusType::SUCCESS);
    }
};

TEST_F(SquadDuel, FailsIfOneSquadEmpty_IncludingBothEmpty) {
    EXPECT_EQ(h.squad_duel(1, 2).status(), StatusType::FAILURE);

    ASSERT_EQ(h.add_hunter(10, 1, nen("Enhancer"), 1, 0), StatusType::SUCCESS);
    EXPECT_EQ(h.squad_duel(1, 2).status(), StatusType::FAILURE);
}

TEST_F(SquadDuel, AuraBasedWin_Code3_WinnerExpPlus3_AllFightsPlus1) {
    ASSERT_EQ(h.add_hunter(11, 1, nen("Enhancer"), 5, 2), StatusType::SUCCESS);
    ASSERT_EQ(h.add_hunter(21, 2, nen("Emitter"), 6, 7), StatusType::SUCCESS);

    auto duel = h.squad_duel(1, 2);
    EXPECT_EQ(duel.status(), StatusType::SUCCESS);
    EXPECT_EQ(duel.ans(), 3);

    expectOutput<int>(h.get_squad_experience(1), StatusType::SUCCESS, 0);
    expectOutput<int>(h.get_squad_experience(2), StatusType::SUCCESS, 3);

    expectOutput<int>(h.get_hunter_fights_number(11), StatusType::SUCCESS, 3);
    expectOutput<int>(h.get_hunter_fights_number(21), StatusType::SUCCESS, 8);
}

TEST_F(SquadDuel, FullTie_Code0_BothExpPlus1_AllFightsPlus1) {
    ASSERT_EQ(h.add_hunter(1, 1, nen("Enhancer"), 0, 0), StatusType::SUCCESS);
    ASSERT_EQ(h.add_hunter(2, 2, nen("Enhancer"), 0, 0), StatusType::SUCCESS);

    auto duel = h.squad_duel(1, 2);
    EXPECT_EQ(duel.status(), StatusType::SUCCESS);
    EXPECT_EQ(duel.ans(), 0);

    expectOutput<int>(h.get_squad_experience(1), StatusType::SUCCESS, 1);
    expectOutput<int>(h.get_squad_experience(2), StatusType::SUCCESS, 1);

    expectOutput<int>(h.get_hunter_fights_number(1), StatusType::SUCCESS, 1);
    expectOutput<int>(h.get_hunter_fights_number(2), StatusType::SUCCESS, 1);
}

TEST_F(SquadDuel, NenBasedWin_WhenAuraTies_Code2Or4Or0_ExpUpdates_AllFightsPlus1) {
    ASSERT_EQ(h.add_hunter(1, 1, nen("Enhancer"), 0, 0), StatusType::SUCCESS);
    ASSERT_EQ(h.add_hunter(2, 2, nen("Conjurer"), 0, 0), StatusType::SUCCESS);

    NenAbility squad1Nen = nen("Enhancer");
    NenAbility squad2Nen = nen("Conjurer");

    int expectedCode = 0;
    if (squad1Nen > squad2Nen) expectedCode = 2;
    else if (squad2Nen > squad1Nen) expectedCode = 4;
    else expectedCode = 0;

    auto duel = h.squad_duel(1, 2);
    EXPECT_EQ(duel.status(), StatusType::SUCCESS);
    EXPECT_EQ(duel.ans(), expectedCode);

    if (expectedCode == 2) {
        expectOutput<int>(h.get_squad_experience(1), StatusType::SUCCESS, 3);
        expectOutput<int>(h.get_squad_experience(2), StatusType::SUCCESS, 0);
    } else if (expectedCode == 4) {
        expectOutput<int>(h.get_squad_experience(1), StatusType::SUCCESS, 0);
        expectOutput<int>(h.get_squad_experience(2), StatusType::SUCCESS, 3);
    } else {
        expectOutput<int>(h.get_squad_experience(1), StatusType::SUCCESS, 1);
        expectOutput<int>(h.get_squad_experience(2), StatusType::SUCCESS, 1);
    }

    expectOutput<int>(h.get_hunter_fights_number(1), StatusType::SUCCESS, 1);
    expectOutput<int>(h.get_hunter_fights_number(2), StatusType::SUCCESS, 1);
}

// -------------------------
// cases_2: get_partial_nen_ability
// -------------------------

TEST(PartialNen, ChronologicalPrefixWithinSquad) {
    Huntech h;
    ASSERT_EQ(h.add_squad(1), StatusType::SUCCESS);

    ASSERT_EQ(h.add_hunter(1, 1, nen("Enhancer"), 0, 0), StatusType::SUCCESS);
    ASSERT_EQ(h.add_hunter(2, 1, nen("Emitter"), 0, 0), StatusType::SUCCESS);
    ASSERT_EQ(h.add_hunter(3, 1, nen("Enhancer"), 0, 0), StatusType::SUCCESS);

    NenAbility expected1 = nen("Enhancer");
    NenAbility expected2 = nen("Enhancer") + nen("Emitter");
    NenAbility expected3 = nen("Enhancer") + nen("Emitter") + nen("Enhancer");

    expectOutputNen(h.get_partial_nen_ability(1), StatusType::SUCCESS, expected1);
    expectOutputNen(h.get_partial_nen_ability(2), StatusType::SUCCESS, expected2);
    expectOutputNen(h.get_partial_nen_ability(3), StatusType::SUCCESS, expected3);
}

// -------------------------
// cases_2: force_join success
// -------------------------

TEST(ForceJoin, SuccessMergesSquads_PreservesFights_ConcatsOrder_RemovesForcedSquad) {
    Huntech h;

    ASSERT_EQ(h.add_squad(1), StatusType::SUCCESS);
    ASSERT_EQ(h.add_squad(2), StatusType::SUCCESS);

    ASSERT_EQ(h.add_hunter(10, 1, nen("Enhancer"), 1, 0), StatusType::SUCCESS);
    ASSERT_EQ(h.add_hunter(11, 1, nen("Emitter"), 1, 0), StatusType::SUCCESS);

    ASSERT_EQ(h.add_hunter(20, 2, nen("Transmuter"), 0, 0), StatusType::SUCCESS);

    ASSERT_EQ(h.force_join(1, 2), StatusType::SUCCESS);

    EXPECT_EQ(h.get_squad_experience(2).status(), StatusType::FAILURE);

    // fights unchanged by force_join
    expectOutput<int>(h.get_hunter_fights_number(10), StatusType::SUCCESS, 0);
    expectOutput<int>(h.get_hunter_fights_number(11), StatusType::SUCCESS, 0);
    expectOutput<int>(h.get_hunter_fights_number(20), StatusType::SUCCESS, 0);

    // Chronological concatenation: (10, 11, 20)
    NenAbility expected10 = nen("Enhancer");
    NenAbility expected11 = nen("Enhancer") + nen("Emitter");
    NenAbility expected20 = nen("Enhancer") + nen("Emitter") + nen("Transmuter");

    expectOutputNen(h.get_partial_nen_ability(10), StatusType::SUCCESS, expected10);
    expectOutputNen(h.get_partial_nen_ability(11), StatusType::SUCCESS, expected11);
    expectOutputNen(h.get_partial_nen_ability(20), StatusType::SUCCESS, expected20);

    // After force_join, forced squadId should no longer be active and can be added again
    ASSERT_EQ(h.add_squad(2), StatusType::SUCCESS);
    expectOutput<int>(h.get_squad_experience(2), StatusType::SUCCESS, 0);
}

// -------------------------
// cases_2: force_join failures
// -------------------------

TEST(ForceJoin, Failure_ForcingSquadEmpty) {
    Huntech h;
    ASSERT_EQ(h.add_squad(1), StatusType::SUCCESS);
    ASSERT_EQ(h.add_squad(2), StatusType::SUCCESS);

    ASSERT_EQ(h.add_hunter(200, 2, nen("Enhancer"), 0, 0), StatusType::SUCCESS);
    EXPECT_EQ(h.force_join(1, 2), StatusType::FAILURE);
}

TEST(ForceJoin, Failure_NonStrictInequality) {
    Huntech h;
    ASSERT_EQ(h.add_squad(1), StatusType::SUCCESS);
    ASSERT_EQ(h.add_squad(2), StatusType::SUCCESS);

    // Both: exp=0, totalAura=0, effectiveNenAbility=1 => equality => should fail
    ASSERT_EQ(h.add_hunter(1, 1, nen("Enhancer"), 0, 0), StatusType::SUCCESS);
    ASSERT_EQ(h.add_hunter(2, 2, nen("Emitter"), 0, 0), StatusType::SUCCESS);

    EXPECT_EQ(h.force_join(1, 2), StatusType::FAILURE);
    EXPECT_EQ(h.force_join(2, 1), StatusType::FAILURE);
}

TEST(ForceJoin, Failure_MissingSquads) {
    Huntech h;
    ASSERT_EQ(h.add_squad(1), StatusType::SUCCESS);
    ASSERT_EQ(h.add_squad(2), StatusType::SUCCESS);

    EXPECT_EQ(h.force_join(1, 999), StatusType::FAILURE);
    EXPECT_EQ(h.force_join(999, 1), StatusType::FAILURE);
}

// -------------------------
// cases_2: remove_squad kills hunters behavior
// -------------------------

TEST(RemoveSquadKillsHunters, DeadHuntersKeepFightsButPartialNenFails_IdNotReusable) {
    Huntech h;

    ASSERT_EQ(h.add_squad(1), StatusType::SUCCESS);
    ASSERT_EQ(h.add_hunter(10, 1, nen("Enhancer"), 0, 5), StatusType::SUCCESS);

    expectOutput<int>(h.get_hunter_fights_number(10), StatusType::SUCCESS, 5);
    expectOutputNen(h.get_partial_nen_ability(10), StatusType::SUCCESS, nen("Enhancer"));

    ASSERT_EQ(h.remove_squad(1), StatusType::SUCCESS);

    EXPECT_EQ(h.get_squad_experience(1).status(), StatusType::FAILURE);

    expectOutput<int>(h.get_hunter_fights_number(10), StatusType::SUCCESS, 5);
    EXPECT_EQ(h.get_partial_nen_ability(10).status(), StatusType::FAILURE);

    ASSERT_EQ(h.add_squad(1), StatusType::SUCCESS);
    EXPECT_EQ(h.add_hunter(10, 1, nen("Emitter"), 0, 0), StatusType::FAILURE);
}

// -------------------------
// cases_2: Integration sanity
// -------------------------

TEST(IntegrationSanity, DuelUpdatesFightsExp_OrderingOk_ForceJoinNoCorruption) {
    Huntech h;

    ASSERT_EQ(h.add_squad(1), StatusType::SUCCESS);
    ASSERT_EQ(h.add_squad(2), StatusType::SUCCESS);
    ASSERT_EQ(h.add_squad(3), StatusType::SUCCESS);

    ASSERT_EQ(h.add_hunter(101, 1, nen("Enhancer"), 2, 0), StatusType::SUCCESS);
    ASSERT_EQ(h.add_hunter(201, 2, nen("Emitter"), 2, 0), StatusType::SUCCESS);
    // squad3 empty

    // By aura: squad3(0), squad1(2), squad2(2 tie => id 1 then 2)
    expectOutput<int>(h.get_ith_collective_aura_squad(1), StatusType::SUCCESS, 3);
    expectOutput<int>(h.get_ith_collective_aura_squad(2), StatusType::SUCCESS, 1);
    expectOutput<int>(h.get_ith_collective_aura_squad(3), StatusType::SUCCESS, 2);

    auto duel = h.squad_duel(1, 2);
    EXPECT_EQ(duel.status(), StatusType::SUCCESS);

    expectOutput<int>(h.get_hunter_fights_number(101), StatusType::SUCCESS, 1);
    expectOutput<int>(h.get_hunter_fights_number(201), StatusType::SUCCESS, 1);

    // Make squad1 stronger to increase chance of strict inequality
    ASSERT_EQ(h.add_hunter(102, 1, nen("Conjurer"), 0, 0), StatusType::SUCCESS);

    StatusType st = h.force_join(1, 2);
    if (st == StatusType::SUCCESS) {
        EXPECT_EQ(h.get_squad_experience(2).status(), StatusType::FAILURE);
        EXPECT_EQ(h.get_partial_nen_ability(101).status(), StatusType::SUCCESS);
        EXPECT_EQ(h.get_partial_nen_ability(201).status(), StatusType::SUCCESS);
    } else {
        EXPECT_EQ(st, StatusType::FAILURE);
        EXPECT_EQ(h.get_squad_experience(1).status(), StatusType::SUCCESS);
        EXPECT_EQ(h.get_squad_experience(2).status(), StatusType::SUCCESS);
    }
}
