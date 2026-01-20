// test_huntech.cpp
#define CATCH_CONFIG_MAIN
#include <catch2/catch_test_macros.hpp>

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
static void requireOutput(output_t<T> out, StatusType expectedStatus, const T& expectedAns) {
    REQUIRE(out.status() == expectedStatus);
    if (expectedStatus == StatusType::SUCCESS) {
        REQUIRE(out.ans() == expectedAns);
    }
}

// For NenAbility, equality operator is non-standard (matchup-based). We compare by printed vector.
static void requireOutputNen(output_t<NenAbility> out, StatusType expectedStatus, const NenAbility& expectedAns) {
    REQUIRE(out.status() == expectedStatus);
    if (expectedStatus == StatusType::SUCCESS) {
        REQUIRE(nenToString(out.ans()) == nenToString(expectedAns));
    }
}

TEST_CASE("cases_2: Empty system: invalid inputs and failure cases are handled") {
    Huntech h;

    SECTION("get_ith_collective_aura_squad fails when there are no squads") {
        auto out = h.get_ith_collective_aura_squad(1);
        REQUIRE(out.status() == StatusType::FAILURE);
    }

    SECTION("remove_squad fails when squad doesn't exist; invalid input is caught") {
        REQUIRE(h.remove_squad(1) == StatusType::FAILURE);
        REQUIRE(h.remove_squad(-7) == StatusType::INVALID_INPUT);
        REQUIRE(h.remove_squad(0) == StatusType::INVALID_INPUT);
    }

    SECTION("get_squad_experience fails for non-existing squad; invalid input is caught") {
        REQUIRE(h.get_squad_experience(1).status() == StatusType::FAILURE);
        REQUIRE(h.get_squad_experience(0).status() == StatusType::INVALID_INPUT);
        REQUIRE(h.get_squad_experience(-1).status() == StatusType::INVALID_INPUT);
    }

    SECTION("get_hunter_fights_number fails for hunter that never existed; invalid input is caught") {
        REQUIRE(h.get_hunter_fights_number(1).status() == StatusType::FAILURE);
        REQUIRE(h.get_hunter_fights_number(0).status() == StatusType::INVALID_INPUT);
        REQUIRE(h.get_hunter_fights_number(-1).status() == StatusType::INVALID_INPUT);
    }

    SECTION("get_partial_nen_ability fails for hunter that never existed; invalid input is caught") {
        REQUIRE(h.get_partial_nen_ability(123).status() == StatusType::FAILURE);
        REQUIRE(h.get_partial_nen_ability(0).status() == StatusType::INVALID_INPUT);
        REQUIRE(h.get_partial_nen_ability(-1).status() == StatusType::INVALID_INPUT);
    }

    SECTION("squad_duel invalid input + missing squads") {
        REQUIRE(h.squad_duel(1, 1).status() == StatusType::INVALID_INPUT);
        REQUIRE(h.squad_duel(0, 1).status() == StatusType::INVALID_INPUT);
        REQUIRE(h.squad_duel(1, 0).status() == StatusType::INVALID_INPUT);
        REQUIRE(h.squad_duel(-1, 2).status() == StatusType::INVALID_INPUT);

        REQUIRE(h.squad_duel(1, 2).status() == StatusType::FAILURE);
    }

    SECTION("force_join invalid input + missing squads") {
        REQUIRE(h.force_join(1, 1) == StatusType::INVALID_INPUT);
        REQUIRE(h.force_join(0, 1) == StatusType::INVALID_INPUT);
        REQUIRE(h.force_join(1, 0) == StatusType::INVALID_INPUT);
        REQUIRE(h.force_join(-1, 2) == StatusType::INVALID_INPUT);

        REQUIRE(h.force_join(1, 2) == StatusType::FAILURE);
    }
}

TEST_CASE("cases_2: add_squad / remove_squad basics and ID reuse after removal") {
    Huntech h;

    SECTION("add_squad invalid input") {
        REQUIRE(h.add_squad(0) == StatusType::INVALID_INPUT);
        REQUIRE(h.add_squad(-5) == StatusType::INVALID_INPUT);
    }

    SECTION("add_squad success then duplicate fails") {
        REQUIRE(h.add_squad(10) == StatusType::SUCCESS);
        REQUIRE(h.add_squad(10) == StatusType::FAILURE);
    }

    SECTION("remove_squad success and squadId can be reused as a NEW squad") {
        REQUIRE(h.add_squad(7) == StatusType::SUCCESS);
        REQUIRE(h.remove_squad(7) == StatusType::SUCCESS);

        // After deletion, the same ID may be added again as a new active squad (fresh exp=0, empty)
        REQUIRE(h.add_squad(7) == StatusType::SUCCESS);
        requireOutput<int>(h.get_squad_experience(7), StatusType::SUCCESS, 0);
    }
}

