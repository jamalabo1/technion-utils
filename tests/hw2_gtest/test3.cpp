//
// Created by jamal on 15/01/2026.
//
// test_huntech_gtest.cpp
// GTest suite for Wet2 (Huntech).
// NOTE: output_t is NOT assignable (has const fields). So never do:
//   output_t<int> out; out = ht.get_squad_experience(...);   // ❌
// Instead always do:
//   auto out = ht.get_squad_experience(...);                // ✅ (copy construction)

#include <gtest/gtest.h>

#include <sstream>
#include <string>
#include <vector>

#include "Huntech26a2.h"
#include "wet2util.h"

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

// For NenAbility: operator== is matchup-based (non-standard). Compare printed vectors instead.
static void expectOutputNen(output_t<NenAbility> out, StatusType expectedStatus, const NenAbility& expectedAns) {
    EXPECT_EQ(out.status(), expectedStatus);
    if (expectedStatus == StatusType::SUCCESS) {
        EXPECT_EQ(nenToString(out.ans()), nenToString(expectedAns));
    }
}

static void expectStatus(StatusType got, StatusType expected) {
    EXPECT_EQ(got, expected);
}

/* ---------------------------
 *  add_squad / remove_squad
 * --------------------------- */

TEST(Huntech_AddRemoveSquad, AddSquad_InvalidInput) {
    Huntech ht;
    expectStatus(ht.add_squad(0), StatusType::INVALID_INPUT);
    expectStatus(ht.add_squad(-7), StatusType::INVALID_INPUT);
}

TEST(Huntech_AddRemoveSquad, AddSquad_DuplicateActiveFails) {
    Huntech ht;
    expectStatus(ht.add_squad(1), StatusType::SUCCESS);
    expectStatus(ht.add_squad(1), StatusType::FAILURE);
}

TEST(Huntech_AddRemoveSquad, RemoveSquad_InvalidAndMissing) {
    Huntech ht;
    expectStatus(ht.remove_squad(0), StatusType::INVALID_INPUT);
    expectStatus(ht.remove_squad(-1), StatusType::INVALID_INPUT);
    expectStatus(ht.remove_squad(10), StatusType::FAILURE); // not in system
}

TEST(Huntech_AddRemoveSquad, RemoveSquad_AllowsReAddSameId) {
    Huntech ht;
    expectStatus(ht.add_squad(5), StatusType::SUCCESS);

    // add 1 hunter so squad exists + non-empty, then remove the squad (kills its hunters)
    expectStatus(ht.add_hunter(100, 5, nen("Enhancer"), 10, 2), StatusType::SUCCESS);

    expectStatus(ht.remove_squad(5), StatusType::SUCCESS);

    // squad should be gone: queries on squad fail
    {
        auto outExp = ht.get_squad_experience(5);
        expectOutput<int>(outExp, StatusType::FAILURE, 0);
    }

    // can add same squad id again
    expectStatus(ht.add_squad(5), StatusType::SUCCESS);

    // new squad starts with 0 experience
    {
        auto outExp = ht.get_squad_experience(5);
        expectOutput<int>(outExp, StatusType::SUCCESS, 0);
    }
}

/* ---------------------------
 *  add_hunter validations
 * --------------------------- */

TEST(Huntech_AddHunter, InvalidInput_AllFields) {
    Huntech ht;
    expectStatus(ht.add_squad(1), StatusType::SUCCESS);

    // hunterId
    expectStatus(ht.add_hunter(0, 1, nen("Enhancer"), 0, 0), StatusType::INVALID_INPUT);
    expectStatus(ht.add_hunter(-1, 1, nen("Enhancer"), 0, 0), StatusType::INVALID_INPUT);

    // squadId
    expectStatus(ht.add_hunter(1, 0, nen("Enhancer"), 0, 0), StatusType::INVALID_INPUT);
    expectStatus(ht.add_hunter(1, -2, nen("Enhancer"), 0, 0), StatusType::INVALID_INPUT);

    // aura < 0
    expectStatus(ht.add_hunter(1, 1, nen("Enhancer"), -1, 0), StatusType::INVALID_INPUT);

    // fightsHad < 0
    expectStatus(ht.add_hunter(1, 1, nen("Enhancer"), 0, -5), StatusType::INVALID_INPUT);

    // invalid nen ability (wet2util.h marks invalid strings as invalid object)
    NenAbility bad("NotAType");
    expectStatus(ht.add_hunter(1, 1, bad, 0, 0), StatusType::INVALID_INPUT);
}

