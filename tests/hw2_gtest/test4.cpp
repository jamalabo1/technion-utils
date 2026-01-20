//
// Created by jamal on 15/01/2026.
//
#include <gtest/gtest.h>
#include "Huntech26a2.h"
#include <vector>
#include <numeric>

// Helper to create NenAbility easily
NenAbility createNen(std::string type) {
    return NenAbility(type);
}

class HuntechTest : public ::testing::Test {
protected:
    Huntech huntech;

    void SetUp() override {
        // Fresh instance for every test
    }

    void TearDown() override {
        // Huntech destructor handles cleanup
    }
};

// ==========================================
// Basic Squad Management
// ==========================================

TEST_F(HuntechTest, AddSquad_Basic) {
    EXPECT_EQ(huntech.add_squad(1), StatusType::SUCCESS);
    EXPECT_EQ(huntech.add_squad(2), StatusType::SUCCESS);
}

TEST_F(HuntechTest, AddSquad_Invalid) {
    EXPECT_EQ(huntech.add_squad(-5), StatusType::INVALID_INPUT);
    EXPECT_EQ(huntech.add_squad(0), StatusType::INVALID_INPUT);
}

TEST_F(HuntechTest, AddSquad_Duplicate) {
    EXPECT_EQ(huntech.add_squad(1), StatusType::SUCCESS);
    EXPECT_EQ(huntech.add_squad(1), StatusType::FAILURE);
}

TEST_F(HuntechTest, RemoveSquad_Basic) {
    huntech.add_squad(1);
    EXPECT_EQ(huntech.remove_squad(1), StatusType::SUCCESS);
    // Removing again should fail
    EXPECT_EQ(huntech.remove_squad(1), StatusType::FAILURE);
}

TEST_F(HuntechTest, RemoveSquad_Invalid) {
    EXPECT_EQ(huntech.remove_squad(-1), StatusType::INVALID_INPUT);
    EXPECT_EQ(huntech.remove_squad(999), StatusType::FAILURE); // Does not exist
}

// ==========================================
// Hunter Management
// ==========================================

TEST_F(HuntechTest, AddHunter_Basic) {
    huntech.add_squad(1);
    // ID: 10, Squad: 1, Type: Enhancer, Aura: 50, Fights: 0
    EXPECT_EQ(huntech.add_hunter(10, 1, createNen("Enhancer"), 50, 0), StatusType::SUCCESS);

    // Check fight count matches initial
    output_t<int> fights = huntech.get_hunter_fights_number(10);
    EXPECT_EQ(fights.status(), StatusType::SUCCESS);
    EXPECT_EQ(fights.ans(), 0);
}

TEST_F(HuntechTest, AddHunter_EdgeCases) {
    huntech.add_squad(1);

    // Invalid IDs
    EXPECT_EQ(huntech.add_hunter(-1, 1, createNen("Enhancer"), 50, 0), StatusType::INVALID_INPUT);
    EXPECT_EQ(huntech.add_hunter(10, -1, createNen("Enhancer"), 50, 0), StatusType::INVALID_INPUT);

    // Invalid Aura/Fights
    EXPECT_EQ(huntech.add_hunter(10, 1, createNen("Enhancer"), -10, 0), StatusType::INVALID_INPUT);
    EXPECT_EQ(huntech.add_hunter(11, 1, createNen("Enhancer"), 50, -5), StatusType::INVALID_INPUT);

    // Invalid Nen
    NenAbility badNen = NenAbility::invalid();
    EXPECT_EQ(huntech.add_hunter(12, 1, badNen, 50, 0), StatusType::INVALID_INPUT);

    // Non-existent squad
    EXPECT_EQ(huntech.add_hunter(20, 99, createNen("Enhancer"), 50, 0), StatusType::FAILURE);

    // Duplicate Hunter ID
    huntech.add_hunter(10, 1, createNen("Enhancer"), 50, 0);
    EXPECT_EQ(huntech.add_hunter(10, 1, createNen("Emitter"), 20, 0), StatusType::FAILURE);
}

