/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Unit tests for zappy::rpc::buildWireLine token encoding
*/

#include <gtest/gtest.h>
#include <stdexcept>
#include <string>
#include <vector>
#include "Rpc/Message/WireFormat.hpp"

using zappy::rpc::buildWireLine;

TEST(WireFormat, OpcodeOnlyHasNoTrailingSpace) {
  EXPECT_EQ(buildWireLine("suc", {}), "suc");
}

TEST(WireFormat, PlainTokensAreSpaceSeparated) {
  EXPECT_EQ(buildWireLine("pnw", {"3", "10"}), "pnw 3 10");
}

TEST(WireFormat, EmptyTokenIsQuoted) {
  EXPECT_EQ(buildWireLine("msg", {""}), "msg \"\"");
}

TEST(WireFormat, TokenWithSpaceIsQuoted) {
  EXPECT_EQ(buildWireLine("bct", {"hello world"}), "bct \"hello world\"");
}

TEST(WireFormat, InnerQuoteIsEscaped) {
  EXPECT_EQ(buildWireLine("say", {"a\"b"}), "say \"a\\\"b\"");
}

TEST(WireFormat, MixedPlainAndQuotedTokens) {
  EXPECT_EQ(buildWireLine("op", {"plain", "needs space"}),
            "op plain \"needs space\"");
}

TEST(WireFormat, NewlineInArgumentIsRejected) {
  EXPECT_THROW(buildWireLine("smg", {"line1\nline2"}), std::invalid_argument);
}

TEST(WireFormat, CarriageReturnInArgumentIsRejected) {
  EXPECT_THROW(buildWireLine("smg", {"line\rinjected"}), std::invalid_argument);
}

TEST(WireFormat, LineBreakInOpcodeIsRejected) {
  EXPECT_THROW(buildWireLine("bad\nop", {}), std::invalid_argument);
}
