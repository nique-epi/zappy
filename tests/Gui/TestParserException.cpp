/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Unit tests for the zappy::cli parser exception hierarchy
*/

#include <gtest/gtest.h>
#include <stdexcept>
#include <string>
#include "Cli/Exceptions/ParserException.hpp"

using zappy::cli::InvalidValueException;
using zappy::cli::MissingValueException;
using zappy::cli::ParserException;
using zappy::cli::UnknownOptionException;

TEST(GuiParserExceptions, MessagesAreComposed) {
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

TEST(GuiParserExceptions, EverySubclassIsAParserException) {
  EXPECT_THROW(throw UnknownOptionException("-z"), ParserException);
  EXPECT_THROW(throw MissingValueException("-p"), ParserException);
  EXPECT_THROW(throw InvalidValueException("-p", "x", "y"), ParserException);
}

TEST(GuiParserExceptions, ParserExceptionIsAStdRuntimeError) {
  EXPECT_THROW(throw ParserException("boom"), std::runtime_error);
  EXPECT_THROW(throw UnknownOptionException("-z"), std::exception);
}
