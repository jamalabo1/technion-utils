//
// Created by jamal on 15/01/2026.
//
#include <gtest/gtest.h>
#include "Huntech26a2.h"
#include "wet2util.h"

// ==================== CONSTRUCTOR/DESTRUCTOR TESTS ====================
class HuntechTest : public ::testing::Test {
protected:

    void SetUp() override {
        // Fresh instance for every test
    }

    void TearDown() override {
        // Huntech destructor handles cleanup
    }
};
// TEST_F(HuntechTest, ConstructorCreatesEmptySystem) {
//     Huntech* ht = new Huntech();
//     ASSERT_NE(ht, nullptr);
//     delete ht;
// }
//
// TEST_F(HuntechTest, DestructorHandlesEmptySystem) {
//     Huntech* ht = new Huntech();
//     ASSERT_NO_THROW(delete ht);
// }
//
// TEST_F(HuntechTest, DestructorHandlesSystemWithData) {
//     Huntech* ht = new Huntech();
//     ht->add_squad(1);
//     ht->add_hunter(100, 1, NenAbility("Enhancer"), 50, 0);
//     ASSERT_NO_THROW(delete ht);
// }

// ==================== ADD_SQUAD TESTS ====================

TEST(AddSquadTest, AddValidSquad) {
    Huntech ht;
    EXPECT_EQ(ht.add_squad(1), StatusType::SUCCESS);
}

TEST(AddSquadTest, AddMultipleSquads) {
    Huntech ht;
    EXPECT_EQ(ht.add_squad(1), StatusType::SUCCESS);
    EXPECT_EQ(ht.add_squad(2), StatusType::SUCCESS);
    EXPECT_EQ(ht.add_squad(100), StatusType::SUCCESS);
}

TEST(AddSquadTest, InvalidSquadIdZero) {
    Huntech ht;
    EXPECT_EQ(ht.add_squad(0), StatusType::INVALID_INPUT);
}

TEST(AddSquadTest, InvalidSquadIdNegative) {
    Huntech ht;
    EXPECT_EQ(ht.add_squad(-1), StatusType::INVALID_INPUT);
    EXPECT_EQ(ht.add_squad(-100), StatusType::INVALID_INPUT);
}

TEST(AddSquadTest, DuplicateSquadId) {
    Huntech ht;
    EXPECT_EQ(ht.add_squad(1), StatusType::SUCCESS);
    EXPECT_EQ(ht.add_squad(1), StatusType::FAILURE);
}

TEST(AddSquadTest, AddSquadAfterRemoval) {
    Huntech ht;
    ht.add_squad(1);
    ht.remove_squad(1);
    EXPECT_EQ(ht.add_squad(1), StatusType::SUCCESS);
}

// ==================== REMOVE_SQUAD TESTS ====================

TEST(RemoveSquadTest, RemoveExistingSquad) {
    Huntech ht;
    ht.add_squad(1);
    EXPECT_EQ(ht.remove_squad(1), StatusType::SUCCESS);
}

TEST(RemoveSquadTest, RemoveNonExistentSquad) {
    Huntech ht;
    EXPECT_EQ(ht.remove_squad(1), StatusType::FAILURE);
}

TEST(RemoveSquadTest, InvalidSquadIdZero) {
    Huntech ht;
    EXPECT_EQ(ht.remove_squad(0), StatusType::INVALID_INPUT);
}

TEST(RemoveSquadTest, InvalidSquadIdNegative) {
    Huntech ht;
    EXPECT_EQ(ht.remove_squad(-1), StatusType::INVALID_INPUT);
    EXPECT_EQ(ht.remove_squad(-50), StatusType::INVALID_INPUT);
}

TEST(RemoveSquadTest, RemoveSquadTwice) {
    Huntech ht;
    ht.add_squad(1);
    ht.remove_squad(1);
    EXPECT_EQ(ht.remove_squad(1), StatusType::FAILURE);
}

TEST(RemoveSquadTest, RemoveSquadWithHunters) {
    Huntech ht;
    ht.add_squad(1);
    ht.add_hunter(100, 1, NenAbility("Enhancer"), 50, 5);
    ht.add_hunter(101, 1, NenAbility("Emitter"), 30, 3);
    EXPECT_EQ(ht.remove_squad(1), StatusType::SUCCESS);
}

// ==================== ADD_HUNTER TESTS ====================

TEST(AddHunterTest, AddValidHunter) {
    Huntech ht;
    ht.add_squad(1);
    EXPECT_EQ(ht.add_hunter(100, 1, NenAbility("Enhancer"), 50, 5),
              StatusType::SUCCESS);
}

TEST(AddHunterTest, AddMultipleHuntersToSquad) {
    Huntech ht;
    ht.add_squad(1);
    EXPECT_EQ(ht.add_hunter(100, 1, NenAbility("Enhancer"), 50, 5),
              StatusType::SUCCESS);
    EXPECT_EQ(ht.add_hunter(101, 1, NenAbility("Emitter"), 30, 3),
              StatusType::SUCCESS);
    EXPECT_EQ(ht.add_hunter(102, 1, NenAbility("Transmuter"), 70, 10),
              StatusType::SUCCESS);
}