TEST_CASE("cases_2: add_hunter: validation, non-existent squad, uniqueness even after death") {
    Huntech h;
    REQUIRE(h.add_squad(1) == StatusType::SUCCESS);

    SECTION("invalid input checks") {
        REQUIRE(h.add_hunter(0, 1, nen("Enhancer"), 0, 0) == StatusType::INVALID_INPUT);
        REQUIRE(h.add_hunter(-1, 1, nen("Enhancer"), 0, 0) == StatusType::INVALID_INPUT);
        REQUIRE(h.add_hunter(1, 0, nen("Enhancer"), 0, 0) == StatusType::INVALID_INPUT);
        REQUIRE(h.add_hunter(1, -3, nen("Enhancer"), 0, 0) == StatusType::INVALID_INPUT);

        REQUIRE(h.add_hunter(1, 1, NenAbility("NotAType"), 0, 0) == StatusType::INVALID_INPUT);
        REQUIRE(h.add_hunter(1, 1, nen("Enhancer"), -1, 0) == StatusType::INVALID_INPUT);
        REQUIRE(h.add_hunter(1, 1, nen("Enhancer"), 0, -1) == StatusType::INVALID_INPUT);
    }

    SECTION("fails when squad doesn't exist") {
        REQUIRE(h.add_hunter(1, 999, nen("Enhancer"), 0, 0) == StatusType::FAILURE);
    }

    SECTION("success; hunterId cannot be reused in same or different squad") {
        REQUIRE(h.add_hunter(100, 1, nen("Enhancer"), 5, 2) == StatusType::SUCCESS);

        REQUIRE(h.add_hunter(100, 1, nen("Emitter"), 3, 0) == StatusType::FAILURE);

        REQUIRE(h.add_squad(2) == StatusType::SUCCESS);
        REQUIRE(h.add_hunter(100, 2, nen("Emitter"), 3, 0) == StatusType::FAILURE);

        requireOutput<int>(h.get_hunter_fights_number(100), StatusType::SUCCESS, 2);
        requireOutputNen(h.get_partial_nen_ability(100), StatusType::SUCCESS, nen("Enhancer"));
    }
}

TEST_CASE("cases_2: get_ith_collective_aura_squad: ordering by total aura, then squadId; bounds are FAILURE") {
    Huntech h;

    REQUIRE(h.add_squad(1) == StatusType::SUCCESS);
    REQUIRE(h.add_squad(2) == StatusType::SUCCESS);
    REQUIRE(h.add_squad(3) == StatusType::SUCCESS);

    SECTION("initially all total aura are 0; tie-break by squadId") {
        requireOutput<int>(h.get_ith_collective_aura_squad(1), StatusType::SUCCESS, 1);
        requireOutput<int>(h.get_ith_collective_aura_squad(2), StatusType::SUCCESS, 2);
        requireOutput<int>(h.get_ith_collective_aura_squad(3), StatusType::SUCCESS, 3);

        REQUIRE(h.get_ith_collective_aura_squad(0).status() == StatusType::FAILURE);
        REQUIRE(h.get_ith_collective_aura_squad(-7).status() == StatusType::FAILURE);
        REQUIRE(h.get_ith_collective_aura_squad(4).status() == StatusType::FAILURE);
    }

    SECTION("ordering updates after adding hunters; tie-break by squadId for same aura") {
        REQUIRE(h.add_hunter(10, 1, nen("Enhancer"), 5, 0) == StatusType::SUCCESS); // aura1=5
        REQUIRE(h.add_hunter(20, 2, nen("Conjurer"), 5, 0) == StatusType::SUCCESS); // aura2=5
        // squad3 aura=0

        // by total aura ascending: squad3(0), squad1(5), squad2(5; id bigger)
        requireOutput<int>(h.get_ith_collective_aura_squad(1), StatusType::SUCCESS, 3);
        requireOutput<int>(h.get_ith_collective_aura_squad(2), StatusType::SUCCESS, 1);
        requireOutput<int>(h.get_ith_collective_aura_squad(3), StatusType::SUCCESS, 2);

        REQUIRE(h.add_hunter(21, 2, nen("Emitter"), 1, 0) == StatusType::SUCCESS); // aura2=6

        requireOutput<int>(h.get_ith_collective_aura_squad(1), StatusType::SUCCESS, 3);
        requireOutput<int>(h.get_ith_collective_aura_squad(2), StatusType::SUCCESS, 1);
        requireOutput<int>(h.get_ith_collective_aura_squad(3), StatusType::SUCCESS, 2);
    }

    SECTION("removing a squad removes it from ordering") {
        REQUIRE(h.add_hunter(10, 1, nen("Enhancer"), 5, 0) == StatusType::SUCCESS);
        REQUIRE(h.add_hunter(20, 2, nen("Conjurer"), 1, 0) == StatusType::SUCCESS);

        REQUIRE(h.remove_squad(2) == StatusType::SUCCESS);

        // remaining: squads 1 (aura 5), 3 (aura 0)
        requireOutput<int>(h.get_ith_collective_aura_squad(1), StatusType::SUCCESS, 3);
        requireOutput<int>(h.get_ith_collective_aura_squad(2), StatusType::SUCCESS, 1);
        REQUIRE(h.get_ith_collective_aura_squad(3).status() == StatusType::FAILURE);
    }
}

