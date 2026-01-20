#include "catch2/catch_all.hpp"
#include "Huntech26a2.h"
#include "wet2util.h"

// Helper to create specific Nen abilities for testing
NenAbility createNen(std::string type) {
    return NenAbility(type);
}

TEST_CASE("Basic Squad Management", "[Squad]") {
    Huntech ht;

    SECTION("Add and Remove Squads") {
        // add_squad returns StatusType directly
        REQUIRE(ht.add_squad(1) == StatusType::SUCCESS);
        REQUIRE(ht.add_squad(2) == StatusType::SUCCESS);

        // Fail on duplicate
        REQUIRE(ht.add_squad(1) == StatusType::FAILURE);

        // Remove existing
        REQUIRE(ht.remove_squad(1) == StatusType::SUCCESS);

        // Remove non-existing
        REQUIRE(ht.remove_squad(1) == StatusType::FAILURE);
        REQUIRE(ht.remove_squad(3) == StatusType::FAILURE);

        // Reuse ID
        REQUIRE(ht.add_squad(1) == StatusType::SUCCESS);
    }

    SECTION("Initial Squad Stats") {
        ht.add_squad(10);
        // get_squad_experience returns output_t<int>, so .ans() is valid
        REQUIRE(ht.get_squad_experience(10).ans() == 0);

        // output_t has .status()
        REQUIRE(ht.get_ith_collective_aura_squad(0).status() == StatusType::FAILURE);
    }
}

TEST_CASE("Hunter Management", "[Hunter]") {
    Huntech ht;
    ht.add_squad(1);

    // Hunter Params: ID, SquadID, NenType, Aura, Fights
    NenAbility enhancer = createNen("Enhancer");

    SECTION("Add Hunter Success") {
        // add_hunter returns StatusType directly
        REQUIRE(ht.add_hunter(101, 1, enhancer, 100, 0) == StatusType::SUCCESS);
        REQUIRE(ht.get_hunter_fights_number(101).ans() == 0);
    }

    SECTION("Add Hunter Failures") {
        ht.add_hunter(101, 1, enhancer, 100, 0);

        // Duplicate Hunter ID
        REQUIRE(ht.add_hunter(101, 1, enhancer, 200, 5) == StatusType::FAILURE);

        // Non-existent Squad
        REQUIRE(ht.add_hunter(102, 999, enhancer, 100, 0) == StatusType::FAILURE);
    }

    SECTION("Hunter Persistence after Squad Removal") {
        ht.add_hunter(101, 1, enhancer, 100, 5);

        // Remove squad returns StatusType
        ht.remove_squad(1);

        // Hunter 101 data should still exist for fights query
        REQUIRE(ht.get_hunter_fights_number(101).ans() == 5);

        // But should fail for squad-dependent queries if implemented strictly
        REQUIRE(ht.get_partial_nen_ability(101).status() == StatusType::FAILURE);

        // ID 101 cannot be reused
        ht.add_squad(2);
        REQUIRE(ht.add_hunter(101, 2, enhancer, 100, 0) == StatusType::FAILURE);
    }
}

TEST_CASE("Collective Aura and Ranking", "[Calculations]") {
    Huntech ht;
    ht.add_squad(1);
    ht.add_squad(2);

    // Squad 1: 2 Hunters, Aura 10 and 30. Avg = 20.
    ht.add_hunter(101, 1, createNen("Enhancer"), 10, 0);
    ht.add_hunter(102, 1, createNen("Enhancer"), 30, 0);

    // Squad 2: 1 Hunter, Aura 100. Avg = 100.
    ht.add_hunter(201, 2, createNen("Emitter"), 100, 0);

    SECTION("Get i-th Collective Aura Squad") {
        // get_ith... returns output_t<int>
        output_t<int> rank0 = ht.get_ith_collective_aura_squad(1);
        output_t<int> rank1 = ht.get_ith_collective_aura_squad(2);

        REQUIRE(rank0.status() == StatusType::SUCCESS);
        REQUIRE(rank1.status() == StatusType::SUCCESS);

        // Verify distinctness
        REQUIRE(rank0.ans() != rank1.ans());
        bool has1 = (rank0.ans() == 1 || rank1.ans() == 1);
        bool has2 = (rank0.ans() == 2 || rank1.ans() == 2);
        REQUIRE(has1);
        REQUIRE(has2);
    }
}

