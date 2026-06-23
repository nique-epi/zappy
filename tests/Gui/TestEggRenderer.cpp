/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Unit tests for egg data consumed by EggRenderer
*/

#include <gtest/gtest.h>
#include <string>
#include "Network/Parsing/MessageParser.hpp"
#include "World/WorldState.hpp"

TEST(EggRenderer, NoEggsInitially) {
  zappy::gui::WorldState world;

  EXPECT_TRUE(world.eggs.empty());
}

TEST(EggRenderer, MultipleEggsStoredInOrder) {
  zappy::gui::WorldState world;
  zappy::gui::MessageParser parser(world);

  parser.parseLine("enw #1 #0 2 4");
  parser.parseLine("enw #2 #0 7 1");

  ASSERT_EQ(world.eggs.size(), 2U);
  EXPECT_EQ(world.eggs[0].id, 1);
  EXPECT_EQ(world.eggs[1].id, 2);
}

TEST(EggRenderer, EggCoordinatesMatchServerValues) {
  zappy::gui::WorldState world;
  zappy::gui::MessageParser parser(world);

  parser.parseLine("enw #7 #3 6 9");

  ASSERT_EQ(world.eggs.size(), 1U);
  EXPECT_EQ(world.eggs[0].x, 6);
  EXPECT_EQ(world.eggs[0].y, 9);
}

TEST(EggRenderer, EggsAtSameTileCoexist) {
  zappy::gui::WorldState world;
  zappy::gui::MessageParser parser(world);

  parser.parseLine("enw #3 #1 2 2");
  parser.parseLine("enw #4 #2 2 2");

  ASSERT_EQ(world.eggs.size(), 2U);
  EXPECT_EQ(world.eggs[0].x, 2);
  EXPECT_EQ(world.eggs[0].y, 2);
  EXPECT_EQ(world.eggs[1].x, 2);
  EXPECT_EQ(world.eggs[1].y, 2);
}

TEST(EggRenderer, EdiOnlyRemovesTargetEgg) {
  zappy::gui::WorldState world;
  zappy::gui::MessageParser parser(world);

  parser.parseLine("enw #10 #1 0 0");
  parser.parseLine("enw #11 #2 3 4");
  parser.parseLine("edi #10");

  ASSERT_EQ(world.eggs.size(), 1U);
  EXPECT_EQ(world.eggs[0].id, 11);
}

TEST(EggRenderer, AllEggsRemovedAfterSequentialHatch) {
  zappy::gui::WorldState world;
  zappy::gui::MessageParser parser(world);

  parser.parseLine("enw #1 #0 0 0");
  parser.parseLine("enw #2 #0 1 1");
  parser.parseLine("enw #3 #0 2 2");
  parser.parseLine("ebo #1");
  parser.parseLine("ebo #2");
  parser.parseLine("ebo #3");

  EXPECT_TRUE(world.eggs.empty());
}

TEST(EggRenderer, MixedEboEdiRemovalsLeaveRemainingEggs) {
  zappy::gui::WorldState world;
  zappy::gui::MessageParser parser(world);

  parser.parseLine("enw #1 #0 0 0");
  parser.parseLine("enw #2 #0 1 1");
  parser.parseLine("enw #3 #0 2 2");
  parser.parseLine("ebo #2");
  parser.parseLine("edi #1");

  ASSERT_EQ(world.eggs.size(), 1U);
  EXPECT_EQ(world.eggs[0].id, 3);
}