// ==========================================
// Squad Experience & Aura Queries
// ==========================================

TEST_F(HuntechTest, SquadExperience) {
    huntech.add_squad(1);
    output_t<int> exp = huntech.get_squad_experience(1);
    EXPECT_EQ(exp.status(), StatusType::SUCCESS);
    EXPECT_EQ(exp.ans(), 0); // Starts at 0

    // Check invalid
    output_t<int> expBad = huntech.get_squad_experience(99);
    EXPECT_EQ(expBad.status(), StatusType::FAILURE);
}



TEST_F(HuntechTest, GetIthCollectiveAura_TieBreaking) {
    // Tie breaker is Squad ID ascending
    huntech.add_squad(10);
    huntech.add_hunter(1, 10, createNen("Enhancer"), 100, 0); // 100

    huntech.add_squad(5);
    huntech.add_hunter(2, 5, createNen("Enhancer"), 100, 0); // 100

    // Squad 5 should be first (smaller ID)
    output_t<int> res1 = huntech.get_ith_collective_aura_squad(1);
    EXPECT_EQ(res1.ans(), 5);

    output_t<int> res2 = huntech.get_ith_collective_aura_squad(2);
    EXPECT_EQ(res2.ans(), 10);
}

// ==========================================
// Duel Logic
// ==========================================

TEST_F(HuntechTest, SquadDuel_WinByAura) {
    huntech.add_squad(1);
    huntech.add_squad(2);

    // Squad 1: Aura 100, Exp 0 -> Eff 100
    huntech.add_hunter(10, 1, createNen("Enhancer"), 100, 0);

    // Squad 2: Aura 10, Exp 0 -> Eff 10
    huntech.add_hunter(20, 2, createNen("Enhancer"), 10, 0);

    // 1 vs 2 -> 1 wins (returns 1)
    output_t<int> res = huntech.squad_duel(1, 2);
    EXPECT_EQ(res.status(), StatusType::SUCCESS);
    EXPECT_EQ(res.ans(), 1);

    // Check Exp update
    // Winner +3, Loser +0
    EXPECT_EQ(huntech.get_squad_experience(1).ans(), 3);
    EXPECT_EQ(huntech.get_squad_experience(2).ans(), 0);

    // Check Fights update
    EXPECT_EQ(huntech.get_hunter_fights_number(10).ans(), 1);
    EXPECT_EQ(huntech.get_hunter_fights_number(20).ans(), 1);
}

TEST_F(HuntechTest, SquadDuel_WinByNen) {
    huntech.add_squad(1); // Enhancer
    huntech.add_squad(2); // Transmuter

    // Same Aura/Exp -> Check Nen
    // Enhancer > Transmuter in matrix (0 vs +1? No check matrix)
    // Enhancer row: E:0, Em:1, Tr:1, Co:-1, Ma:-1, Sp:-1
    // Enhancer vs Transmuter = +1 (Win for Enhancer)

    huntech.add_hunter(10, 1, createNen("Enhancer"), 100, 0);
    huntech.add_hunter(20, 2, createNen("Transmuter"), 100, 0);

    output_t<int> res = huntech.squad_duel(1, 2);
    EXPECT_EQ(res.status(), StatusType::SUCCESS);
    EXPECT_EQ(res.ans(), 2); // 2 means Squad 1 won by Nen

    EXPECT_EQ(huntech.get_squad_experience(1).ans(), 3);
}

TEST_F(HuntechTest, SquadDuel_Draw) {
    huntech.add_squad(1);
    huntech.add_squad(2);

    // Identical setup
    huntech.add_hunter(10, 1, createNen("Enhancer"), 100, 0);
    huntech.add_hunter(20, 2, createNen("Enhancer"), 100, 0);

    output_t<int> res = huntech.squad_duel(1, 2);
    EXPECT_EQ(res.status(), StatusType::SUCCESS);
    EXPECT_EQ(res.ans(), 0); // Draw

    // Both +1 Exp
    EXPECT_EQ(huntech.get_squad_experience(1).ans(), 1);
    EXPECT_EQ(huntech.get_squad_experience(2).ans(), 1);
}

