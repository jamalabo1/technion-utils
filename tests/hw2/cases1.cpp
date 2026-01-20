#include "catch2/catch_all.hpp"
#include "Huntech26a2.h"

// Helper to create NenAbility types
NenAbility makeNen(const std::string &type) {
    return NenAbility(type);
}

// ============================================================================
// CONSTRUCTOR & DESTRUCTOR TESTS
// ============================================================================

TEST_CASE("Huntech constructor creates empty system", "[constructor]") {
    Huntech ht;
    // System should be empty - no squads or hunters
    REQUIRE(ht.get_ith_collective_aura_squad(1).status() == StatusType::FAILURE);
}

TEST_CASE("Huntech destructor works correctly", "[destructor]") {
    Huntech *ht = new Huntech();
    ht->add_squad(1);
    ht->add_hunter(1, 1, makeNen("Enhancer"), 100, 5);
    delete ht; // Should not leak or crash
    REQUIRE(true);
}

// ============================================================================
// ADD_SQUAD TESTS
// ============================================================================

TEST_CASE("add_squad: Invalid input - non-positive IDs", "[add_squad]") {
    Huntech ht;
    REQUIRE(ht.add_squad(0) == StatusType::INVALID_INPUT);
    REQUIRE(ht.add_squad(-1) == StatusType::INVALID_INPUT);
    REQUIRE(ht.add_squad(-999) == StatusType::INVALID_INPUT);
}

TEST_CASE("add_squad: Success cases", "[add_squad]") {
    Huntech ht;
    REQUIRE(ht.add_squad(1) == StatusType::SUCCESS);
    REQUIRE(ht.add_squad(100) == StatusType::SUCCESS);
    REQUIRE(ht.add_squad(5) == StatusType::SUCCESS);
}

TEST_CASE("add_squad: Failure - duplicate squad ID", "[add_squad]") {
    Huntech ht;
    REQUIRE(ht.add_squad(1) == StatusType::SUCCESS);
    REQUIRE(ht.add_squad(1) == StatusType::FAILURE);
    REQUIRE(ht.add_squad(1) == StatusType::FAILURE);
}

TEST_CASE("add_squad: Squad ID can be reused after removal", "[add_squad]") {
    Huntech ht;
    REQUIRE(ht.add_squad(1) == StatusType::SUCCESS);
    REQUIRE(ht.remove_squad(1) == StatusType::SUCCESS);
    REQUIRE(ht.add_squad(1) == StatusType::SUCCESS); // Reuse allowed
}

TEST_CASE("add_squad: New squad has zero experience", "[add_squad]") {
    Huntech ht;
    ht.add_squad(1);
    auto res = ht.get_squad_experience(1);
    REQUIRE(res.status() == StatusType::SUCCESS);
    REQUIRE(res.ans() == 0);
}

// ============================================================================
// REMOVE_SQUAD TESTS
// ============================================================================

TEST_CASE("remove_squad: Invalid input - non-positive IDs", "[remove_squad]") {
    Huntech ht;
    REQUIRE(ht.remove_squad(0) == StatusType::INVALID_INPUT);
    REQUIRE(ht.remove_squad(-5) == StatusType::INVALID_INPUT);
}

TEST_CASE("remove_squad: Failure - non-existent squad", "[remove_squad]") {
    Huntech ht;
    REQUIRE(ht.remove_squad(999) == StatusType::FAILURE);
}

TEST_CASE("remove_squad: Success - remove existing squad", "[remove_squad]") {
    Huntech ht;
    ht.add_squad(1);
    REQUIRE(ht.remove_squad(1) == StatusType::SUCCESS);
    REQUIRE(ht.remove_squad(1) == StatusType::FAILURE); // Already removed
}

TEST_CASE("remove_squad: Removes all hunters in squad", "[remove_squad]") {
    Huntech ht;
    ht.add_squad(1);
    ht.add_hunter(10, 1, makeNen("Enhancer"), 100, 5);
    ht.add_hunter(20, 1, makeNen("Emitter"), 50, 3);

    REQUIRE(ht.remove_squad(1) == StatusType::SUCCESS);

    // Hunters are dead - get_partial_nen_ability should fail
    REQUIRE(ht.get_partial_nen_ability(10).status() == StatusType::FAILURE);
    REQUIRE(ht.get_partial_nen_ability(20).status() == StatusType::FAILURE);

    // But fights count should still be accessible (hunter existed)
    auto fights10 = ht.get_hunter_fights_number(10);
    REQUIRE(fights10.status() == StatusType::SUCCESS);
    REQUIRE(fights10.ans() == 5);
}