TEST(AddHunterTest, InvalidHunterIdZero) {
    Huntech ht;
    ht.add_squad(1);
    EXPECT_EQ(ht.add_hunter(0, 1, NenAbility("Enhancer"), 50, 5),
              StatusType::INVALID_INPUT);
}

TEST(AddHunterTest, InvalidHunterIdNegative) {
    Huntech ht;
    ht.add_squad(1);
    EXPECT_EQ(ht.add_hunter(-1, 1, NenAbility("Enhancer"), 50, 5),
              StatusType::INVALID_INPUT);
}

TEST(AddHunterTest, InvalidSquadIdZero) {
    Huntech ht;
    EXPECT_EQ(ht.add_hunter(100, 0, NenAbility("Enhancer"), 50, 5),
              StatusType::INVALID_INPUT);
}

TEST(AddHunterTest, InvalidSquadIdNegative) {
    Huntech ht;
    EXPECT_EQ(ht.add_hunter(100, -1, NenAbility("Enhancer"), 50, 5),
              StatusType::INVALID_INPUT);
}

TEST(AddHunterTest, InvalidNenAbility) {
    Huntech ht;
    ht.add_squad(1);
    EXPECT_EQ(ht.add_hunter(100, 1, NenAbility::invalid(), 50, 5),
              StatusType::INVALID_INPUT);
}

TEST(AddHunterTest, InvalidAuraNegative) {
    Huntech ht;
    ht.add_squad(1);
    EXPECT_EQ(ht.add_hunter(100, 1, NenAbility("Enhancer"), -1, 5),
              StatusType::INVALID_INPUT);
}

TEST(AddHunterTest, ValidAuraZero) {
    Huntech ht;
    ht.add_squad(1);
    EXPECT_EQ(ht.add_hunter(100, 1, NenAbility("Enhancer"), 0, 5),
              StatusType::SUCCESS);
}

TEST(AddHunterTest, InvalidFightsNegative) {
    Huntech ht;
    ht.add_squad(1);
    EXPECT_EQ(ht.add_hunter(100, 1, NenAbility("Enhancer"), 50, -1),
              StatusType::INVALID_INPUT);
}

TEST(AddHunterTest, ValidFightsZero) {
    Huntech ht;
    ht.add_squad(1);
    EXPECT_EQ(ht.add_hunter(100, 1, NenAbility("Enhancer"), 50, 0),
              StatusType::SUCCESS);
}

TEST(AddHunterTest, DuplicateHunterId) {
    Huntech ht;
    ht.add_squad(1);
    ht.add_hunter(100, 1, NenAbility("Enhancer"), 50, 5);
    EXPECT_EQ(ht.add_hunter(100, 1, NenAbility("Emitter"), 30, 3),
              StatusType::FAILURE);
}

TEST(AddHunterTest, NonExistentSquad) {
    Huntech ht;
    EXPECT_EQ(ht.add_hunter(100, 1, NenAbility("Enhancer"), 50, 5),
              StatusType::FAILURE);
}

TEST(AddHunterTest, HunterToRemovedSquad) {
    Huntech ht;
    ht.add_squad(1);
    ht.add_hunter(100, 1, NenAbility("Enhancer"), 50, 5);
    ht.remove_squad(1);
    ht.add_squad(1);
    EXPECT_EQ(ht.add_hunter(100, 1, NenAbility("Enhancer"), 50, 5),
              StatusType::FAILURE);
}

TEST(AddHunterTest, AllNenTypes) {
    Huntech ht;
    ht.add_squad(1);
    EXPECT_EQ(ht.add_hunter(100, 1, NenAbility("Enhancer"), 50, 5),
              StatusType::SUCCESS);
    EXPECT_EQ(ht.add_hunter(101, 1, NenAbility("Emitter"), 50, 5),
              StatusType::SUCCESS);
    EXPECT_EQ(ht.add_hunter(102, 1, NenAbility("Transmuter"), 50, 5),
              StatusType::SUCCESS);
    EXPECT_EQ(ht.add_hunter(103, 1, NenAbility("Conjurer"), 50, 5),
              StatusType::SUCCESS);
    EXPECT_EQ(ht.add_hunter(104, 1, NenAbility("Manipulator"), 50, 5),
              StatusType::SUCCESS);
    EXPECT_EQ(ht.add_hunter(105, 1, NenAbility("Specialist"), 50, 5),
              StatusType::SUCCESS);
}

// ==================== SQUAD_DUEL TESTS ====================

TEST(SquadDuelTest, Squad1WinsByAura) {
    Huntech ht;
    ht.add_squad(1);
    ht.add_squad(2);
    ht.add_hunter(100, 1, NenAbility("Enhancer"), 100, 0);
    ht.add_hunter(200, 2, NenAbility("Enhancer"), 50, 0);

    auto result = ht.squad_duel(1, 2);
    EXPECT_EQ(result.status(), StatusType::SUCCESS);
    EXPECT_EQ(result.ans(), 1);
}