TEST(Huntech_AddHunter, Failure_SquadMissingOrHunterAlreadyExistsOrEverExisted) {
    Huntech ht;
    expectStatus(ht.add_squad(10), StatusType::SUCCESS);

    // squad missing
    expectStatus(ht.add_hunter(7, 999, nen("Enhancer"), 0, 0), StatusType::FAILURE);

    // already exists
    expectStatus(ht.add_hunter(7, 10, nen("Enhancer"), 0, 0), StatusType::SUCCESS);
    expectStatus(ht.add_hunter(7, 10, nen("Emitter"), 3, 1), StatusType::FAILURE);

    // "ever existed": if hunter dies (via remove_squad), cannot be re-added
    expectStatus(ht.remove_squad(10), StatusType::SUCCESS); // kills hunter 7
    expectStatus(ht.add_squad(10), StatusType::SUCCESS);
    expectStatus(ht.add_hunter(7, 10, nen("Enhancer"), 0, 0), StatusType::FAILURE);
}

/* ---------------------------
 *  get_hunter_fights_number
 * --------------------------- */

TEST(Huntech_GetHunterFights, InvalidAndNeverExisted) {
    Huntech ht;
    auto out1 = ht.get_hunter_fights_number(0);
    expectOutput<int>(out1, StatusType::INVALID_INPUT, 0);

    auto out2 = ht.get_hunter_fights_number(-3);
    expectOutput<int>(out2, StatusType::INVALID_INPUT, 0);

    auto out3 = ht.get_hunter_fights_number(123);
    expectOutput<int>(out3, StatusType::FAILURE, 0);
}

TEST(Huntech_GetHunterFights, IncludesInitialFightsAndIncrementsOnDuelEvenAfterDeath) {
    Huntech ht;
    expectStatus(ht.add_squad(1), StatusType::SUCCESS);
    expectStatus(ht.add_squad(2), StatusType::SUCCESS);

    // hunter 10 starts with fightsHad = 5
    expectStatus(ht.add_hunter(10, 1, nen("Enhancer"), 0, 5), StatusType::SUCCESS);
    expectStatus(ht.add_hunter(20, 2, nen("Emitter"),   0, 0), StatusType::SUCCESS);

    {
        auto out = ht.get_hunter_fights_number(10);
        expectOutput<int>(out, StatusType::SUCCESS, 5);
    }

    // duel once => each hunter +1
    {
        auto outDuel = ht.squad_duel(1, 2);
        EXPECT_EQ(outDuel.status(), StatusType::SUCCESS);
    }

    {
        auto out = ht.get_hunter_fights_number(10);
        expectOutput<int>(out, StatusType::SUCCESS, 6);
    }

    // kill squad 1 (kills hunter 10)
    expectStatus(ht.remove_squad(1), StatusType::SUCCESS);

    // fights number still queryable even if dead
    {
        auto out = ht.get_hunter_fights_number(10);
        expectOutput<int>(out, StatusType::SUCCESS, 6);
    }
}

/* ---------------------------
 *  get_squad_experience
 * --------------------------- */

TEST(Huntech_GetSquadExperience, InvalidMissingAndEmptySquadOk) {
    Huntech ht;

    auto outBad = ht.get_squad_experience(0);
    expectOutput<int>(outBad, StatusType::INVALID_INPUT, 0);

    auto outMissing = ht.get_squad_experience(7);
    expectOutput<int>(outMissing, StatusType::FAILURE, 0);

    expectStatus(ht.add_squad(7), StatusType::SUCCESS);
    auto outEmpty = ht.get_squad_experience(7);
    expectOutput<int>(outEmpty, StatusType::SUCCESS, 0);
}

/* ---------------------------
 *  get_ith_collective_aura_squad
 * --------------------------- */