TEST_CASE("cases_2: squad_duel: invalid inputs, empty squads, result codes, exp updates, fights increment") {
    Huntech h;

    REQUIRE(h.add_squad(1) == StatusType::SUCCESS);
    REQUIRE(h.add_squad(2) == StatusType::SUCCESS);

    SECTION("fails if one squad is empty (including both empty)") {
        REQUIRE(h.squad_duel(1, 2).status() == StatusType::FAILURE);

        REQUIRE(h.add_hunter(10, 1, nen("Enhancer"), 1, 0) == StatusType::SUCCESS);
        REQUIRE(h.squad_duel(1, 2).status() == StatusType::FAILURE);
    }

    SECTION("aura-based win: code 1 or 3; winner exp +3; all fights +1") {
        REQUIRE(h.add_hunter(11, 1, nen("Enhancer"), 5, 2) == StatusType::SUCCESS);
        REQUIRE(h.add_hunter(21, 2, nen("Emitter"), 6, 7) == StatusType::SUCCESS);

        // effective battle aura: squad1=5, squad2=6 => squad2 wins by aura => code 3
        auto duel = h.squad_duel(1, 2);
        REQUIRE(duel.status() == StatusType::SUCCESS);
        REQUIRE(duel.ans() == 3);

        requireOutput<int>(h.get_squad_experience(1), StatusType::SUCCESS, 0);
        requireOutput<int>(h.get_squad_experience(2), StatusType::SUCCESS, 3);

        requireOutput<int>(h.get_hunter_fights_number(11), StatusType::SUCCESS, 3);
        requireOutput<int>(h.get_hunter_fights_number(21), StatusType::SUCCESS, 8);
    }

    SECTION("full tie: code 0; both exp +1; all fights +1") {
        REQUIRE(h.add_hunter(1, 1, nen("Enhancer"), 0, 0) == StatusType::SUCCESS);
        REQUIRE(h.add_hunter(2, 2, nen("Enhancer"), 0, 0) == StatusType::SUCCESS);

        auto duel = h.squad_duel(1, 2);
        REQUIRE(duel.status() == StatusType::SUCCESS);
        REQUIRE(duel.ans() == 0);

        requireOutput<int>(h.get_squad_experience(1), StatusType::SUCCESS, 1);
        requireOutput<int>(h.get_squad_experience(2), StatusType::SUCCESS, 1);

        requireOutput<int>(h.get_hunter_fights_number(1), StatusType::SUCCESS, 1);
        requireOutput<int>(h.get_hunter_fights_number(2), StatusType::SUCCESS, 1);
    }

    SECTION("nen-based win: when effective battle aura ties, return 2 or 4; winner exp +3; all fights +1") {
        REQUIRE(h.add_hunter(1, 1, nen("Enhancer"), 0, 0) == StatusType::SUCCESS);
        REQUIRE(h.add_hunter(2, 2, nen("Conjurer"), 0, 0) == StatusType::SUCCESS);

        // Compute the expected winner using NenAbility operators (same semantics required by spec)
        NenAbility squad1Nen = nen("Enhancer");
        NenAbility squad2Nen = nen("Conjurer");

        int expectedCode = 0;
        if (squad1Nen > squad2Nen) expectedCode = 2;
        else if (squad2Nen > squad1Nen) expectedCode = 4;
        else expectedCode = 0;

        auto duel = h.squad_duel(1, 2);
        REQUIRE(duel.status() == StatusType::SUCCESS);
        REQUIRE(duel.ans() == expectedCode);

        if (expectedCode == 2) {
            requireOutput<int>(h.get_squad_experience(1), StatusType::SUCCESS, 3);
            requireOutput<int>(h.get_squad_experience(2), StatusType::SUCCESS, 0);
        } else if (expectedCode == 4) {
            requireOutput<int>(h.get_squad_experience(1), StatusType::SUCCESS, 0);
            requireOutput<int>(h.get_squad_experience(2), StatusType::SUCCESS, 3);
        } else {
            requireOutput<int>(h.get_squad_experience(1), StatusType::SUCCESS, 1);
            requireOutput<int>(h.get_squad_experience(2), StatusType::SUCCESS, 1);
        }

        requireOutput<int>(h.get_hunter_fights_number(1), StatusType::SUCCESS, 1);
        requireOutput<int>(h.get_hunter_fights_number(2), StatusType::SUCCESS, 1);
    }
}