// ============================================================================
// ADD_HUNTER TESTS
// ============================================================================

TEST_CASE("add_hunter: Invalid input - non-positive hunter ID", "[add_hunter]") {
    Huntech ht;
    ht.add_squad(1);
    REQUIRE(ht.add_hunter(0, 1, makeNen("Enhancer"), 100, 5) == StatusType::INVALID_INPUT);
    REQUIRE(ht.add_hunter(-1, 1, makeNen("Enhancer"), 100, 5) == StatusType::INVALID_INPUT);
}

TEST_CASE("add_hunter: Invalid input - non-positive squad ID", "[add_hunter]") {
    Huntech ht;
    REQUIRE(ht.add_hunter(1, 0, makeNen("Enhancer"), 100, 5) == StatusType::INVALID_INPUT);
    REQUIRE(ht.add_hunter(1, -5, makeNen("Enhancer"), 100, 5) == StatusType::INVALID_INPUT);
}

TEST_CASE("add_hunter: Invalid input - invalid NenAbility", "[add_hunter]") {
    Huntech ht;
    ht.add_squad(1);
    REQUIRE(ht.add_hunter(1, 1, NenAbility::invalid(), 100, 5) == StatusType::INVALID_INPUT);
    REQUIRE(ht.add_hunter(2, 1, makeNen("Invalid"), 100, 5) == StatusType::INVALID_INPUT);
}

TEST_CASE("add_hunter: Invalid input - negative aura", "[add_hunter]") {
    Huntech ht;
    ht.add_squad(1);
    REQUIRE(ht.add_hunter(1, 1, makeNen("Enhancer"), -1, 5) == StatusType::INVALID_INPUT);
    REQUIRE(ht.add_hunter(2, 1, makeNen("Enhancer"), -100, 5) == StatusType::INVALID_INPUT);
}

TEST_CASE("add_hunter: Invalid input - negative fights", "[add_hunter]") {
    Huntech ht;
    ht.add_squad(1);
    REQUIRE(ht.add_hunter(1, 1, makeNen("Enhancer"), 100, -1) == StatusType::INVALID_INPUT);
    REQUIRE(ht.add_hunter(2, 1, makeNen("Enhancer"), 100, -10) == StatusType::INVALID_INPUT);
}

TEST_CASE("add_hunter: Valid input with zero aura and fights", "[add_hunter]") {
    Huntech ht;
    ht.add_squad(1);
    REQUIRE(ht.add_hunter(1, 1, makeNen("Enhancer"), 0, 0) == StatusType::SUCCESS);
}

TEST_CASE("add_hunter: Failure - squad does not exist", "[add_hunter]") {
    Huntech ht;
    REQUIRE(ht.add_hunter(1, 999, makeNen("Enhancer"), 100, 5) == StatusType::FAILURE);
}

TEST_CASE("add_hunter: Failure - duplicate hunter ID", "[add_hunter]") {
    Huntech ht;
    ht.add_squad(1);
    REQUIRE(ht.add_hunter(1, 1, makeNen("Enhancer"), 100, 5) == StatusType::SUCCESS);
    REQUIRE(ht.add_hunter(1, 1, makeNen("Emitter"), 50, 3) == StatusType::FAILURE);
}

TEST_CASE("add_hunter: Failure - hunter ID reused after death", "[add_hunter]") {
    Huntech ht;
    ht.add_squad(1);
    ht.add_hunter(10, 1, makeNen("Enhancer"), 100, 5);
    ht.remove_squad(1); // Hunter 10 dies

    ht.add_squad(2);
    // Cannot reuse hunter ID even after death
    REQUIRE(ht.add_hunter(10, 2, makeNen("Emitter"), 50, 3) == StatusType::FAILURE);
}