TEST(Huntech_GetIthAuraSquad, FailureWhenNoSquadsOrIndexOutOfRange) {
    Huntech ht;

    auto outNone = ht.get_ith_collective_aura_squad(1);
    expectOutput<int>(outNone, StatusType::FAILURE, 0);

    expectStatus(ht.add_squad(1), StatusType::SUCCESS);
    expectStatus(ht.add_squad(2), StatusType::SUCCESS);

    auto out0 = ht.get_ith_collective_aura_squad(0);
    expectOutput<int>(out0, StatusType::FAILURE, 0);

    auto outNeg = ht.get_ith_collective_aura_squad(-1);
    expectOutput<int>(outNeg, StatusType::FAILURE, 0);

    auto outTooBig = ht.get_ith_collective_aura_squad(3);
    expectOutput<int>(outTooBig, StatusType::FAILURE, 0);
}

TEST(Huntech_GetIthAuraSquad, OrdersByTotalAuraThenBySquadIdAscending) {
    Huntech ht;
    expectStatus(ht.add_squad(10), StatusType::SUCCESS);
    expectStatus(ht.add_squad(5),  StatusType::SUCCESS);
    expectStatus(ht.add_squad(7),  StatusType::SUCCESS);

    // All empty => totalAura=0 tie-break by squadId asc => 5,7,10
    expectOutput<int>(ht.get_ith_collective_aura_squad(1), StatusType::SUCCESS, 5);
    expectOutput<int>(ht.get_ith_collective_aura_squad(2), StatusType::SUCCESS, 7);
    expectOutput<int>(ht.get_ith_collective_aura_squad(3), StatusType::SUCCESS, 10);

    // Give squad 10 aura 3, squad 7 aura 2, squad 5 stays 0
    expectStatus(ht.add_hunter(1, 10, nen("Enhancer"), 3, 0), StatusType::SUCCESS);
    expectStatus(ht.add_hunter(2, 7,  nen("Enhancer"), 2, 0), StatusType::SUCCESS);

    // Now order by aura: squad5(0), squad7(2), squad10(3)
    expectOutput<int>(ht.get_ith_collective_aura_squad(1), StatusType::SUCCESS, 5);
    expectOutput<int>(ht.get_ith_collective_aura_squad(2), StatusType::SUCCESS, 7);
    expectOutput<int>(ht.get_ith_collective_aura_squad(3), StatusType::SUCCESS, 10);
}

/* ---------------------------
 *  get_partial_nen_ability
 * --------------------------- */

TEST(Huntech_GetPartialNen, InvalidNeverExistedDead) {
    Huntech ht;

    auto outBad = ht.get_partial_nen_ability(0);
    EXPECT_EQ(outBad.status(), StatusType::INVALID_INPUT);

    auto outNever = ht.get_partial_nen_ability(123);
    EXPECT_EQ(outNever.status(), StatusType::FAILURE);

    // Create and then kill
    expectStatus(ht.add_squad(1), StatusType::SUCCESS);
    expectStatus(ht.add_hunter(11, 1, nen("Enhancer"), 0, 0), StatusType::SUCCESS);
    expectStatus(ht.remove_squad(1), StatusType::SUCCESS);

    auto outDead = ht.get_partial_nen_ability(11);
    EXPECT_EQ(outDead.status(), StatusType::FAILURE);
}

TEST(Huntech_GetPartialNen, PrefixByJoinOrderWithinSquad) {
    Huntech ht;
    expectStatus(ht.add_squad(1), StatusType::SUCCESS);

    // join order: 10 (Enhancer), 20 (Emitter), 30 (Enhancer)
    expectStatus(ht.add_hunter(10, 1, nen("Enhancer"), 0, 0), StatusType::SUCCESS);
    expectStatus(ht.add_hunter(20, 1, nen("Emitter"),   0, 0), StatusType::SUCCESS);
    expectStatus(ht.add_hunter(30, 1, nen("Enhancer"), 0, 0), StatusType::SUCCESS);

    NenAbility e = nen("Enhancer");
    NenAbility em = nen("Emitter");

    // hunter 10 prefix: Enhancer
    expectOutputNen(ht.get_partial_nen_ability(10), StatusType::SUCCESS, e);

    // hunter 20 prefix: Enhancer + Emitter
    expectOutputNen(ht.get_partial_nen_ability(20), StatusType::SUCCESS, (e + em));

    // hunter 30 prefix: Enhancer + Emitter + Enhancer
    expectOutputNen(ht.get_partial_nen_ability(30), StatusType::SUCCESS, (e + em + e));
}

