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

using zappy::cli::InvalidValueException;
using zappy::cli::MissingValueException;
using zappy::cli::ParserException;
using zappy::cli::UnknownOptionException;
using zappy::server::helpRequested;
using zappy::server::parseArguments;
using zappy::server::ServerConfig;
using zappy::server::usageMessage;

namespace {

ServerConfig parse(std::vector<std::string> tokens) {
  std::vector<char*> argv;
  argv.reserve(tokens.size());
  for (std::string& token : tokens) {
    argv.push_back(token.data());
  }
  return parseArguments(static_cast<int>(argv.size()), argv.data());
}

bool helpRequestedFor(std::vector<std::string> tokens) {
  std::vector<char*> argv;
  argv.reserve(tokens.size());
  for (std::string& token : tokens) {
    argv.push_back(token.data());
  }
  return helpRequested(static_cast<int>(argv.size()), argv.data());
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

// Given a command line carrying --help
// When help detection runs
// Then it reports that the usage was requested
TEST(ArgsParser, LongHelpFlagIsDetected) {
  EXPECT_TRUE(helpRequestedFor({"zappy_server", "--help"}));
}

// Given a command line carrying -h, a flag the server does not reserve
// When help detection runs
// Then it reports that the usage was requested
TEST(ArgsParser, ShortHelpFlagIsDetected) {
  EXPECT_TRUE(helpRequestedFor({"zappy_server", "-h"}));
}

// Given a command line where --help follows otherwise valid options
// When help detection runs
// Then it still reports that the usage was requested
TEST(ArgsParser, HelpFlagIsDetectedAmongOtherArguments) {
  EXPECT_TRUE(
      helpRequestedFor({"zappy_server", "-p", "8080", "--help", "-n", "red"}));
}

// Given a regular command line with no help flag
// When help detection runs
// Then it reports that the usage was not requested
TEST(ArgsParser, NoHelpFlagMeansNoHelpRequested) {
  EXPECT_FALSE(helpRequestedFor({"zappy_server", "-p", "8080", "-n", "red"}));
}

// Given the server option schema
// When the usage message is rendered
// Then it carries the USAGE synopsis plus every flag and its description
TEST(ArgsParser, UsageMessageListsSynopsisFlagsAndDescriptions) {
  const std::string usage = usageMessage();

  EXPECT_NE(usage.find("USAGE: ./zappy_server"), std::string::npos);
  EXPECT_NE(usage.find("-p port"), std::string::npos);
  EXPECT_NE(usage.find("TCP port to listen on"), std::string::npos);
  EXPECT_NE(usage.find("-n teamName"), std::string::npos);
  EXPECT_NE(usage.find("team names"), std::string::npos);
}
