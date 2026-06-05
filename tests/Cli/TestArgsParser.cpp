/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Unit tests for the schema-driven command-line argument parser
*/

#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "Cli/ArgsParser.hpp"
#include "Cli/Exceptions/ParserException.hpp"

using zappy::server::InvalidValueException;
using zappy::server::MissingValueException;
using zappy::server::parseArguments;
using zappy::server::ParserException;
using zappy::server::ServerConfig;
using zappy::server::UnknownOptionException;

namespace {

ServerConfig parse(std::vector<std::string> tokens) {
  std::vector<char *> argv;
  argv.reserve(tokens.size());
  for (std::string &token : tokens) {
    argv.push_back(token.data());
  }
  return parseArguments(static_cast<int>(argv.size()), argv.data());
}

}  // namespace

TEST(ArgsParser, KeepsDefaultsWhenOnlyTeamsAreGiven) {
  const ServerConfig config = parse({"zappy_server", "-n", "red"});

  EXPECT_EQ(config.port, 4242);
  EXPECT_EQ(config.width, 10);
  EXPECT_EQ(config.height, 10);
  EXPECT_EQ(config.clientsPerTeam, 1);
  EXPECT_EQ(config.frequency, 100);
  EXPECT_EQ(config.teamNames, std::vector<std::string>{"red"});
}

TEST(ArgsParser, ParsesEveryOption) {
  const ServerConfig config =
      parse({"zappy_server", "-p", "8080", "-x", "20", "-y", "30", "-c", "5",
             "-f", "200", "-n", "red", "blue", "green"});

  EXPECT_EQ(config.port, 8080);
  EXPECT_EQ(config.width, 20);
  EXPECT_EQ(config.height, 30);
  EXPECT_EQ(config.clientsPerTeam, 5);
  EXPECT_EQ(config.frequency, 200);
  EXPECT_EQ(config.teamNames,
            (std::vector<std::string>{"red", "blue", "green"}));
}

TEST(ArgsParser, TeamListStopsAtNextOption) {
  const ServerConfig config =
      parse({"zappy_server", "-n", "red", "blue", "-p", "5000"});

  EXPECT_EQ(config.teamNames, (std::vector<std::string>{"red", "blue"}));
  EXPECT_EQ(config.port, 5000);
}

TEST(ArgsParser, UnknownOptionIsRejected) {
  EXPECT_THROW(parse({"zappy_server", "-z", "-n", "red"}),
               UnknownOptionException);
}

TEST(ArgsParser, MissingIntegerValueIsRejected) {
  EXPECT_THROW(parse({"zappy_server", "-p"}), MissingValueException);
  EXPECT_THROW(parse({"zappy_server", "-p", "-n", "red"}),
               MissingValueException);
}

TEST(ArgsParser, MissingTeamNamesAreRejected) {
  EXPECT_THROW(parse({"zappy_server", "-p", "8080"}), MissingValueException);
  EXPECT_THROW(parse({"zappy_server", "-n"}), MissingValueException);
  EXPECT_THROW(parse({"zappy_server", "-n", "-p", "8080"}),
               MissingValueException);
}

TEST(ArgsParser, NonIntegerValueIsRejected) {
  EXPECT_THROW(parse({"zappy_server", "-p", "abc", "-n", "red"}),
               InvalidValueException);
  EXPECT_THROW(parse({"zappy_server", "-x", "12x", "-n", "red"}),
               InvalidValueException);
}

TEST(ArgsParser, SignedNumberIsValueNotOption) {
  EXPECT_THROW(parse({"zappy_server", "-p", "-1", "-n", "red"}),
               InvalidValueException);
  EXPECT_THROW(parse({"zappy_server", "-x", "-5", "-n", "red"}),
               InvalidValueException);
}

TEST(ArgsParser, OutOfRangeValueIsRejected) {
  EXPECT_THROW(parse({"zappy_server", "-p", "0", "-n", "red"}),
               InvalidValueException);
  EXPECT_THROW(parse({"zappy_server", "-p", "70000", "-n", "red"}),
               InvalidValueException);
  EXPECT_THROW(parse({"zappy_server", "-x", "0", "-n", "red"}),
               InvalidValueException);
}

TEST(ArgsParser, OverflowingIntegerIsRejected) {
  EXPECT_THROW(
      parse({"zappy_server", "-p", "99999999999999999999", "-n", "red"}),
      InvalidValueException);
}

TEST(ArgsParser, EveryFailureIsAParserException) {
  EXPECT_THROW(parse({"zappy_server", "-z"}), ParserException);
  EXPECT_THROW(parse({"zappy_server", "-p", "abc", "-n", "red"}),
               ParserException);
}