/* ---------------------------
 *  squad_duel
 * --------------------------- */

TEST(Huntech_SquadDuel, InvalidInputAndFailures) {
    Huntech ht;
    expectStatus(ht.add_squad(1), StatusType::SUCCESS);
    expectStatus(ht.add_squad(2), StatusType::SUCCESS);

    // invalid: non-positive or equal ids
    EXPECT_EQ(ht.squad_duel(0, 2).status(), StatusType::INVALID_INPUT);
    EXPECT_EQ(ht.squad_duel(1, 0).status(), StatusType::INVALID_INPUT);
    EXPECT_EQ(ht.squad_duel(-1, 2).status(), StatusType::INVALID_INPUT);
    EXPECT_EQ(ht.squad_duel(1, 1).status(), StatusType::INVALID_INPUT);

    // failure: empty squad
    EXPECT_EQ(ht.squad_duel(1, 2).status(), StatusType::FAILURE);

    // failure: missing squad
    EXPECT_EQ(ht.squad_duel(1, 999).status(), StatusType::FAILURE);
}

TEST(Huntech_SquadDuel, WinByEffectiveAura_UpdatesExpAndHunterFights) {
    Huntech ht;
    expectStatus(ht.add_squad(1), StatusType::SUCCESS);
    expectStatus(ht.add_squad(2), StatusType::SUCCESS);

    // totalAura: squad1=10, squad2=5 (exp both 0)
    expectStatus(ht.add_hunter(10, 1, nen("Enhancer"), 10, 0), StatusType::SUCCESS);
    expectStatus(ht.add_hunter(20, 2, nen("Enhancer"), 5,  0), StatusType::SUCCESS);

    auto outDuel = ht.squad_duel(1, 2);
    expectOutput<int>(outDuel, StatusType::SUCCESS, 1); // squad1 wins by effective aura

    // winner exp += 3
    expectOutput<int>(ht.get_squad_experience(1), StatusType::SUCCESS, 3);
    expectOutput<int>(ht.get_squad_experience(2), StatusType::SUCCESS, 0);

    // each hunter fights +1
    expectOutput<int>(ht.get_hunter_fights_number(10), StatusType::SUCCESS, 1);
    expectOutput<int>(ht.get_hunter_fights_number(20), StatusType::SUCCESS, 1);
}

TEST(Huntech_SquadDuel, Tie_UpdatesBothExpAndAllHunterFights) {
    Huntech ht;
    expectStatus(ht.add_squad(1), StatusType::SUCCESS);
    expectStatus(ht.add_squad(2), StatusType::SUCCESS);

    // equal aura and equal nen => tie
    expectStatus(ht.add_hunter(10, 1, nen("Enhancer"), 0, 0), StatusType::SUCCESS);
    expectStatus(ht.add_hunter(20, 2, nen("Enhancer"), 0, 0), StatusType::SUCCESS);

    auto outDuel = ht.squad_duel(1, 2);
    expectOutput<int>(outDuel, StatusType::SUCCESS, 0);

    // tie => exp +1 each
    expectOutput<int>(ht.get_squad_experience(1), StatusType::SUCCESS, 1);
    expectOutput<int>(ht.get_squad_experience(2), StatusType::SUCCESS, 1);

    // fights +1 each
    expectOutput<int>(ht.get_hunter_fights_number(10), StatusType::SUCCESS, 1);
    expectOutput<int>(ht.get_hunter_fights_number(20), StatusType::SUCCESS, 1);
}