TEST_CASE("cases_2: get_partial_nen_ability: chronological prefix within squad") {
    Huntech h;
    REQUIRE(h.add_squad(1) == StatusType::SUCCESS);

    REQUIRE(h.add_hunter(1, 1, nen("Enhancer"), 0, 0) == StatusType::SUCCESS);
    REQUIRE(h.add_hunter(2, 1, nen("Emitter"), 0, 0) == StatusType::SUCCESS);
    REQUIRE(h.add_hunter(3, 1, nen("Enhancer"), 0, 0) == StatusType::SUCCESS);

    NenAbility expected1 = nen("Enhancer");
    NenAbility expected2 = nen("Enhancer") + nen("Emitter");
    NenAbility expected3 = nen("Enhancer") + nen("Emitter") + nen("Enhancer");

    requireOutputNen(h.get_partial_nen_ability(1), StatusType::SUCCESS, expected1);
    requireOutputNen(h.get_partial_nen_ability(2), StatusType::SUCCESS, expected2);
    requireOutputNen(h.get_partial_nen_ability(3), StatusType::SUCCESS, expected3);
}

TEST_CASE("cases_2: force_join: success merges squads, preserves fights, concatenates chronological order, removes forced squad") {
    Huntech h;

    REQUIRE(h.add_squad(1) == StatusType::SUCCESS);
    REQUIRE(h.add_squad(2) == StatusType::SUCCESS);

    REQUIRE(h.add_hunter(10, 1, nen("Enhancer"), 1, 0) == StatusType::SUCCESS);
    REQUIRE(h.add_hunter(11, 1, nen("Emitter"), 1, 0) == StatusType::SUCCESS);

    REQUIRE(h.add_hunter(20, 2, nen("Transmuter"), 0, 0) == StatusType::SUCCESS);

    REQUIRE(h.force_join(1, 2) == StatusType::SUCCESS);

    REQUIRE(h.get_squad_experience(2).status() == StatusType::FAILURE);

    // fights unchanged by force_join
    requireOutput<int>(h.get_hunter_fights_number(10), StatusType::SUCCESS, 0);
    requireOutput<int>(h.get_hunter_fights_number(11), StatusType::SUCCESS, 0);
    requireOutput<int>(h.get_hunter_fights_number(20), StatusType::SUCCESS, 0);

    // Chronological concatenation: (10, 11, 20)
    NenAbility expected10 = nen("Enhancer");
    NenAbility expected11 = nen("Enhancer") + nen("Emitter");
    NenAbility expected20 = nen("Enhancer") + nen("Emitter") + nen("Transmuter");

    requireOutputNen(h.get_partial_nen_ability(10), StatusType::SUCCESS, expected10);
    requireOutputNen(h.get_partial_nen_ability(11), StatusType::SUCCESS, expected11);
    requireOutputNen(h.get_partial_nen_ability(20), StatusType::SUCCESS, expected20);

    // After force_join, forced squadId should no longer be active and can be added again
    REQUIRE(h.add_squad(2) == StatusType::SUCCESS);
    requireOutput<int>(h.get_squad_experience(2), StatusType::SUCCESS, 0);
}

