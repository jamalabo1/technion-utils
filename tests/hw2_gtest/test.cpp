#include "gtest/gtest.h"
#include "Huntech26a2.h"
#include <string>

using namespace std;

// ==========================================
// Helper Macros & Functions
// ==========================================

// Use this for functions returning output_t<T>
#define ASSERT_OUT_STATUS(res, status_type) ASSERT_EQ(res.status(), status_type)
#define ASSERT_OUT_SUCCESS(res) ASSERT_OUT_STATUS(res, StatusType::SUCCESS)
#define ASSERT_OUT_FAILURE(res) ASSERT_OUT_STATUS(res, StatusType::FAILURE)
#define ASSERT_OUT_INVALID(res) ASSERT_OUT_STATUS(res, StatusType::INVALID_INPUT)

// Use this for functions returning StatusType directly
#define ASSERT_ENUM_STATUS(res, status_type) ASSERT_EQ(res, status_type)
#define ASSERT_ENUM_SUCCESS(res) ASSERT_ENUM_STATUS(res, StatusType::SUCCESS)
#define ASSERT_ENUM_FAILURE(res) ASSERT_ENUM_STATUS(res, StatusType::FAILURE)
#define ASSERT_ENUM_INVALID(res) ASSERT_ENUM_STATUS(res, StatusType::INVALID_INPUT)

NenAbility mkNen(string type) {
    return NenAbility(type);
}

class HuntechEdgeTest : public ::testing::Test {
protected:
    Huntech* ht;

    void SetUp() override {
        ht = new Huntech();
    }

    void TearDown() override {
        delete ht;
    }
};

// ==========================================
// 1. Invalid Input & Basic Error Handling
// ==========================================
TEST_F(HuntechEdgeTest, InvalidInputsEverything) {
    // add_squad (Returns StatusType -> Use ENUM macros)
    ASSERT_ENUM_INVALID(ht->add_squad(0));
    ASSERT_ENUM_INVALID(ht->add_squad(-1));

    // remove_squad (Returns StatusType)
    ASSERT_ENUM_INVALID(ht->remove_squad(0));
    ASSERT_ENUM_INVALID(ht->remove_squad(-10));

    // add_hunter (Returns StatusType)
    // Invalid IDs
    ASSERT_ENUM_INVALID(ht->add_hunter(0, 1, mkNen("Enhancer"), 10, 0)); 
    ASSERT_ENUM_INVALID(ht->add_hunter(1, 0, mkNen("Enhancer"), 10, 0));
    // Invalid Fights
    ASSERT_ENUM_INVALID(ht->add_hunter(1, 1, mkNen("Enhancer"), 10, -1));
    // Invalid Nen (Manually construct invalid)
    NenAbility badNen("NotARealType");
    ASSERT_ENUM_INVALID(ht->add_hunter(1, 1, badNen, 10, 0));

    // squad_duel (Returns output_t -> Use OUT macros)
    ASSERT_OUT_INVALID(ht->squad_duel(0, 1));
    ASSERT_OUT_INVALID(ht->squad_duel(1, -5));

    // force_join (Returns StatusType)
    ASSERT_ENUM_INVALID(ht->force_join(0, 1));
    ASSERT_ENUM_INVALID(ht->force_join(1, 0));
    ASSERT_ENUM_INVALID(ht->force_join(1, 1)); 

    // get_squad_experience (Returns output_t)
    ASSERT_OUT_INVALID(ht->get_squad_experience(0));

    // get_ith_collective_aura_squad (Returns output_t)
    ASSERT_OUT_FAILURE(ht->get_ith_collective_aura_squad(0)); // Spec: Failure if i < 1
    ASSERT_OUT_FAILURE(ht->get_ith_collective_aura_squad(1)); // Failure if empty

    // get_partial_nen_ability (Returns output_t)
    ASSERT_OUT_INVALID(ht->get_partial_nen_ability(0));
}

// ==========================================
// 2. The "Reincarnation" Ban (History Persistence)
// ==========================================
TEST_F(HuntechEdgeTest, HunterReincarnationBan) {
    ht->add_squad(1);
    ASSERT_ENUM_SUCCESS(ht->add_hunter(10, 1, mkNen("Enhancer"), 10, 0));

    // Remove squad - Hunter 10 is "killed"
    ASSERT_ENUM_SUCCESS(ht->remove_squad(1));

    // 1. Check data access on dead hunter (Returns output_t)
    // Fights: Should SUCCESS (PDF: "whether he is alive or killed")
    ASSERT_OUT_SUCCESS(ht->get_hunter_fights_number(10)); 
    // Partial Nen: Should FAILURE (PDF: "Failure if... he was killed")
    ASSERT_OUT_FAILURE(ht->get_partial_nen_ability(10));

    // 2. Try to add Hunter 10 again to a new squad (Returns StatusType)
    ht->add_squad(2);
    // PDF: "Failure if... existed in the past"
    ASSERT_ENUM_FAILURE(ht->add_hunter(10, 2, mkNen("Emitter"), 20, 0)); 
}