TEST_CASE("add_hunter: Success - multiple hunters in squad", "[add_hunter]") {
    Huntech ht;
    ht.add_squad(1);
    REQUIRE(ht.add_hunter(1, 1, makeNen("Enhancer"), 100, 5) == StatusType::SUCCESS);
    REQUIRE(ht.add_hunter(2, 1, makeNen("Emitter"), 50, 3) == StatusType::SUCCESS);
    REQUIRE(ht.add_hunter(3, 1, makeNen("Specialist"), 200, 10) == StatusType::SUCCESS);
}

TEST_CASE("add_hunter: All NenAbility types valid", "[add_hunter]") {
    Huntech ht;
    ht.add_squad(1);
    REQUIRE(ht.add_hunter(1, 1, makeNen("Enhancer"), 100, 0) == StatusType::SUCCESS);
    REQUIRE(ht.add_hunter(2, 1, makeNen("Emitter"), 100, 0) == StatusType::SUCCESS);
    REQUIRE(ht.add_hunter(3, 1, makeNen("Transmuter"), 100, 0) == StatusType::SUCCESS);
    REQUIRE(ht.add_hunter(4, 1, makeNen("Conjurer"), 100, 0) == StatusType::SUCCESS);
    REQUIRE(ht.add_hunter(5, 1, makeNen("Manipulator"), 100, 0) == StatusType::SUCCESS);
    REQUIRE(ht.add_hunter(6, 1, makeNen("Specialist"), 100, 0) == StatusType::SUCCESS);
}

// ============================================================================
// SQUAD_DUEL TESTS
// ============================================================================

TEST_CASE("squad_duel: Invalid input - non-positive IDs", "[squad_duel]") {
    Huntech ht;
    REQUIRE(ht.squad_duel(0, 1).status() == StatusType::INVALID_INPUT);
    REQUIRE(ht.squad_duel(1, 0).status() == StatusType::INVALID_INPUT);
    REQUIRE(ht.squad_duel(-1, 1).status() == StatusType::INVALID_INPUT);
    REQUIRE(ht.squad_duel(1, -1).status() == StatusType::INVALID_INPUT);
}

TEST_CASE("squad_duel: Invalid input - same squad ID", "[squad_duel]") {
    Huntech ht;
    ht.add_squad(1);
    REQUIRE(ht.squad_duel(1, 1).status() == StatusType::INVALID_INPUT);
}

TEST_CASE("squad_duel: Failure - squad does not exist", "[squad_duel]") {
    Huntech ht;
    ht.add_squad(1);
    REQUIRE(ht.squad_duel(1, 999).status() == StatusType::FAILURE);
    REQUIRE(ht.squad_duel(999, 1).status() == StatusType::FAILURE);
}

TEST_CASE("squad_duel: Failure - empty squad", "[squad_duel]") {
    Huntech ht;
    ht.add_squad(1);
    ht.add_squad(2);
    REQUIRE(ht.squad_duel(1, 2).status() == StatusType::FAILURE);

    ht.add_hunter(1, 1, makeNen("Enhancer"), 100, 0);
    REQUIRE(ht.squad_duel(1, 2).status() == StatusType::FAILURE); // Squad 2 empty
}

TEST_CASE("squad_duel: Squad 1 wins by effective battle aura", "[squad_duel]") {
    Huntech ht;
    ht.add_squad(1);
    ht.add_squad(2);
    ht.add_hunter(1, 1, makeNen("Enhancer"), 100, 0);
    ht.add_hunter(2, 2, makeNen("Enhancer"), 50, 0);

    auto res = ht.squad_duel(1, 2);
    REQUIRE(res.status() == StatusType::SUCCESS);
    REQUIRE(res.ans() == 1); // Squad 1 wins by aura

    // Check experience updates
    auto exp1 = ht.get_squad_experience(1);
    auto exp2 = ht.get_squad_experience(2);
    REQUIRE(exp1.ans() == 3); // Winner gets +3
    REQUIRE(exp2.ans() == 0); // Loser unchanged

    // Check fights updated
    auto fights1 = ht.get_hunter_fights_number(1);
    auto fights2 = ht.get_hunter_fights_number(2);
    REQUIRE(fights1.ans() == 1);
    REQUIRE(fights2.ans() == 1);
}