TEST_F(HuntechTest, SquadDuel_EmptySquad) {
    huntech.add_squad(1);
    huntech.add_squad(2);

    // Duel with empty squad returns FAILURE
    EXPECT_EQ(huntech.squad_duel(1, 2).status(), StatusType::FAILURE);
}

// ==========================================
// Force Join Logic
// ==========================================

TEST_F(HuntechTest, ForceJoin_ConditionFails) {
    huntech.add_squad(1);
    huntech.add_squad(2);

    // Equal strength -> Strict inequality fails
    huntech.add_hunter(10, 1, createNen("Enhancer"), 100, 0);
    huntech.add_hunter(20, 2, createNen("Enhancer"), 100, 0);

    EXPECT_EQ(huntech.force_join(1, 2), StatusType::FAILURE);
}

TEST_F(HuntechTest, ForceJoin_EmptySquad) {
    huntech.add_squad(1);
    huntech.add_squad(2);
    huntech.add_hunter(10, 1, createNen("Enhancer"), 100, 0);

    // Squad 2 is empty.
    // PDF: "Treat empty squad as if it can never force, and can always be forced by non-empty."

    // 1 forces 2 (Success)
    EXPECT_EQ(huntech.force_join(1, 2), StatusType::SUCCESS);

    // 2 forces 1 (Failure - empty cannot force)
    huntech.add_squad(3); // Empty
    EXPECT_EQ(huntech.force_join(3, 1), StatusType::FAILURE);
}

// ==========================================
// Partial Nen Ability (Prefix Sums)
// ==========================================

TEST_F(HuntechTest, PartialNen_SingleSquad) {
    huntech.add_squad(1);

    // Add H1: Enhancer (1,0,0,0,0,0)
    huntech.add_hunter(1, 1, createNen("Enhancer"), 10, 0);

    // Add H2: Emitter (0,1,0,0,0,0)
    huntech.add_hunter(2, 1, createNen("Emitter"), 10, 0);

    // Get Partial for H2 -> Should be H1 + H2
    output_t<NenAbility> res = huntech.get_partial_nen_ability(2);
    EXPECT_EQ(res.status(), StatusType::SUCCESS);

    // Check components manually roughly (since we can't inspect private, we assume Sum works)
    // Enhancer:1, Emitter:1
    // Effective Ability (L2 Norm): 1^2 + 1^2 = 2
    EXPECT_EQ(res.ans().getEffectiveNenAbility(), 2);

    // Get Partial for H1 -> Just H1
    output_t<NenAbility> res1 = huntech.get_partial_nen_ability(1);
    EXPECT_EQ(res1.ans().getEffectiveNenAbility(), 1);
}

TEST_F(HuntechTest, PartialNen_AfterMerge) {
    // Squad 1: H1 (Enhancer)
    huntech.add_squad(1);
    huntech.add_hunter(1, 1, createNen("Enhancer"), 100, 0);

    // Squad 2: H2 (Emitter)
    huntech.add_squad(2);
    huntech.add_hunter(2, 2, createNen("Emitter"), 10, 0);

    // 1 forces 2. Order becomes H1, H2.
    huntech.force_join(1, 2);

    // Partial for H2 should now include H1 (Enhancer + Emitter)
    output_t<NenAbility> res = huntech.get_partial_nen_ability(2);
    EXPECT_EQ(res.status(), StatusType::SUCCESS);

    // Check it's not just Emitter (Eff=1) but (Enh:1, Em:1 -> Eff=2)
    EXPECT_EQ(res.ans().getEffectiveNenAbility(), 2);
}


// ==========================================
// Memory Leak / Allocation Error Check (Mock)
// ==========================================
// Note: We cannot strictly test memory leaks without valgrind hooks,
// but we can ensure destructors run by scoping.
TEST_F(HuntechTest, DestructorRuns) {
    {
        Huntech scopedHuntech;
        scopedHuntech.add_squad(1);
        scopedHuntech.add_hunter(1, 1, createNen("Specialist"), 1000, 0);
    } // Should not crash
}