// ==========================================
// 3. Squad Duel Edge Cases
// ==========================================
TEST_F(HuntechEdgeTest, DuelEdgeCases) {
    ht->add_squad(1);
    ht->add_squad(2);

    // Case 1: Empty Squads Dueling
    // PDF: "Failure if... one of the squads is empty"
    ASSERT_OUT_FAILURE(ht->squad_duel(1, 2));

    ht->add_hunter(10, 1, mkNen("Enhancer"), 10, 0);
    // Squad 2 still empty
    ASSERT_OUT_FAILURE(ht->squad_duel(1, 2));

    ht->add_hunter(20, 2, mkNen("Enhancer"), 10, 0);
    // Now both valid.
    ASSERT_OUT_SUCCESS(ht->squad_duel(1, 2));
}

TEST_F(HuntechEdgeTest, DuelTieBreakerPerfectTie) {
    // Setup two identical squads
    ht->add_squad(1);
    ht->add_squad(2);

    // Same Type (Enhancer), Same Aura (10), Same Squad Exp (0)
    ht->add_hunter(10, 1, mkNen("Enhancer"), 10, 0);
    ht->add_hunter(20, 2, mkNen("Enhancer"), 10, 0);

    // Duel returns output_t
    output_t<int> res = ht->squad_duel(1, 2);
    ASSERT_OUT_SUCCESS(res);
    ASSERT_EQ(res.ans(), 0);

    // Check Exp (Both +1)
    ASSERT_OUT_SUCCESS(ht->get_squad_experience(1));
    ASSERT_EQ(ht->get_squad_experience(1).ans(), 1);
    ASSERT_EQ(ht->get_squad_experience(2).ans(), 1);
}

// ==========================================
// 4. Force Join Logic & Empty Squads
// ==========================================
TEST_F(HuntechEdgeTest, ForceJoinEmptyLogic) {
    ht->add_squad(1); // Empty
    ht->add_squad(2); // Empty
    ht->add_squad(3); // Not empty

    ht->add_hunter(30, 3, mkNen("Enhancer"), 100, 0);

    // Case 1: Empty forces Empty (Returns StatusType)
    // "Empty squad ... never can force" -> Failure
    ASSERT_ENUM_FAILURE(ht->force_join(1, 2));

    // Case 2: Empty forces Non-Empty
    // "Empty squad ... never can force" -> Failure
    ASSERT_ENUM_FAILURE(ht->force_join(1, 3));

    // Case 3: Non-Empty forces Empty
    // "Empty squad ... always can be forced by non-empty" -> Success
    ASSERT_ENUM_SUCCESS(ht->force_join(3, 1)); // 3 swallows 1
    
    // Check 1 is gone (get_squad returns output_t)
    ASSERT_OUT_FAILURE(ht->get_squad_experience(1));
    // Check 3 is still there
    ASSERT_OUT_SUCCESS(ht->get_squad_experience(3));
}

TEST_F(HuntechEdgeTest, ForceJoinInequalityStrictness) {
    ht->add_squad(1);
    ht->add_squad(2);

    // Create Identical Stats
    ht->add_hunter(10, 1, mkNen("Enhancer"), 100, 0);
    ht->add_hunter(20, 2, mkNen("Enhancer"), 100, 0);

    // Force Join requires STRICT inequality >
    ASSERT_ENUM_FAILURE(ht->force_join(1, 2));
    ASSERT_ENUM_FAILURE(ht->force_join(2, 1));

    // Tip the scales slightly for Squad 1
    ht->add_hunter(11, 1, mkNen("Specialist"), 1, 0); 
    
    // Now Squad 1 > Squad 2
    ASSERT_ENUM_SUCCESS(ht->force_join(1, 2));
}

// ==========================================
// 5. Complex Partial Nen (The Merge Chain)
// ==========================================
TEST_F(HuntechEdgeTest, DeepMergePartialNen) {
    // Setup 3 squads
    ht->add_squad(1); 
    ht->add_squad(2); 
    ht->add_squad(3); 

    NenAbility enh("Enhancer");   // (1,0,0,0,0,0)
    NenAbility emi("Emitter");    // (0,1,0,0,0,0)
    NenAbility tra("Transmuter"); // (0,0,1,0,0,0)

    // Give them huge aura differences so forces always work 1 > 2 > 3
    const int H1_ID = 1000, H2_ID = 2000, H3_ID = 3000;
    ht->add_hunter(H1_ID, 1, enh, 10000, 0);
    ht->add_hunter(H2_ID, 2, emi, 1000, 0);
    ht->add_hunter(H3_ID, 3, tra, 10, 0);

    // 1 forces 2.
    ASSERT_ENUM_SUCCESS(ht->force_join(1, 2));

    // Verify H2 Partial: H1 + H2
    ASSERT_EQ(ht->get_partial_nen_ability(H2_ID).ans(), enh + emi);

    // 1 forces 3.
    ASSERT_ENUM_SUCCESS(ht->force_join(1, 3));

    // Verify H3 Partial: H1 + H2 + H3
    ASSERT_EQ(ht->get_partial_nen_ability(H3_ID).ans(), enh + emi + tra);

    // Verify H2 Partial is still H1 + H2
    ASSERT_EQ(ht->get_partial_nen_ability(H2_ID).ans(), enh + emi);
}