TEST_CASE("squad_duel: Squad 2 wins by effective battle aura", "[squad_duel]") {
    Huntech ht;
    ht.add_squad(1);
    ht.add_squad(2);
    ht.add_hunter(1, 1, makeNen("Enhancer"), 50, 0);
    ht.add_hunter(2, 2, makeNen("Enhancer"), 100, 0);

    auto res = ht.squad_duel(1, 2);
    REQUIRE(res.status() == StatusType::SUCCESS);
    REQUIRE(res.ans() == 3); // Squad 2 wins by aura

    auto exp1 = ht.get_squad_experience(1);
    auto exp2 = ht.get_squad_experience(2);
    REQUIRE(exp1.ans() == 0);
    REQUIRE(exp2.ans() == 3);
}

TEST_CASE("squad_duel: Squad 1 wins by NenAbility", "[squad_duel]") {
    Huntech ht;
    ht.add_squad(1);
    ht.add_squad(2);
    // Equal aura, but Enhancer > Transmuter
    ht.add_hunter(1, 1, makeNen("Enhancer"), 100, 0);
    ht.add_hunter(2, 2, makeNen("Transmuter"), 100, 0);

    auto res = ht.squad_duel(1, 2);
    REQUIRE(res.status() == StatusType::SUCCESS);
    REQUIRE(res.ans() == 2); // Squad 1 wins by Nen

    auto exp1 = ht.get_squad_experience(1);
    REQUIRE(exp1.ans() == 3);
}

TEST_CASE("squad_duel: Squad 2 wins by NenAbility", "[squad_duel]") {
    Huntech ht;
    ht.add_squad(1);
    ht.add_squad(2);
    // Equal aura, but Emitter > Enhancer
    ht.add_hunter(1, 1, makeNen("Emitter"), 100, 0);
    ht.add_hunter(2, 2, makeNen("Enhancer"), 100, 0);
    bool isGreater = makeNen("Enhancer") < makeNen("Emitter");
    auto res = ht.squad_duel(1, 2);
    REQUIRE(res.status() == StatusType::SUCCESS);
    REQUIRE(res.ans() == 4); // Squad 2 wins by Nen

    auto exp2 = ht.get_squad_experience(2);
    REQUIRE(exp2.ans() == 3);
}

TEST_CASE("squad_duel: Tie - equal aura and NenAbility", "[squad_duel]") {
    Huntech ht;
    ht.add_squad(1);
    ht.add_squad(2);
    ht.add_hunter(1, 1, makeNen("Enhancer"), 100, 0);
    ht.add_hunter(2, 2, makeNen("Enhancer"), 100, 0);

    auto res = ht.squad_duel(1, 2);
    REQUIRE(res.status() == StatusType::SUCCESS);
    REQUIRE(res.ans() == 0); // Tie

    // Both get +1 experience
    auto exp1 = ht.get_squad_experience(1);
    auto exp2 = ht.get_squad_experience(2);
    REQUIRE(exp1.ans() == 1);
    REQUIRE(exp2.ans() == 1);
}

TEST_CASE("squad_duel: Experience affects battle aura", "[squad_duel]") {
    Huntech ht;
    ht.add_squad(1);
    ht.add_squad(2);
    ht.add_hunter(1, 1, makeNen("Enhancer"), 100, 0);
    ht.add_hunter(2, 2, makeNen("Enhancer"), 100, 0);

    // First duel - tie
    auto res1 = ht.squad_duel(1, 2);
    REQUIRE(res1.ans() == 0);

    // Second duel - still tie (both have exp=1, aura=100)
    auto res2 = ht.squad_duel(1, 2);
    REQUIRE(res2.ans() == 0);

    // Third duel - still tie
    auto res3 = ht.squad_duel(1, 2);
    REQUIRE(res3.ans() == 0);
}

TEST_CASE("squad_duel: Specialist advantages", "[squad_duel]") {
    Huntech ht;
    ht.add_squad(1);
    ht.add_squad(2);

    ht.add_hunter(1, 1, makeNen("Specialist"), 100, 0);
    ht.add_hunter(2, 2, makeNen("Enhancer"), 100, 0);

    auto res = ht.squad_duel(1, 2);
    REQUIRE(res.status() == StatusType::SUCCESS);
    REQUIRE(res.ans() == 2); // Specialist wins by Nen
}

// ============================================================================
// GET_HUNTER_FIGHTS_NUMBER TESTS
// ============================================================================