TEST_CASE("cases_2: force_join: failure conditions (empty forcing squad, non-strict inequality, missing squads)") {
    Huntech h;
    REQUIRE(h.add_squad(1) == StatusType::SUCCESS);
    REQUIRE(h.add_squad(2) == StatusType::SUCCESS);

    SECTION("forcing squad empty -> FAILURE") {
        REQUIRE(h.add_hunter(200, 2, nen("Enhancer"), 0, 0) == StatusType::SUCCESS);
        REQUIRE(h.force_join(1, 2) == StatusType::FAILURE);
    }

    SECTION("non-strict inequality -> FAILURE") {
        // Both: exp=0, totalAura=0, effectiveNenAbility=1 => equality => should fail
        REQUIRE(h.add_hunter(1, 1, nen("Enhancer"), 0, 0) == StatusType::SUCCESS);
        REQUIRE(h.add_hunter(2, 2, nen("Emitter"), 0, 0) == StatusType::SUCCESS);

        REQUIRE(h.force_join(1, 2) == StatusType::FAILURE);
        REQUIRE(h.force_join(2, 1) == StatusType::FAILURE);
    }

    SECTION("missing squads -> FAILURE") {
        REQUIRE(h.force_join(1, 999) == StatusType::FAILURE);
        REQUIRE(h.force_join(999, 1) == StatusType::FAILURE);
    }
}

TEST_CASE("cases_2: remove_squad kills its hunters: dead hunters keep fights-number but get_partial_nen_ability fails; hunterId can't be reused") {
    Huntech h;

    REQUIRE(h.add_squad(1) == StatusType::SUCCESS);
    REQUIRE(h.add_hunter(10, 1, nen("Enhancer"), 0, 5) == StatusType::SUCCESS);

    requireOutput<int>(h.get_hunter_fights_number(10), StatusType::SUCCESS, 5);
    requireOutputNen(h.get_partial_nen_ability(10), StatusType::SUCCESS, nen("Enhancer"));

    REQUIRE(h.remove_squad(1) == StatusType::SUCCESS);

    REQUIRE(h.get_squad_experience(1).status() == StatusType::FAILURE);

    requireOutput<int>(h.get_hunter_fights_number(10), StatusType::SUCCESS, 5);
    REQUIRE(h.get_partial_nen_ability(10).status() == StatusType::FAILURE);

    REQUIRE(h.add_squad(1) == StatusType::SUCCESS);
    REQUIRE(h.add_hunter(10, 1, nen("Emitter"), 0, 0) == StatusType::FAILURE);
}

TEST_CASE("cases_2: Integration sanity: duel updates fights/exp; ordering by aura unaffected; force_join either succeeds or fails without corruption") {
    Huntech h;

    REQUIRE(h.add_squad(1) == StatusType::SUCCESS);
    REQUIRE(h.add_squad(2) == StatusType::SUCCESS);
    REQUIRE(h.add_squad(3) == StatusType::SUCCESS);

    REQUIRE(h.add_hunter(101, 1, nen("Enhancer"), 2, 0) == StatusType::SUCCESS);
    REQUIRE(h.add_hunter(201, 2, nen("Emitter"), 2, 0) == StatusType::SUCCESS);
    // squad3 empty

    // By aura: squad3(0), squad1(2), squad2(2 tie => id 1 then 2)
    requireOutput<int>(h.get_ith_collective_aura_squad(1), StatusType::SUCCESS, 3);
    requireOutput<int>(h.get_ith_collective_aura_squad(2), StatusType::SUCCESS, 1);
    requireOutput<int>(h.get_ith_collective_aura_squad(3), StatusType::SUCCESS, 2);

    auto duel = h.squad_duel(1, 2);
    REQUIRE(duel.status() == StatusType::SUCCESS);

    requireOutput<int>(h.get_hunter_fights_number(101), StatusType::SUCCESS, 1);
    requireOutput<int>(h.get_hunter_fights_number(201), StatusType::SUCCESS, 1);

    // Make squad1 stronger to increase chance of strict inequality
    REQUIRE(h.add_hunter(102, 1, nen("Conjurer"), 0, 0) == StatusType::SUCCESS);

    StatusType st = h.force_join(1, 2);
    if (st == StatusType::SUCCESS) {
        REQUIRE(h.get_squad_experience(2).status() == StatusType::FAILURE);
        REQUIRE(h.get_partial_nen_ability(101).status() == StatusType::SUCCESS);
        REQUIRE(h.get_partial_nen_ability(201).status() == StatusType::SUCCESS);
    } else {
        REQUIRE(st == StatusType::FAILURE);
        REQUIRE(h.get_squad_experience(1).status() == StatusType::SUCCESS);
        REQUIRE(h.get_squad_experience(2).status() == StatusType::SUCCESS);
    }
}