TEST_F(HuntechEdgeTest, MergeOrderInverted) {
    ht->add_squad(1); // A
    ht->add_squad(2); // B
    ht->add_squad(3); // C

    NenAbility nA("Enhancer");
    NenAbility nB("Emitter");
    NenAbility nC("Transmuter");

    ht->add_hunter(10, 1, nA, 100, 0);
    ht->add_hunter(20, 2, nB, 10, 0);
    ht->add_hunter(30, 3, nC, 1000, 0); // C is strongest

    // 1 forces 2 => [10, 20] in Squad 1
    ASSERT_ENUM_SUCCESS(ht->force_join(1, 2));

    // 3 forces 1 => [30, 10, 20] in Squad 3
    ASSERT_ENUM_SUCCESS(ht->force_join(3, 1));

    // Check Partial Nen for the last guy (20 from original B)
    // Should be C + A + B
    output_t<NenAbility> res = ht->get_partial_nen_ability(20);
    ASSERT_OUT_SUCCESS(res);
    ASSERT_EQ(res.ans(), nC + nA + nB);
}

// ==========================================
// 6. Rank Tree (AVL) Stability & Sorting
// ==========================================
TEST_F(HuntechEdgeTest, RankTreeSortingRules) {
    ht->add_squad(1);
    ht->add_squad(2);
    
    ht->add_hunter(10, 1, mkNen("Enhancer"), 100, 0); // S1 = 100
    ht->add_hunter(20, 2, mkNen("Enhancer"), 200, 0); // S2 = 200
    
    // Edge case: Equal Aura, Different IDs.
    ht->add_squad(3);
    ht->add_hunter(30, 3, mkNen("Enhancer"), 100, 0); // S3 = 100

    // Expected Order (Ascending Aura, Ascending ID):
    // 1. S1 (100, ID 1)
    // 2. S3 (100, ID 3)
    // 3. S2 (200, ID 2)

    ASSERT_EQ(ht->get_ith_collective_aura_squad(1).ans(), 1);
    ASSERT_EQ(ht->get_ith_collective_aura_squad(2).ans(), 3);
    ASSERT_EQ(ht->get_ith_collective_aura_squad(3).ans(), 2);
}

// ==========================================
// 7. Stress / Volume Test (Mini)
// ==========================================
TEST_F(HuntechEdgeTest, MiniStressTest) {
    // Add 100 squads
    for(int i=1; i<=100; i++) {
        ASSERT_ENUM_SUCCESS(ht->add_squad(i));
        ht->add_hunter(i*100, i, mkNen("Specialist"), i, 0);
    }

    ASSERT_EQ(ht->get_ith_collective_aura_squad(50).ans(), 50);

    // Remove evens
    for(int i=2; i<=100; i+=2) {
        ASSERT_ENUM_SUCCESS(ht->remove_squad(i));
    }

    // Now 50 squads remain (the odds).
    // The 25th is 49.
    ASSERT_EQ(ht->get_ith_collective_aura_squad(25).ans(), 49);
}

// ==========================================
// 8. Hunter Fights Preservation
// ==========================================
TEST_F(HuntechEdgeTest, HunterFightsComplexPreservation) {
    ht->add_squad(1);
    ht->add_squad(2);
    ht->add_squad(3);
    ht->add_hunter(3, 3, mkNen("Enhancer"), 100, 20);

    // H1 starts with 10 fights
    ht->add_hunter(1, 1, mkNen("Enhancer"), 100, 10);
    // H2 starts with 20 fights
    ht->add_hunter(2, 2, mkNen("Enhancer"), 50, 20);

    // Duel S1 vs S2 (Both get +1) -> H1:11, H2:21
    ht->squad_duel(1, 2); 
    
    // S1 forces S2 (S2 removed, H2 moves to S1)
    // S1 now has H1(11), H2(21).
    ht->force_join(1, 2);

    // Duel S1 vs S3 (Everyone in S1 gets +1) -> H1:12, H2:22
    ht->squad_duel(1, 3);

    ASSERT_EQ(ht->get_hunter_fights_number(1).ans(), 12);
    ASSERT_EQ(ht->get_hunter_fights_number(2).ans(), 22);
}