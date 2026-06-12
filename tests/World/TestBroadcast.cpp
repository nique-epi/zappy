/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Unit tests for the broadcastSoundDirection world helper
*/

#include <gtest/gtest.h>
#include "App/World/Broadcast.hpp"
#include "App/World/Player/Direction.hpp"

using zappy::world::broadcastSoundDirection;
using zappy::world::Direction;

namespace {

int soundFrom(int emitterX, int emitterY, int receiverX, int receiverY,
              Direction facing) {
  return broadcastSoundDirection(emitterX, emitterY, receiverX, receiverY,
                                 facing, 7, 7);
}

}  // namespace

TEST(Broadcast, SameTileIsZero) {
  // Given an emitter standing on the receiver's own tile
  // When the sound direction is computed
  // Then the receiver hears it from tile 0
  EXPECT_EQ(soundFrom(3, 3, 3, 3, Direction::North), 0);
  EXPECT_EQ(soundFrom(3, 3, 3, 3, Direction::East), 0);
}

TEST(Broadcast, CardinalTilesForAnEastFacingReceiver) {
  // Given a receiver on (3, 3) facing East
  // When the emitter sits on each adjacent cardinal tile
  // Then front is 1, left (North) is 3, back (West) is 5, right (South) is 7
  EXPECT_EQ(soundFrom(4, 3, 3, 3, Direction::East), 1);
  EXPECT_EQ(soundFrom(3, 2, 3, 3, Direction::East), 3);
  EXPECT_EQ(soundFrom(2, 3, 3, 3, Direction::East), 5);
  EXPECT_EQ(soundFrom(3, 4, 3, 3, Direction::East), 7);
}

TEST(Broadcast, DiagonalTilesForAnEastFacingReceiver) {
  // Given a receiver on (3, 3) facing East
  // When the emitter sits on each adjacent diagonal tile
  // Then the counter-clockwise numbering yields 2, 4, 6, 8
  EXPECT_EQ(soundFrom(4, 2, 3, 3, Direction::East), 2);
  EXPECT_EQ(soundFrom(2, 2, 3, 3, Direction::East), 4);
  EXPECT_EQ(soundFrom(2, 4, 3, 3, Direction::East), 6);
  EXPECT_EQ(soundFrom(4, 4, 3, 3, Direction::East), 8);
}

TEST(Broadcast, NumberingRotatesWithANorthFacingReceiver) {
  // Given a receiver on (3, 3) facing North
  // When the emitter sits on each adjacent cardinal tile
  // Then front (North) is 1, right (East) is 7, left (West) is 3, back is 5
  EXPECT_EQ(soundFrom(3, 2, 3, 3, Direction::North), 1);
  EXPECT_EQ(soundFrom(4, 3, 3, 3, Direction::North), 7);
  EXPECT_EQ(soundFrom(2, 3, 3, 3, Direction::North), 3);
  EXPECT_EQ(soundFrom(3, 4, 3, 3, Direction::North), 5);
}

TEST(Broadcast, FollowsTheShortestPathAcrossTheToroidalEdge) {
  // Given a 5x5 world with a receiver on (0, 0) facing West
  // When the emitter is on the opposite column (4, 0)
  // Then the sound wraps and arrives from the front tile (1)
  EXPECT_EQ(broadcastSoundDirection(4, 0, 0, 0, Direction::West, 5, 5), 1);
}

TEST(Broadcast, WrapsOnTheVerticalEdgeToo) {
  // Given a 5x5 world with a receiver on (0, 0) facing North
  // When the emitter is on the opposite row (0, 4)
  // Then the sound wraps and arrives from the front tile (1)
  EXPECT_EQ(broadcastSoundDirection(0, 4, 0, 0, Direction::North, 5, 5), 1);
}

TEST(Broadcast, FarEmitterSnapsToTheDiagonalCorner) {
  // Given a 9x9 world with a receiver on (4, 4) facing East
  // When the emitter is three tiles ahead and three tiles to the left
  // Then the bearing snaps to the front-left diagonal tile (2)
  EXPECT_EQ(broadcastSoundDirection(7, 1, 4, 4, Direction::East, 9, 9), 2);
}