TEST(Huntech_SquadDuel, WinByNen_WhenEffectiveAuraEqual) {
    Huntech ht;
    expectStatus(ht.add_squad(1), StatusType::SUCCESS);
    expectStatus(ht.add_squad(2), StatusType::SUCCESS);

    // effective aura equal: exp=0, totalAura=0
    // Nen matchup matrix in wet2util.h implies Enhancer beats Emitter (matrix[Enhancer][Emitter] = +1).
    expectStatus(ht.add_hunter(10, 1, nen("Enhancer"), 0, 0), StatusType::SUCCESS);
    expectStatus(ht.add_hunter(20, 2, nen("Emitter"),   0, 0), StatusType::SUCCESS);

    auto outDuel = ht.squad_duel(1, 2);
    expectOutput<int>(outDuel, StatusType::SUCCESS, 2); // squad1 wins by nen advantage

    // winner exp += 3
    expectOutput<int>(ht.get_squad_experience(1), StatusType::SUCCESS, 3);
    expectOutput<int>(ht.get_squad_experience(2), StatusType::SUCCESS, 0);

    // fights +1 each
    expectOutput<int>(ht.get_hunter_fights_number(10), StatusType::SUCCESS, 1);
    expectOutput<int>(ht.get_hunter_fights_number(20), StatusType::SUCCESS, 1);
}

/* ---------------------------
 *  force_join
 * --------------------------- */

TEST(Huntech_ForceJoin, InvalidInput) {
    Huntech ht;
    EXPECT_EQ(ht.force_join(0, 1), StatusType::INVALID_INPUT);
    EXPECT_EQ(ht.force_join(1, 0), StatusType::INVALID_INPUT);
    EXPECT_EQ(ht.force_join(-1, 2), StatusType::INVALID_INPUT);
    EXPECT_EQ(ht.force_join(3, 3), StatusType::INVALID_INPUT);
}

TEST(Huntech_ForceJoin, Failure_SquadsMissingOrForcingEmptyOrInequalityNotStrict) {
    Huntech ht;
    expectStatus(ht.add_squad(1), StatusType::SUCCESS);
    expectStatus(ht.add_squad(2), StatusType::SUCCESS);

    // forcing empty => cannot force
    EXPECT_EQ(ht.force_join(1, 2), StatusType::FAILURE);

    // missing squad
    EXPECT_EQ(ht.force_join(1, 999), StatusType::FAILURE);
    EXPECT_EQ(ht.force_join(999, 1), StatusType::FAILURE);

    // Make both non-empty but equal "power" => not strict => failure
    // power = exp + totalAura + effectiveNenAbility (L2 norm squared of NenAbility vector)
    // Each has: exp=0, aura=0, effective=1 => equal
    expectStatus(ht.add_hunter(10, 1, nen("Enhancer"), 0, 0), StatusType::SUCCESS);
    expectStatus(ht.add_hunter(20, 2, nen("Enhancer"), 0, 0), StatusType::SUCCESS);
    EXPECT_EQ(ht.force_join(1, 2), StatusType::FAILURE);
}

TEST(Huntech_ForceJoin, Success_MergesExperiencesKeepsForcingIdRemovesForced) {
    Huntech ht;
    expectStatus(ht.add_squad(1), StatusType::SUCCESS);
    expectStatus(ht.add_squad(2), StatusType::SUCCESS);

    // Give squad1 strictly larger power.
    // squad1: exp 0, totalAura 5, effectiveNen 1 => power 6
    // squad2: exp 0, totalAura 0, effectiveNen 1 => power 1
    expectStatus(ht.add_hunter(10, 1, nen("Enhancer"), 5, 2), StatusType::SUCCESS);
    expectStatus(ht.add_hunter(20, 2, nen("Emitter"),  0, 7), StatusType::SUCCESS);

    // Also add second hunter to forced squad to test chronological merge order
    expectStatus(ht.add_hunter(30, 2, nen("Conjurer"),  0, 0), StatusType::SUCCESS);

    // Before join: both squads exist
    expectOutput<int>(ht.get_squad_experience(1), StatusType::SUCCESS, 0);
    expectOutput<int>(ht.get_squad_experience(2), StatusType::SUCCESS, 0);

    EXPECT_EQ(ht.force_join(1, 2), StatusType::SUCCESS);

    // Forced squad should be removed from system
    expectOutput<int>(ht.get_squad_experience(2), StatusType::FAILURE, 0);

    // Forcing squad id remains; experience = sum of both (0+0)
    expectOutput<int>(ht.get_squad_experience(1), StatusType::SUCCESS, 0);

    // Fights counts are preserved (no +1 on join)
    expectOutput<int>(ht.get_hunter_fights_number(10), StatusType::SUCCESS, 2);
    expectOutput<int>(ht.get_hunter_fights_number(20), StatusType::SUCCESS, 7);
    expectOutput<int>(ht.get_hunter_fights_number(30), StatusType::SUCCESS, 0);

    // Chronological order after join: all forcing hunters first, then all forced hunters.
    // So for hunter 20 (first in forced squad), prefix should include squad1's total nen + hunter20's nen.
    NenAbility enh = nen("Enhancer"); // hunter 10
    NenAbility emi = nen("Emitter");  // hunter 20
    NenAbility con = nen("Conjurer"); // hunter 30

    // hunter 10 was first in forcing => prefix is just Enhancer
    expectOutputNen(ht.get_partial_nen_ability(10), StatusType::SUCCESS, enh);

    // hunter 20 should see Enhancer + Emitter
    expectOutputNen(ht.get_partial_nen_ability(20), StatusType::SUCCESS, (enh + emi));

    // hunter 30 should see Enhancer + Emitter + Conjurer
    expectOutputNen(ht.get_partial_nen_ability(30), StatusType::SUCCESS, (enh + emi + con));
}

