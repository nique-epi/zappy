/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Unit tests for Direction helpers (offset, opposite, ejection code)
*/

#include <gtest/gtest.h>
#include "App/World/Player/Direction.hpp"

using zappy::world::Direction;
using zappy::world::directionOffset;
using zappy::world::ejectionCode;
using zappy::world::opposite;
using zappy::world::turnLeft;
using zappy::world::turnRight;

TEST(Direction, OffsetMovesAlongTheRightAxis) {
  /*
   * Given the four cardinal directions
   * When their movement offset is queried
   * Then North/South move on the row and East/West on the column
   */
  EXPECT_EQ(directionOffset(Direction::North).dx, 0);
  EXPECT_EQ(directionOffset(Direction::North).dy, -1);
  EXPECT_EQ(directionOffset(Direction::South).dy, 1);
  EXPECT_EQ(directionOffset(Direction::East).dx, 1);
  EXPECT_EQ(directionOffset(Direction::West).dx, -1);
}

TEST(Direction, OppositeFlipsTheAxis) {
  /*
   * Given a direction
   * When opposite is queried
   * Then it returns the direction facing the other way
   */
  EXPECT_EQ(opposite(Direction::North), Direction::South);
  EXPECT_EQ(opposite(Direction::East), Direction::West);
  EXPECT_EQ(opposite(Direction::South), Direction::North);
  EXPECT_EQ(opposite(Direction::West), Direction::East);
}

TEST(Direction, EjectionCodeIsRelativeToTheVictimOrientation) {
  /*
   * Given a drone pushed northwards
   * When the eject code is computed for each possible victim orientation
   * Then it reads as back/front/right/left in the subject's tile numbering
   */
  EXPECT_EQ(ejectionCode(Direction::North, Direction::North), 5);
  EXPECT_EQ(ejectionCode(Direction::North, Direction::South), 1);
  EXPECT_EQ(ejectionCode(Direction::North, Direction::East), 7);
  EXPECT_EQ(ejectionCode(Direction::North, Direction::West), 3);
}

TEST(Direction, EjectionCodeHandlesEveryPushDirection) {
  /*
   * Given an eastward push
   * When the eject code is computed for sampled victim orientations
   * Then a drone pushed from behind hears 5 and one pushed from its left 3
   */
  EXPECT_EQ(ejectionCode(Direction::East, Direction::East), 5);
  EXPECT_EQ(ejectionCode(Direction::East, Direction::North), 3);
  EXPECT_EQ(ejectionCode(Direction::East, Direction::West), 1);
}

TEST(Direction, TurnRightRotatesClockwise) {
  /*
   * Given each cardinal direction
   * When the drone turns right (Right command)
   * Then the orientation steps N->E->S->W->N
   */
  EXPECT_EQ(turnRight(Direction::North), Direction::East);
  EXPECT_EQ(turnRight(Direction::East), Direction::South);
  EXPECT_EQ(turnRight(Direction::South), Direction::West);
  EXPECT_EQ(turnRight(Direction::West), Direction::North);
}

TEST(Direction, TurnLeftRotatesCounterClockwise) {
  /*
   * Given each cardinal direction
   * When the drone turns left (Left command)
   * Then the orientation steps N->W->S->E->N
   */
  EXPECT_EQ(turnLeft(Direction::North), Direction::West);
  EXPECT_EQ(turnLeft(Direction::West), Direction::South);
  EXPECT_EQ(turnLeft(Direction::South), Direction::East);
  EXPECT_EQ(turnLeft(Direction::East), Direction::North);
}

TEST(Direction, TurnLeftUndoesTurnRight) {
  /*
   * Given any cardinal direction
   * When a right turn is followed by a left turn
   * Then the original orientation is restored
   */
  for (const Direction direction :
       {Direction::North, Direction::East, Direction::South, Direction::West}) {
    EXPECT_EQ(turnLeft(turnRight(direction)), direction);
  }
}
