/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Unit tests for the zappy::server parser exception hierarchy
*/

#include <gtest/gtest.h>
#include <stdexcept>
#include <string>
#include "Cli/Exceptions/ParserException.hpp"

using zappy::server::InvalidValueException;
using zappy::server::MissingValueException;
using zappy::server::ParserException;
using zappy::server::UnknownOptionException;

TEST(ParserExceptions, MessagesAreComposed) {
  EXPECT_EQ(std::string(ParserException("root").what()), "root");
  EXPECT_EQ(std::string(UnknownOptionException("-z").what()),
            "unknown option -z");
  EXPECT_EQ(std::string(MissingValueException("-p").what()),
            "missing value for -p");
  EXPECT_EQ(
      std::string(
          InvalidValueException("-p", "foo", "expected an integer").what()),
      "invalid value 'foo' for -p: expected an integer");
}

TEST(ParserExceptions, EverySubclassIsAParserException) {
  EXPECT_THROW(throw UnknownOptionException("-z"), ParserException);
  EXPECT_THROW(throw MissingValueException("-p"), ParserException);
  EXPECT_THROW(throw InvalidValueException("-p", "x", "y"), ParserException);
}

TEST(ParserExceptions, ParserExceptionIsAStdRuntimeError) {
  EXPECT_THROW(throw ParserException("boom"), std::runtime_error);
  EXPECT_THROW(throw UnknownOptionException("-z"), std::exception);
}