TEST(Huntech_ForceJoin, CanForceEmptySquad) {
    Huntech ht;
    expectStatus(ht.add_squad(1), StatusType::SUCCESS);
    expectStatus(ht.add_squad(2), StatusType::SUCCESS);

    // forcing non-empty, forced empty => should succeed
    expectStatus(ht.add_hunter(10, 1, nen("Enhancer"), 1, 0), StatusType::SUCCESS);

    EXPECT_EQ(ht.force_join(1, 2), StatusType::SUCCESS);

    // forced removed
    expectOutput<int>(ht.get_squad_experience(2), StatusType::FAILURE, 0);
    // forcing remains
    expectOutput<int>(ht.get_squad_experience(1), StatusType::SUCCESS, 0);
}

/* ---------------------------
 *  Integration / regression
 * --------------------------- */

TEST(Huntech_Integration, DuelThenForceJoinThenOrdering) {
    Huntech ht;
    expectStatus(ht.add_squad(1), StatusType::SUCCESS);
    expectStatus(ht.add_squad(2), StatusType::SUCCESS);
    expectStatus(ht.add_squad(3), StatusType::SUCCESS);

    // squad1 aura 2, squad2 aura 1, squad3 aura 0
    expectStatus(ht.add_hunter(11, 1, nen("Enhancer"), 2, 0), StatusType::SUCCESS);
    expectStatus(ht.add_hunter(22, 2, nen("Emitter"),   1, 0), StatusType::SUCCESS);

    // Order initially: squad3(0), squad2(1), squad1(2)
    expectOutput<int>(ht.get_ith_collective_aura_squad(1), StatusType::SUCCESS, 3);
    expectOutput<int>(ht.get_ith_collective_aura_squad(2), StatusType::SUCCESS, 2);
    expectOutput<int>(ht.get_ith_collective_aura_squad(3), StatusType::SUCCESS, 1);

    // Duel 1 vs 2: effective aura differs (2 vs 1) => squad1 wins by aura, exp1=3
    expectOutput<int>(ht.squad_duel(1, 2), StatusType::SUCCESS, 1);
    expectOutput<int>(ht.get_squad_experience(1), StatusType::SUCCESS, 3);

    // Ordering by aura is unchanged by exp
    expectOutput<int>(ht.get_ith_collective_aura_squad(1), StatusType::SUCCESS, 3);
    expectOutput<int>(ht.get_ith_collective_aura_squad(2), StatusType::SUCCESS, 2);
    expectOutput<int>(ht.get_ith_collective_aura_squad(3), StatusType::SUCCESS, 1);

    // Now force_join 1 forces 2 (should be strict because exp1=3 helps)
    EXPECT_EQ(ht.force_join(1, 2), StatusType::SUCCESS);

    // squads now: 1 and 3 only. Total aura of squad1 becomes 3.
    // Order: squad3(0), squad1(3)
    expectOutput<int>(ht.get_ith_collective_aura_squad(1), StatusType::SUCCESS, 3);
    expectOutput<int>(ht.get_ith_collective_aura_squad(2), StatusType::SUCCESS, 1);

    // forced squad removed
    EXPECT_EQ(ht.get_squad_experience(2).status(), StatusType::FAILURE);
}