TEST(SquadDuelTest, Squad2WinsByAura) {
    Huntech ht;
    ht.add_squad(1);
    ht.add_squad(2);
    ht.add_hunter(100, 1, NenAbility("Enhancer"), 50, 0);
    ht.add_hunter(200, 2, NenAbility("Enhancer"), 100, 0);

    auto result = ht.squad_duel(1, 2);
    EXPECT_EQ(result.status(), StatusType::SUCCESS);
    EXPECT_EQ(result.ans(), 3);
}

TEST(SquadDuelTest, Squad1WinsByNen) {
    Huntech ht;
    ht.add_squad(1);
    ht.add_squad(2);
    ht.add_hunter(100, 1, NenAbility("Enhancer"), 50, 0);
    ht.add_hunter(200, 2, NenAbility("Transmuter"), 50, 0);

    auto result = ht.squad_duel(1, 2);
    EXPECT_EQ(result.status(), StatusType::SUCCESS);
    EXPECT_EQ(result.ans(), 2);
}

TEST(SquadDuelTest, Squad2WinsByNen) {
    Huntech ht;
    ht.add_squad(1);
    ht.add_squad(2);
    ht.add_hunter(100, 1, NenAbility("Transmuter"), 50, 0);
    ht.add_hunter(200, 2, NenAbility("Enhancer"), 50, 0);

    auto result = ht.squad_duel(1, 2);
    EXPECT_EQ(result.status(), StatusType::SUCCESS);
    EXPECT_EQ(result.ans(), 4);
}

TEST(SquadDuelTest, DrawDuel) {
    Huntech ht;
    ht.add_squad(1);
    ht.add_squad(2);
    ht.add_hunter(100, 1, NenAbility("Enhancer"), 50, 0);
    ht.add_hunter(200, 2, NenAbility("Enhancer"), 50, 0);

    auto result = ht.squad_duel(1, 2);
    EXPECT_EQ(result.status(), StatusType::SUCCESS);
    EXPECT_EQ(result.ans(), 0);
}

TEST(SquadDuelTest, InvalidSquadId1Zero) {
    Huntech ht;
    ht.add_squad(2);
    auto result = ht.squad_duel(0, 2);
    EXPECT_EQ(result.status(), StatusType::INVALID_INPUT);
}

TEST(SquadDuelTest, InvalidSquadId2Zero) {
    Huntech ht;
    ht.add_squad(1);
    auto result = ht.squad_duel(1, 0);
    EXPECT_EQ(result.status(), StatusType::INVALID_INPUT);
}

TEST(SquadDuelTest, InvalidSquadIdsNegative) {
    Huntech ht;
    auto result = ht.squad_duel(-1, -2);
    EXPECT_EQ(result.status(), StatusType::INVALID_INPUT);
}

TEST(SquadDuelTest, SameSquadIds) {
    Huntech ht;
    ht.add_squad(1);
    auto result = ht.squad_duel(1, 1);
    EXPECT_EQ(result.status(), StatusType::INVALID_INPUT);
}

TEST(SquadDuelTest, NonExistentSquad1) {
    Huntech ht;
    ht.add_squad(2);
    auto result = ht.squad_duel(1, 2);
    EXPECT_EQ(result.status(), StatusType::FAILURE);
}

TEST(SquadDuelTest, NonExistentSquad2) {
    Huntech ht;
    ht.add_squad(1);
    auto result = ht.squad_duel(1, 2);
    EXPECT_EQ(result.status(), StatusType::FAILURE);
}

TEST(SquadDuelTest, EmptySquad1) {
    Huntech ht;
    ht.add_squad(1);
    ht.add_squad(2);
    ht.add_hunter(200, 2, NenAbility("Enhancer"), 50, 0);
    auto result = ht.squad_duel(1, 2);
    EXPECT_EQ(result.status(), StatusType::FAILURE);
}

TEST(SquadDuelTest, EmptySquad2) {
    Huntech ht;
    ht.add_squad(1);
    ht.add_squad(2);
    ht.add_hunter(100, 1, NenAbility("Enhancer"), 50, 0);
    auto result = ht.squad_duel(1, 2);
    EXPECT_EQ(result.status(), StatusType::FAILURE);
}

TEST(SquadDuelTest, BothSquadsEmpty) {
    Huntech ht;
    ht.add_squad(1);
    ht.add_squad(2);
    auto result = ht.squad_duel(1, 2);
    EXPECT_EQ(result.status(), StatusType::FAILURE);
}

TEST(SquadDuelTest, ExperienceAffectsOutcome) {
    Huntech ht;
    ht.add_squad(1);
    ht.add_squad(2);
    ht.add_hunter(100, 1, NenAbility("Enhancer"), 50, 0);
    ht.add_hunter(200, 2, NenAbility("Enhancer"), 50, 0);

    // First duel is a draw
    auto result1 = ht.squad_duel(1, 2);
    EXPECT_EQ(result1.ans(), 0);

    // After draw, both gain exp=1, still equal
    auto result2 = ht.squad_duel(1, 2);
    EXPECT_EQ(result2.ans(), 0);

    // Get experience values
    auto exp1 = ht.get_squad_experience(1);
    auto exp2 = ht.get_squad_experience(2);
    EXPECT_EQ(exp1.ans(), 2);
    EXPECT_EQ(exp2.ans(), 2);
}