TEST_CASE("Force Join Logic & Prefix Sums", "[Complex]") {
    Huntech ht;
    ht.add_squad(1); // Target
    ht.add_squad(2); // Source

    // Squad 1 Members: [H1, H2]
    // Nen: Enhancer (types[0]=1), Emitter (types[1]=1)
    NenAbility n1 = createNen("Enhancer");
    NenAbility n2 = createNen("Emitter");

    ht.add_hunter(101, 1, n1, 50, 0);
    ht.add_hunter(102, 1, n2, 50, 0);

    // Squad 2 Members: [H3]
    // Nen: Transmuter (types[2]=1)
    NenAbility n3 = createNen("Transmuter");
    ht.add_hunter(201, 2, n3, 50, 0);

    SECTION("Prefix Nen Ability Before Join") {
        // H2 partial sum should be H1 + H2
        output_t<NenAbility> res = ht.get_partial_nen_ability(102);
        REQUIRE(res.status() == StatusType::SUCCESS);

        // Expected: Enhancer + Emitter
        NenAbility expected = n1 + n2;
        REQUIRE(res.ans() == expected);
    }

    SECTION("Force Join Execution") {
        // We force stats such that Squad 1 is stronger than Squad 2 to ensure merge happens.

        // force_join returns StatusType directly
        REQUIRE(ht.force_join(1, 2) == StatusType::SUCCESS);

        // Squad 2 should be gone
        REQUIRE(ht.get_squad_experience(2).status() == StatusType::FAILURE);

        // Check Order Preservation: [H1, H2, H3]
        // H3 partial sum should be H1 + H2 + H3
        output_t<NenAbility> res = ht.get_partial_nen_ability(201);
        REQUIRE(res.status() == StatusType::SUCCESS);

        NenAbility expectedTotal = n1 + n2 + n3;
        REQUIRE(res.ans() == expectedTotal);
    }
}

TEST_CASE("Squad Duel Logic", "[Duel]") {
    Huntech ht;
    ht.add_squad(1);
    ht.add_squad(2);

    // Duel Formula: SquadExp + CollectiveAura (Avg)

    // Squad 1: Exp 0, 1 Hunter, Aura 100 -> Score 100
    ht.add_hunter(10, 1, createNen("Enhancer"), 100, 0);

    // Squad 2: Exp 0, 1 Hunter, Aura 20 -> Score 20
    ht.add_hunter(20, 2, createNen("Enhancer"), 20, 0);

    SECTION("Duel Calculation") {
        // squad_duel returns output_t<int>
        output_t<int> result = ht.squad_duel(1, 2);
        REQUIRE(result.status() == StatusType::SUCCESS);
        REQUIRE(result.ans() == 1); // Squad 1 wins
    }

    SECTION("Duel with Invalid Squads") {
        REQUIRE(ht.squad_duel(1, 99).status() == StatusType::FAILURE);
    }
}

TEST_CASE("Edge Cases", "[Edge]") {
    Huntech ht;

    SECTION("Empty Squad Operations") {
        ht.add_squad(1);
        // Getting partial nen of non-existent hunter
        REQUIRE(ht.get_partial_nen_ability(999).status() == StatusType::FAILURE);

        // Force joining empty squads
        ht.add_squad(2);
        // Returns StatusType
        ht.force_join(1, 2);
    }

    SECTION("Complex Merge Chain") {
        // A(1) -> B(2) -> C(3)
        // Merge A and B, then Result and C.
        ht.add_squad(1); ht.add_squad(2); ht.add_squad(3);

        NenAbility n = createNen("Enhancer");
        ht.add_hunter(101, 1, n, 1000, 0); // High stats to win merges
        ht.add_hunter(102, 2, n, 10, 0);
        ht.add_hunter(103, 3, n, 10, 0);

        // 1 absorbs 2
        ht.force_join(1, 2);
        // 1 absorbs 3
        ht.force_join(1, 3);

        // Check partial sum of last element (103)
        // Should be 101 + 102 + 103 (3 * Enhancer)
        NenAbility expected = n + n + n;
        REQUIRE(ht.get_partial_nen_ability(103).ans() == expected);
    }
}