TEST_CASE("get_hunter_fights_number: Invalid input", "[get_hunter_fights]") {
    Huntech ht;
    REQUIRE(ht.get_hunter_fights_number(0).status() == StatusType::INVALID_INPUT);
    REQUIRE(ht.get_hunter_fights_number(-1).status() == StatusType::INVALID_INPUT);
}

TEST_CASE("get_hunter_fights_number: Failure - hunter never existed", "[get_hunter_fights]") {
    Huntech ht;
    REQUIRE(ht.get_hunter_fights_number(999).status() == StatusType::FAILURE);
}

TEST_CASE("get_hunter_fights_number: Success - initial fights", "[get_hunter_fights]") {
    Huntech ht;
    ht.add_squad(1);
    ht.add_hunter(1, 1, makeNen("Enhancer"), 100, 5);

    auto res = ht.get_hunter_fights_number(1);
    REQUIRE(res.status() == StatusType::SUCCESS);
    REQUIRE(res.ans() == 5);
}

TEST_CASE("get_hunter_fights_number: Success - after duel", "[get_hunter_fights]") {
    Huntech ht;
    ht.add_squad(1);
    ht.add_squad(2);
    ht.add_hunter(1, 1, makeNen("Enhancer"), 100, 5);
    ht.add_hunter(2, 2, makeNen("Emitter"), 50, 3);

    ht.squad_duel(1, 2);

    REQUIRE(ht.get_hunter_fights_number(1).ans() == 6);
    REQUIRE(ht.get_hunter_fights_number(2).ans() == 4);
}

TEST_CASE("get_hunter_fights_number: Success - hunter is dead", "[get_hunter_fights]") {
    Huntech ht;
    ht.add_squad(1);
    ht.add_hunter(1, 1, makeNen("Enhancer"), 100, 5);
    ht.remove_squad(1); // Hunter dies

    auto res = ht.get_hunter_fights_number(1);
    REQUIRE(res.status() == StatusType::SUCCESS);
    REQUIRE(res.ans() == 5); // Fights persist after death
}

// ============================================================================
// GET_SQUAD_EXPERIENCE TESTS
// ============================================================================

TEST_CASE("get_squad_experience: Invalid input", "[get_squad_exp]") {
    Huntech ht;
    REQUIRE(ht.get_squad_experience(0).status() == StatusType::INVALID_INPUT);
    REQUIRE(ht.get_squad_experience(-1).status() == StatusType::INVALID_INPUT);
}

TEST_CASE("get_squad_experience: Failure - squad does not exist", "[get_squad_exp]") {
    Huntech ht;
    REQUIRE(ht.get_squad_experience(999).status() == StatusType::FAILURE);
}

TEST_CASE("get_squad_experience: Success - initial experience is zero", "[get_squad_exp]") {
    Huntech ht;
    ht.add_squad(1);
    auto res = ht.get_squad_experience(1);
    REQUIRE(res.status() == StatusType::SUCCESS);
    REQUIRE(res.ans() == 0);
}

TEST_CASE("get_squad_experience: Success - after wins", "[get_squad_exp]") {
    Huntech ht;
    ht.add_squad(1);
    ht.add_squad(2);
    ht.add_hunter(1, 1, makeNen("Enhancer"), 200, 0);
    ht.add_hunter(2, 2, makeNen("Enhancer"), 100, 0);

    ht.squad_duel(1, 2); // Squad 1 wins
    REQUIRE(ht.get_squad_experience(1).ans() == 3);

    ht.squad_duel(1, 2); // Squad 1 wins again
    REQUIRE(ht.get_squad_experience(1).ans() == 6);
}

// ============================================================================
// GET_ITH_COLLECTIVE_AURA_SQUAD TESTS
// ============================================================================

TEST_CASE("get_ith_collective_aura_squad: Failure - no squads", "[get_ith_aura]") {
    Huntech ht;
    REQUIRE(ht.get_ith_collective_aura_squad(1).status() == StatusType::FAILURE);
}

TEST_CASE("get_ith_collective_aura_squad: Failure - i out of range", "[get_ith_aura]") {
    Huntech ht;
    ht.add_squad(1);
    ht.add_hunter(1, 1, makeNen("Enhancer"), 100, 0);

    REQUIRE(ht.get_ith_collective_aura_squad(0).status() == StatusType::FAILURE);
    REQUIRE(ht.get_ith_collective_aura_squad(2).status() == StatusType::FAILURE);
    REQUIRE(ht.get_ith_collective_aura_squad(-1).status() == StatusType::FAILURE);
}