TEST(SquadDuelTest, FightsCountIncreases) {
    Huntech ht;
    ht.add_squad(1);
    ht.add_squad(2);
    ht.add_hunter(100, 1, NenAbility("Enhancer"), 50, 5);
    ht.add_hunter(200, 2, NenAbility("Enhancer"), 50, 3);

    ht.squad_duel(1, 2);

    auto fights1 = ht.get_hunter_fights_number(100);
    auto fights2 = ht.get_hunter_fights_number(200);
    EXPECT_EQ(fights1.ans(), 6);
    EXPECT_EQ(fights2.ans(), 4);
}

// ==================== GET_HUNTER_FIGHTS_NUMBER TESTS ====================

TEST(GetHunterFightsTest, ValidHunter) {
    Huntech ht;
    ht.add_squad(1);
    ht.add_hunter(100, 1, NenAbility("Enhancer"), 50, 5);

    auto result = ht.get_hunter_fights_number(100);
    EXPECT_EQ(result.status(), StatusType::SUCCESS);
    EXPECT_EQ(result.ans(), 5);
}

TEST(GetHunterFightsTest, InvalidHunterIdZero) {
    Huntech ht;
    auto result = ht.get_hunter_fights_number(0);
    EXPECT_EQ(result.status(), StatusType::INVALID_INPUT);
}

TEST(GetHunterFightsTest, InvalidHunterIdNegative) {
    Huntech ht;
    auto result = ht.get_hunter_fights_number(-1);
    EXPECT_EQ(result.status(), StatusType::INVALID_INPUT);
}

TEST(GetHunterFightsTest, NonExistentHunter) {
    Huntech ht;
    auto result = ht.get_hunter_fights_number(100);
    EXPECT_EQ(result.status(), StatusType::FAILURE);
}

TEST(GetHunterFightsTest, DeadHunter) {
    Huntech ht;
    ht.add_squad(1);
    ht.add_hunter(100, 1, NenAbility("Enhancer"), 50, 5);
    ht.remove_squad(1);

    auto result = ht.get_hunter_fights_number(100);
    EXPECT_EQ(result.status(), StatusType::SUCCESS);
    EXPECT_EQ(result.ans(), 5);
}

TEST(GetHunterFightsTest, HunterWithZeroFights) {
    Huntech ht;
    ht.add_squad(1);
    ht.add_hunter(100, 1, NenAbility("Enhancer"), 50, 0);

    auto result = ht.get_hunter_fights_number(100);
    EXPECT_EQ(result.status(), StatusType::SUCCESS);
    EXPECT_EQ(result.ans(), 0);
}

// ==================== GET_SQUAD_EXPERIENCE TESTS ====================

TEST(GetSquadExperienceTest, ValidSquadNoExp) {
    Huntech ht;
    ht.add_squad(1);

    auto result = ht.get_squad_experience(1);
    EXPECT_EQ(result.status(), StatusType::SUCCESS);
    EXPECT_EQ(result.ans(), 0);
}

TEST(GetSquadExperienceTest, InvalidSquadIdZero) {
    Huntech ht;
    auto result = ht.get_squad_experience(0);
    EXPECT_EQ(result.status(), StatusType::INVALID_INPUT);
}

TEST(GetSquadExperienceTest, InvalidSquadIdNegative) {
    Huntech ht;
    auto result = ht.get_squad_experience(-1);
    EXPECT_EQ(result.status(), StatusType::INVALID_INPUT);
}

TEST(GetSquadExperienceTest, NonExistentSquad) {
    Huntech ht;
    auto result = ht.get_squad_experience(1);
    EXPECT_EQ(result.status(), StatusType::FAILURE);
}

TEST(GetSquadExperienceTest, SquadAfterDuel) {
    Huntech ht;
    ht.add_squad(1);
    ht.add_squad(2);
    ht.add_hunter(100, 1, NenAbility("Enhancer"), 100, 0);
    ht.add_hunter(200, 2, NenAbility("Enhancer"), 50, 0);

    ht.squad_duel(1, 2);

    auto exp1 = ht.get_squad_experience(1);
    auto exp2 = ht.get_squad_experience(2);
    EXPECT_EQ(exp1.ans(), 3); // Winner gets +3
    EXPECT_EQ(exp2.ans(), 0); // Loser gets nothing
}

// ==================== GET_ITH_COLLECTIVE_AURA_SQUAD TESTS ====================

TEST(GetIthAuraSquadTest, SingleSquad) {
    Huntech ht;
    ht.add_squad(1);
    ht.add_hunter(100, 1, NenAbility("Enhancer"), 50, 0);

    auto result = ht.get_ith_collective_aura_squad(1);
    EXPECT_EQ(result.status(), StatusType::SUCCESS);
    EXPECT_EQ(result.ans(), 1);
}

