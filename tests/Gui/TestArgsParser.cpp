/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Functional tests for the GUI command-line argument parser
*/

#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "Cli/ArgsParser.hpp"
#include "Cli/Exceptions/ParserException.hpp"

using zappy::gui::GuiConfig;
using zappy::gui::InvalidValueException;
using zappy::gui::MissingValueException;
using zappy::gui::parseArguments;
using zappy::gui::ParserException;
using zappy::gui::UnknownOptionException;

namespace {

GuiConfig parse(std::vector<std::string> tokens) {
  std::vector<char*> argv;
  argv.reserve(tokens.size());
  for (std::string& token : tokens) {
    argv.push_back(token.data());
  }
  return parseArguments(static_cast<int>(argv.size()), argv.data());
}

}  // namespace

TEST(GuiArgsParser, ParsesBothFlags) {
  const GuiConfig config =
      parse({"zappy_gui", "-p", "4242", "-h", "localhost"});

  EXPECT_EQ(config.port, 4242);
  EXPECT_EQ(config.hostname, "localhost");
}

TEST(GuiArgsParser, FlagsCanBeSuppliedInAnyOrder) {
  const GuiConfig config =
      parse({"zappy_gui", "-h", "192.168.1.1", "-p", "8080"});

  EXPECT_EQ(config.port, 8080);
  EXPECT_EQ(config.hostname, "192.168.1.1");
}

TEST(GuiArgsParser, AcceptsMinimumPort) {
  const GuiConfig config = parse({"zappy_gui", "-p", "1", "-h", "localhost"});

  EXPECT_EQ(config.port, 1);
}

TEST(GuiArgsParser, AcceptsMaximumPort) {
  const GuiConfig config =
      parse({"zappy_gui", "-p", "65535", "-h", "localhost"});

  EXPECT_EQ(config.port, 65535);
}

TEST(GuiArgsParser, AcceptsArbitraryHostname) {
  const GuiConfig config =
      parse({"zappy_gui", "-p", "4242", "-h", "my.server.example.com"});

  EXPECT_EQ(config.hostname, "my.server.example.com");
}

TEST(GuiArgsParser, MissingPortFlagIsRejected) {
  EXPECT_THROW(parse({"zappy_gui", "-h", "localhost"}), MissingValueException);
}

TEST(GuiArgsParser, MissingHostnameFlagIsRejected) {
  EXPECT_THROW(parse({"zappy_gui", "-p", "4242"}), MissingValueException);
}

TEST(GuiArgsParser, MissingPortValueIsRejected) {
  EXPECT_THROW(parse({"zappy_gui", "-p", "-h", "localhost"}),
               MissingValueException);
}

TEST(GuiArgsParser, MissingHostnameValueIsRejected) {
  EXPECT_THROW(parse({"zappy_gui", "-p", "4242", "-h", "-p"}),
               MissingValueException);
}

TEST(GuiArgsParser, NonIntegerPortIsRejected) {
  EXPECT_THROW(parse({"zappy_gui", "-p", "abc", "-h", "localhost"}),
               InvalidValueException);
  EXPECT_THROW(parse({"zappy_gui", "-p", "42x", "-h", "localhost"}),
               InvalidValueException);
}

TEST(GuiArgsParser, PortZeroIsRejected) {
  EXPECT_THROW(parse({"zappy_gui", "-p", "0", "-h", "localhost"}),
               InvalidValueException);
}

TEST(GuiArgsParser, PortAboveMaximumIsRejected) {
  EXPECT_THROW(parse({"zappy_gui", "-p", "65536", "-h", "localhost"}),
               InvalidValueException);
  EXPECT_THROW(parse({"zappy_gui", "-p", "70000", "-h", "localhost"}),
               InvalidValueException);
}

TEST(GuiArgsParser, NegativePortIsRejected) {
  EXPECT_THROW(parse({"zappy_gui", "-p", "-1", "-h", "localhost"}),
               InvalidValueException);
}

TEST(GuiArgsParser, OverflowingPortIsRejected) {
  EXPECT_THROW(
      parse({"zappy_gui", "-p", "99999999999999999999", "-h", "localhost"}),
      InvalidValueException);
}

TEST(GuiArgsParser, UnknownFlagIsRejected) {
  EXPECT_THROW(parse({"zappy_gui", "-z", "-p", "4242", "-h", "localhost"}),
               UnknownOptionException);
}

TEST(GuiArgsParser, EveryFailureIsAParserException) {
  EXPECT_THROW(parse({"zappy_gui", "-p", "abc", "-h", "localhost"}),
               ParserException);
  EXPECT_THROW(parse({"zappy_gui", "-h", "localhost"}), ParserException);
  EXPECT_THROW(parse({"zappy_gui", "-z"}), ParserException);
}
