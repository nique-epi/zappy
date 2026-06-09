/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Unit tests for zappy::server::costOf
*/

#include <gtest/gtest.h>
#include "App/ActionCosts.hpp"

using zappy::server::costOf;
using zappy::server::defaultActionCost;

TEST(ActionCosts, MovementOpcodesCostSeven) {
  /*
   * Given the subject-mandated cost of 7 for movement and orientation
   * When costOf is queried for Forward, Right, Left
   * Then each returns 7
   */
  EXPECT_EQ(costOf("Forward"), 7);
  EXPECT_EQ(costOf("Right"), 7);
  EXPECT_EQ(costOf("Left"), 7);
}

TEST(ActionCosts, InventoryCostsOne) {
  /*
   * Given the subject mandate that Inventory costs 1 time unit
   * When costOf is queried for Inventory
   * Then it returns 1
   */
  EXPECT_EQ(costOf("Inventory"), 1);
}

TEST(ActionCosts, ConnectNbrIsFree) {
  /*
   * Given Connect_nbr costs zero per the subject
   * When costOf is queried for Connect_nbr
   * Then it returns 0
   */
  EXPECT_EQ(costOf("Connect_nbr"), 0);
}

TEST(ActionCosts, ForkCostsFortyTwo) {
  /*
   * Given Fork costs 42 time units per the subject
   * When costOf is queried for Fork
   * Then it returns 42
   */
  EXPECT_EQ(costOf("Fork"), 42);
}

TEST(ActionCosts, IncantationCostsThreeHundred) {
  /*
   * Given Incantation costs 300 time units per the subject
   * When costOf is queried for Incantation
   * Then it returns 300
   */
  EXPECT_EQ(costOf("Incantation"), 300);
}

TEST(ActionCosts, ResourceOpcodesCostSeven) {
  /*
   * Given Take, Set, Eject, Broadcast, Look all cost 7
   * When costOf is queried for each
   * Then it returns 7
   */
  EXPECT_EQ(costOf("Take"), 7);
  EXPECT_EQ(costOf("Set"), 7);
  EXPECT_EQ(costOf("Eject"), 7);
  EXPECT_EQ(costOf("Broadcast"), 7);
  EXPECT_EQ(costOf("Look"), 7);
}

TEST(ActionCosts, UnknownOpcodeFallsBackToDefault) {
  /*
   * Given an opcode that is not in the table
   * When costOf is queried
   * Then it returns the fallback defaultActionCost
   */
  EXPECT_EQ(costOf("garbage"), defaultActionCost);
  EXPECT_EQ(costOf(""), defaultActionCost);
}

TEST(ActionCosts, OpcodeMatchIsCaseSensitive) {
  /*
   * Given opcodes are case-sensitive in the Zappy protocol
   * When costOf is queried with a lowercased form
   * Then it falls back to the default cost
   */
  EXPECT_EQ(costOf("forward"), defaultActionCost);
}