TEST(GetIthAuraSquadTest, MultipleSquadsSorted) {
    Huntech ht;
    ht.add_squad(1);
    ht.add_squad(2);
    ht.add_squad(3);
    ht.add_hunter(100, 1, NenAbility("Enhancer"), 50, 0);
    ht.add_hunter(200, 2, NenAbility("Enhancer"), 100, 0);
    ht.add_hunter(300, 3, NenAbility("Enhancer"), 75, 0);

    auto result1 = ht.get_ith_collective_aura_squad(1);
    auto result2 = ht.get_ith_collective_aura_squad(2);
    auto result3 = ht.get_ith_collective_aura_squad(3);

    EXPECT_EQ(result1.ans(), 1); // 50 aura
    EXPECT_EQ(result2.ans(), 3); // 75 aura
    EXPECT_EQ(result3.ans(), 2); // 100 aura
}

TEST(GetIthAuraSquadTest, TieBreakBySquadId) {
    Huntech ht;
    ht.add_squad(5);
    ht.add_squad(2);
    ht.add_squad(8);
    ht.add_hunter(100, 5, NenAbility("Enhancer"), 50, 0);
    ht.add_hunter(200, 2, NenAbility("Enhancer"), 50, 0);
    ht.add_hunter(300, 8, NenAbility("Enhancer"), 50, 0);

    auto result1 = ht.get_ith_collective_aura_squad(1);
    auto result2 = ht.get_ith_collective_aura_squad(2);
    auto result3 = ht.get_ith_collective_aura_squad(3);

    EXPECT_EQ(result1.ans(), 2); // Same aura, lower ID
    EXPECT_EQ(result2.ans(), 5);
    EXPECT_EQ(result3.ans(), 8);
}

TEST(GetIthAuraSquadTest, InvalidIndexZero) {
    Huntech ht;
    ht.add_squad(1);
    auto result = ht.get_ith_collective_aura_squad(0);
    EXPECT_EQ(result.status(), StatusType::FAILURE);
}

TEST(GetIthAuraSquadTest, InvalidIndexNegative) {
    Huntech ht;
    ht.add_squad(1);
    auto result = ht.get_ith_collective_aura_squad(-1);
    EXPECT_EQ(result.status(), StatusType::FAILURE);
}

TEST(GetIthAuraSquadTest, IndexTooLarge) {
    Huntech ht;
    ht.add_squad(1);
    auto result = ht.get_ith_collective_aura_squad(2);
    EXPECT_EQ(result.status(), StatusType::FAILURE);
}

TEST(GetIthAuraSquadTest, NoSquads) {
    Huntech ht;
    auto result = ht.get_ith_collective_aura_squad(1);
    EXPECT_EQ(result.status(), StatusType::FAILURE);
}

TEST(GetIthAuraSquadTest, EmptySquadsIncluded) {
    Huntech ht;
    ht.add_squad(1);
    ht.add_squad(2);
    ht.add_hunter(200, 2, NenAbility("Enhancer"), 50, 0);

    auto result1 = ht.get_ith_collective_aura_squad(1);
    auto result2 = ht.get_ith_collective_aura_squad(2);

    EXPECT_EQ(result1.ans(), 1); // 0 aura
    EXPECT_EQ(result2.ans(), 2); // 50 aura
}

// ==================== GET_PARTIAL_NEN_ABILITY TESTS ====================

TEST(GetPartialNenTest, SingleHunter) {
    Huntech ht;
    ht.add_squad(1);
    ht.add_hunter(100, 1, NenAbility("Enhancer"), 50, 0);

    auto result = ht.get_partial_nen_ability(100);
    EXPECT_EQ(result.status(), StatusType::SUCCESS);
    EXPECT_TRUE(result.ans() == NenAbility("Enhancer"));
}

TEST(GetPartialNenTest, FirstHunterInSquad) {
    Huntech ht;
    ht.add_squad(1);
    ht.add_hunter(100, 1, NenAbility("Enhancer"), 50, 0);
    ht.add_hunter(101, 1, NenAbility("Emitter"), 30, 0);

    auto result = ht.get_partial_nen_ability(100);
    EXPECT_EQ(result.status(), StatusType::SUCCESS);
    EXPECT_TRUE(result.ans() == NenAbility("Enhancer"));
}

TEST(GetPartialNenTest, MiddleHunterInSquad) {
    Huntech ht;
    ht.add_squad(1);
    ht.add_hunter(100, 1, NenAbility("Enhancer"), 50, 0);
    ht.add_hunter(101, 1, NenAbility("Emitter"), 30, 0);
    ht.add_hunter(102, 1, NenAbility("Transmuter"), 40, 0);

    auto result = ht.get_partial_nen_ability(101);
    EXPECT_EQ(result.status(), StatusType::SUCCESS);
    NenAbility expected = NenAbility("Enhancer") + NenAbility("Emitter");
    EXPECT_TRUE(result.ans() == expected);
}