TEST_CASE("get_ith_collective_aura_squad: Success - single squad", "[get_ith_aura]") {
    Huntech ht;
    ht.add_squad(1);
    ht.add_hunter(1, 1, makeNen("Enhancer"), 100, 0);

    auto res = ht.get_ith_collective_aura_squad(1);
    REQUIRE(res.status() == StatusType::SUCCESS);
    REQUIRE(res.ans() == 1);
}

TEST_CASE("get_ith_collective_aura_squad: Sorted by total aura ascending", "[get_ith_aura]") {
    Huntech ht;
    ht.add_squad(1);
    ht.add_squad(2);
    ht.add_squad(3);

    ht.add_hunter(1, 1, makeNen("Enhancer"), 300, 0); // Squad 1: 300
    ht.add_hunter(2, 2, makeNen("Enhancer"), 100, 0); // Squad 2: 100
    ht.add_hunter(3, 3, makeNen("Enhancer"), 200, 0); // Squad 3: 200

    REQUIRE(ht.get_ith_collective_aura_squad(1).ans() == 2); // 100
    REQUIRE(ht.get_ith_collective_aura_squad(2).ans() == 3); // 200
    REQUIRE(ht.get_ith_collective_aura_squad(3).ans() == 1); // 300
}

TEST_CASE("get_ith_collective_aura_squad: Tiebreaker by squadId ascending", "[get_ith_aura]") {
    Huntech ht;
    ht.add_squad(5);
    ht.add_squad(3);
    ht.add_squad(7);

    ht.add_hunter(1, 5, makeNen("Enhancer"), 100, 0);
    ht.add_hunter(2, 3, makeNen("Enhancer"), 100, 0);
    ht.add_hunter(3, 7, makeNen("Enhancer"), 100, 0);

    // All have aura=100, sorted by ID: 3, 5, 7
    REQUIRE(ht.get_ith_collective_aura_squad(1).ans() == 3);
    REQUIRE(ht.get_ith_collective_aura_squad(2).ans() == 5);
    REQUIRE(ht.get_ith_collective_aura_squad(3).ans() == 7);
}

TEST_CASE("get_ith_collective_aura_squad: Empty squads have zero aura", "[get_ith_aura]") {
    Huntech ht;
    ht.add_squad(1);
    ht.add_squad(2);

    ht.add_hunter(1, 2, makeNen("Enhancer"), 100, 0);

    // Squad 1 (empty, aura=0) < Squad 2 (aura=100)
    REQUIRE(ht.get_ith_collective_aura_squad(1).ans() == 1);
    REQUIRE(ht.get_ith_collective_aura_squad(2).ans() == 2);
}

TEST_CASE("get_ith_collective_aura_squad: Updates after adding hunters", "[get_ith_aura]") {
    Huntech ht;
    ht.add_squad(1);
    ht.add_squad(2);

    ht.add_hunter(1, 1, makeNen("Enhancer"), 100, 0);
    REQUIRE(ht.get_ith_collective_aura_squad(1).ans() == 2); // Empty
    REQUIRE(ht.get_ith_collective_aura_squad(2).ans() == 1); // 100

    ht.add_hunter(2, 2, makeNen("Enhancer"), 200, 0);
    REQUIRE(ht.get_ith_collective_aura_squad(1).ans() == 1); // 100
    REQUIRE(ht.get_ith_collective_aura_squad(2).ans() == 2); // 200
}

// ============================================================================
// GET_PARTIAL_NEN_ABILITY TESTS
// ============================================================================

TEST_CASE("get_partial_nen_ability: Invalid input", "[get_partial_nen]") {
    Huntech ht;
    REQUIRE(ht.get_partial_nen_ability(0).status() == StatusType::INVALID_INPUT);
    REQUIRE(ht.get_partial_nen_ability(-1).status() == StatusType::INVALID_INPUT);
}

TEST_CASE("get_partial_nen_ability: Failure - hunter never existed", "[get_partial_nen]") {
    Huntech ht;
    REQUIRE(ht.get_partial_nen_ability(999).status() == StatusType::FAILURE);
}
