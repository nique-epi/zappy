/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Unit tests for zappy::server::ActionQueue
*/

#include <gtest/gtest.h>
#include <string>
#include "Net/ActionQueue.hpp"

using zappy::server::ActionQueue;
using zappy::server::maxPendingActions;

TEST(ActionQueue, EmptyByDefault) {
  /*
   * Given a freshly constructed queue
   * When no command has been pushed
   * Then it reports empty and zero size
   */
  ActionQueue queue;
  EXPECT_TRUE(queue.empty());
  EXPECT_FALSE(queue.full());
  EXPECT_EQ(queue.size(), 0U);
  EXPECT_FALSE(queue.pop().has_value());
}

TEST(ActionQueue, PushIncreasesSize) {
  /*
   * Given an empty queue
   * When two lines are pushed
   * Then size reflects the count and empty is false
   */
  ActionQueue queue;
  EXPECT_TRUE(queue.push("Forward"));
  EXPECT_TRUE(queue.push("Right"));
  EXPECT_EQ(queue.size(), 2U);
  EXPECT_FALSE(queue.empty());
}

TEST(ActionQueue, PopReturnsLinesInFifoOrder) {
  /*
   * Given three lines pushed in order
   * When pop is called repeatedly
   * Then it returns them in arrival order
   */
  ActionQueue queue;
  queue.push("Forward");
  queue.push("Right");
  queue.push("Look");
  EXPECT_EQ(queue.pop().value(), "Forward");
  EXPECT_EQ(queue.pop().value(), "Right");
  EXPECT_EQ(queue.pop().value(), "Look");
  EXPECT_TRUE(queue.empty());
}

TEST(ActionQueue, RejectsBeyondMaxPendingActions) {
  /*
   * Given a queue filled to maxPendingActions
   * When one more push is attempted
   * Then it returns false and size stays capped
   */
  ActionQueue queue;
  for (std::size_t index = 0; index < maxPendingActions; index++) {
    EXPECT_TRUE(queue.push("cmd" + std::to_string(index)));
  }
  EXPECT_TRUE(queue.full());
  EXPECT_FALSE(queue.push("overflow"));
  EXPECT_EQ(queue.size(), maxPendingActions);
}

TEST(ActionQueue, OverflowDoesNotReorder) {
  /*
   * Given a saturated queue
   * When the dropped line would have been "overflow"
   * Then pop still returns the originally-pushed FIFO order
   */
  ActionQueue queue;
  for (std::size_t index = 0; index < maxPendingActions; index++) {
    queue.push("cmd" + std::to_string(index));
  }
  queue.push("overflow");
  EXPECT_EQ(queue.pop().value(), "cmd0");
  EXPECT_EQ(queue.pop().value(), "cmd1");
}

TEST(ActionQueue, ClearEmptiesQueue) {
  /*
   * Given a queue with entries
   * When clear is called
   * Then it becomes empty again
   */
  ActionQueue queue;
  queue.push("a");
  queue.push("b");
  queue.clear();
  EXPECT_TRUE(queue.empty());
  EXPECT_EQ(queue.size(), 0U);
}

TEST(ActionQueue, ReopensAfterPopBelowCap) {
  /*
   * Given a saturated queue
   * When one entry is popped
   * Then a new push is accepted again
   */
  ActionQueue queue;
  for (std::size_t index = 0; index < maxPendingActions; index++) {
    queue.push("cmd" + std::to_string(index));
  }
  queue.pop();
  EXPECT_FALSE(queue.full());
  EXPECT_TRUE(queue.push("fresh"));
  EXPECT_EQ(queue.size(), maxPendingActions);
}