TEST(GetPartialNenTest, LastHunterInSquad) {
    Huntech ht;
    ht.add_squad(1);
    ht.add_hunter(100, 1, NenAbility("Enhancer"), 50, 0);
    ht.add_hunter(101, 1, NenAbility("Emitter"), 30, 0);
    ht.add_hunter(102, 1, NenAbility("Transmuter"), 40, 0);

    auto result = ht.get_partial_nen_ability(102);
    EXPECT_EQ(result.status(), StatusType::SUCCESS);
    NenAbility expected = NenAbility("Enhancer") + NenAbility("Emitter") +
                         NenAbility("Transmuter");
    EXPECT_TRUE(result.ans() == expected);
}

TEST(GetPartialNenTest, InvalidHunterIdZero) {
    Huntech ht;
    auto result = ht.get_partial_nen_ability(0);
    EXPECT_EQ(result.status(), StatusType::INVALID_INPUT);
}

TEST(GetPartialNenTest, InvalidHunterIdNegative) {
    Huntech ht;
    auto result = ht.get_partial_nen_ability(-1);
    EXPECT_EQ(result.status(), StatusType::INVALID_INPUT);
}

TEST(GetPartialNenTest, NonExistentHunter) {
    Huntech ht;
    auto result = ht.get_partial_nen_ability(100);
    EXPECT_EQ(result.status(), StatusType::FAILURE);
}

TEST(GetPartialNenTest, DeadHunter) {
    Huntech ht;
    ht.add_squad(1);
    ht.add_hunter(100, 1, NenAbility("Enhancer"), 50, 0);
    ht.remove_squad(1);

    auto result = ht.get_partial_nen_ability(100);
    EXPECT_EQ(result.status(), StatusType::FAILURE);
}

// ==================== FORCE_JOIN TESTS ====================

TEST(ForceJoinTest, ValidForceJoin) {
    Huntech ht;
    ht.add_squad(1);
    ht.add_squad(2);
    ht.add_hunter(100, 1, NenAbility("Enhancer"), 100, 0);
    ht.add_hunter(200, 2, NenAbility("Enhancer"), 50, 0);

    EXPECT_EQ(ht.force_join(1, 2), StatusType::SUCCESS);
}

TEST(ForceJoinTest, InvalidForcingSquadIdZero) {
    Huntech ht;
    ht.add_squad(2);
    EXPECT_EQ(ht.force_join(0, 2), StatusType::INVALID_INPUT);
}

TEST(ForceJoinTest, InvalidForcedSquadIdZero) {
    Huntech ht;
    ht.add_squad(1);
    EXPECT_EQ(ht.force_join(1, 0), StatusType::INVALID_INPUT);
}

TEST(ForceJoinTest, InvalidSquadIdsNegative) {
    Huntech ht;
    EXPECT_EQ(ht.force_join(-1, -2), StatusType::INVALID_INPUT);
}

TEST(ForceJoinTest, SameSquadIds) {
    Huntech ht;
    ht.add_squad(1);
    EXPECT_EQ(ht.force_join(1, 1), StatusType::INVALID_INPUT);
}

TEST(ForceJoinTest, NonExistentForcingSquad) {
    Huntech ht;
    ht.add_squad(2);
    EXPECT_EQ(ht.force_join(1, 2), StatusType::FAILURE);
}

TEST(ForceJoinTest, NonExistentForcedSquad) {
    Huntech ht;
    ht.add_squad(1);
    EXPECT_EQ(ht.force_join(1, 2), StatusType::FAILURE);
}

TEST(ForceJoinTest, ForcingSquadEmpty) {
    Huntech ht;
    ht.add_squad(1);
    ht.add_squad(2);
    ht.add_hunter(200, 2, NenAbility("Enhancer"), 50, 0);

    EXPECT_EQ(ht.force_join(1, 2), StatusType::FAILURE);
}

TEST(ForceJoinTest, InsufficientStrength) {
    Huntech ht;
    ht.add_squad(1);
    ht.add_squad(2);
    ht.add_hunter(100, 1, NenAbility("Enhancer"), 50, 0);
    ht.add_hunter(200, 2, NenAbility("Enhancer"), 100, 0);

    EXPECT_EQ(ht.force_join(1, 2), StatusType::FAILURE);
}

TEST(ForceJoinTest, EqualStrength) {
    Huntech ht;
    ht.add_squad(1);
    ht.add_squad(2);
    ht.add_hunter(100, 1, NenAbility("Enhancer"), 50, 0);
    ht.add_hunter(200, 2, NenAbility("Enhancer"), 50, 0);

    EXPECT_EQ(ht.force_join(1, 2), StatusType::FAILURE);
}

TEST(ForceJoinTest, EmptySquadCanBeForced) {
    Huntech ht;
    ht.add_squad(1);
    ht.add_squad(2);
    ht.add_hunter(100, 1, NenAbility("Enhancer"), 50, 0);

    EXPECT_EQ(ht.force_join(1, 2), StatusType::SUCCESS);
}

TEST(ForceJoinTest, HunterOrderPreserved) {
    Huntech ht;
    ht.add_squad(1);
    ht.add_squad(2);
    ht.add_hunter(100, 1, NenAbility("Enhancer"), 100, 0);
    ht.add_hunter(101, 1, NenAbility("Emitter"), 50, 0);
    ht.add_hunter(200, 2, NenAbility("Transmuter"), 30, 0);
    ht.add_hunter(201, 2, NenAbility("Conjurer"), 20, 0);

    ht.force_join(1, 2);

    // Check partial nen for last hunter shows all in correct order
    auto result = ht.get_partial_nen_ability(201);
    NenAbility expected = NenAbility("Enhancer") + NenAbility("Emitter") +
                         NenAbility("Transmuter") + NenAbility("Conjurer");
    EXPECT_TRUE(result.ans() == expected);
}

TEST(ForceJoinTest, ExperienceSummed) {
    Huntech ht;
    ht.add_squad(1);
    ht.add_squad(2);
    ht.add_squad(3);

    ht.add_hunter(100, 1, NenAbility("Enhancer"), 100, 0);
    ht.add_hunter(200, 2, NenAbility("Enhancer"), 50, 0);
    ht.add_hunter(300, 3, NenAbility("Enhancer"), 30, 0);

    // Give squad 1 some experience
    ht.squad_duel(1, 3);

    // Give squad 2 some experience
    ht.squad_duel(2, 3);

    auto exp1_before = ht.get_squad_experience(1);
    auto exp2_before = ht.get_squad_experience(2);

    ht.force_join(1, 2);

    auto exp1_after = ht.get_squad_experience(1);
    EXPECT_EQ(exp1_after.ans(), exp1_before.ans() + exp2_before.ans());
}

TEST(ForceJoinTest, ForcedSquadNoLongerExists) {
    Huntech ht;
    ht.add_squad(1);
    ht.add_squad(2);
    ht.add_hunter(100, 1, NenAbility("Enhancer"), 100, 0);
    ht.add_hunter(200, 2, NenAbility("Enhancer"), 50, 0);

    ht.force_join(1, 2);

    auto result = ht.get_squad_experience(2);
    EXPECT_EQ(result.status(), StatusType::FAILURE);
}

TEST(ForceJoinTest, FightsPreservedAfterJoin) {
    Huntech ht;
    ht.add_squad(1);
    ht.add_squad(2);
    ht.add_hunter(100, 1, NenAbility("Enhancer"), 100, 5);
    ht.add_hunter(200, 2, NenAbility("Enhancer"), 50, 3);

    ht.force_join(1, 2);

    auto fights1 = ht.get_hunter_fights_number(100);
    auto fights2 = ht.get_hunter_fights_number(200);

    EXPECT_EQ(fights1.ans(), 5);
    EXPECT_EQ(fights2.ans(), 3);
}

TEST(ForceJoinTest, StrengthCalculationWithExperience) {
    Huntech ht;
    ht.add_squad(1);
    ht.add_squad(2);
    ht.add_squad(3);

    // Squad 1: aura=50, exp=0, effective=1
    ht.add_hunter(100, 1, NenAbility("Enhancer"), 50, 0);

    // Squad 2: aura=50, exp=0, effective=1
    ht.add_hunter(200, 2, NenAbility("Enhancer"), 50, 0);

    // Squad 3: weak squad for testing
    ht.add_hunter(300, 3, NenAbility("Enhancer"), 10, 0);

    // Initially, squads 1 and 2 are equal, force_join should fail
    EXPECT_EQ(ht.force_join(1, 2), StatusType::FAILURE);

    // Give squad 1 experience
    ht.squad_duel(1, 3);

    // Now squad 1 should be able to force squad 2
    EXPECT_EQ(ht.force_join(1, 2), StatusType::SUCCESS);
}

// ==================== COMPLEX INTEGRATION TESTS ====================

TEST(IntegrationTest, MultipleOperationsSequence) {
    Huntech ht;

    ht.add_squad(1);
    ht.add_squad(2);
    ht.add_squad(3);

    ht.add_hunter(100, 1, NenAbility("Enhancer"), 100, 0);
    ht.add_hunter(101, 1, NenAbility("Emitter"), 50, 0);
    ht.add_hunter(200, 2, NenAbility("Transmuter"), 80, 0);
    ht.add_hunter(300, 3, NenAbility("Conjurer"), 60, 0);

    auto duel1 = ht.squad_duel(1, 2);
    EXPECT_EQ(duel1.status(), StatusType::SUCCESS);

    auto exp1 = ht.get_squad_experience(1);
    EXPECT_GT(exp1.ans(), 0);

    auto fights = ht.get_hunter_fights_number(100);
    EXPECT_EQ(fights.ans(), 1);

    auto ith = ht.get_ith_collective_aura_squad(3);
    EXPECT_EQ(ith.status(), StatusType::SUCCESS);
}

TEST(IntegrationTest, ForceJoinThenDuel) {
    Huntech ht;

    ht.add_squad(1);
    ht.add_squad(2);
    ht.add_squad(3);

    ht.add_hunter(100, 1, NenAbility("Enhancer"), 100, 0);
    ht.add_hunter(200, 2, NenAbility("Emitter"), 50, 0);
    ht.add_hunter(300, 3, NenAbility("Transmuter"), 80, 0);

    ht.force_join(1, 2);

    auto duel = ht.squad_duel(1, 3);
    EXPECT_EQ(duel.status(), StatusType::SUCCESS);
}

TEST(IntegrationTest, RemoveSquadThenCheckHunters) {
    Huntech ht;

    ht.add_squad(1);
    ht.add_hunter(100, 1, NenAbility("Enhancer"), 50, 5);
    ht.add_hunter(101, 1, NenAbility("Emitter"), 30, 3);

    ht.remove_squad(1);

    // Hunters should still have fight records
    auto fights1 = ht.get_hunter_fights_number(100);
    auto fights2 = ht.get_hunter_fights_number(101);

    EXPECT_EQ(fights1.status(), StatusType::SUCCESS);
    EXPECT_EQ(fights1.ans(), 5);
    EXPECT_EQ(fights2.status(), StatusType::SUCCESS);
    EXPECT_EQ(fights2.ans(), 3);

    // But partial nen should fail (squad is gone)
    auto partial = ht.get_partial_nen_ability(100);
    EXPECT_EQ(partial.status(), StatusType::FAILURE);
}

TEST(IntegrationTest, LargeScaleOperations) {
    Huntech ht;

    // Add 10 squads
    for (int i = 1; i <= 10; i++) {
        EXPECT_EQ(ht.add_squad(i), StatusType::SUCCESS);
    }

    // Add 5 hunters to each squad
    for (int squad = 1; squad <= 10; squad++) {
        for (int hunter = 0; hunter < 5; hunter++) {
            int hunterId = squad * 100 + hunter;
            ht.add_hunter(hunterId, squad, NenAbility("Enhancer"),
                         squad * 10, hunter);
        }
    }

    // Perform multiple duels
    for (int i = 1; i < 10; i++) {
        auto duel = ht.squad_duel(i, i + 1);
        EXPECT_EQ(duel.status(), StatusType::SUCCESS);
    }

    // Check rankings
    auto ith1 = ht.get_ith_collective_aura_squad(1);
    EXPECT_EQ(ith1.status(), StatusType::SUCCESS);

    auto ith10 = ht.get_ith_collective_aura_squad(10);
    EXPECT_EQ(ith10.status(), StatusType::SUCCESS);
}

TEST(IntegrationTest, AllNenTypesCombined) {
    Huntech ht;

    ht.add_squad(1);
    ht.add_hunter(100, 1, NenAbility("Enhancer"), 50, 0);
    ht.add_hunter(101, 1, NenAbility("Emitter"), 50, 0);
    ht.add_hunter(102, 1, NenAbility("Transmuter"), 50, 0);
    ht.add_hunter(103, 1, NenAbility("Conjurer"), 50, 0);
    ht.add_hunter(104, 1, NenAbility("Manipulator"), 50, 0);
    ht.add_hunter(105, 1, NenAbility("Specialist"), 50, 0);

    auto partial = ht.get_partial_nen_ability(105);
    EXPECT_EQ(partial.status(), StatusType::SUCCESS);
}

TEST(EdgeCaseTest, VeryLargeAuraValues) {
    Huntech ht;
    ht.add_squad(1);
    ht.add_hunter(100, 1, NenAbility("Enhancer"), 1000000, 0);

    auto ith = ht.get_ith_collective_aura_squad(1);
    EXPECT_EQ(ith.status(), StatusType::SUCCESS);
    EXPECT_EQ(ith.ans(), 1);
}

TEST(EdgeCaseTest, VeryLargeFightCounts) {
    Huntech ht;
    ht.add_squad(1);
    ht.add_hunter(100, 1, NenAbility("Enhancer"), 50, 1000000);

    auto fights = ht.get_hunter_fights_number(100);
    EXPECT_EQ(fights.status(), StatusType::SUCCESS);
    EXPECT_EQ(fights.ans(), 1000000);
}

TEST(EdgeCaseTest, ChainedForceJoins) {
    Huntech ht;

    ht.add_squad(1);
    ht.add_squad(2);
    ht.add_squad(3);
    ht.add_squad(4);

    ht.add_hunter(100, 1, NenAbility("Enhancer"), 1000, 0);
    ht.add_hunter(200, 2, NenAbility("Enhancer"), 100, 0);
    ht.add_hunter(300, 3, NenAbility("Enhancer"), 10, 0);
    ht.add_hunter(400, 4, NenAbility("Enhancer"), 1, 0);

    EXPECT_EQ(ht.force_join(1, 2), StatusType::SUCCESS);
    EXPECT_EQ(ht.force_join(1, 3), StatusType::SUCCESS);
    EXPECT_EQ(ht.force_join(1, 4), StatusType::SUCCESS);

    // Squad 1 should now have all hunters
    auto partial = ht.get_partial_nen_ability(400);
    EXPECT_EQ(partial.status(), StatusType::SUCCESS);
}
