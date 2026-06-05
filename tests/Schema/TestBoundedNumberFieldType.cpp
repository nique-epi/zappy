/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Unit tests for the zappy::schema BoundedNumberFieldType
*/

#include <gtest/gtest.h>
#include <string>
#include "Schema/Fields/BoundedNumberFieldType.hpp"

using zappy::schema::BoundedNumberFieldType;

TEST(BoundedNumberFieldType, AcceptsValuesInsideRange) {
  const BoundedNumberFieldType field("port", 1, 65535);

  EXPECT_TRUE(field.validate("1"));
  EXPECT_TRUE(field.validate("4242"));
  EXPECT_TRUE(field.validate("65535"));
}

TEST(BoundedNumberFieldType, RejectsValuesOutsideRange) {
  const BoundedNumberFieldType field("port", 1, 65535);

  EXPECT_FALSE(field.validate("0"));
  EXPECT_FALSE(field.validate("65536"));
  EXPECT_FALSE(field.validate("-1"));
}

TEST(BoundedNumberFieldType, RejectsNonIntegerAndOverflow) {
  const BoundedNumberFieldType field("count", 1, 2147483647);

  EXPECT_FALSE(field.validate(""));
  EXPECT_FALSE(field.validate("abc"));
  EXPECT_FALSE(field.validate("12x"));
  EXPECT_FALSE(field.validate("99999999999999999999"));
}

TEST(BoundedNumberFieldType, ExposesBoundsAndRangeTypeName) {
  const BoundedNumberFieldType field("port", 1, 65535);

  EXPECT_EQ(field.minimum(), 1);
  EXPECT_EQ(field.maximum(), 65535);
  EXPECT_EQ(field.name(), "port");
  EXPECT_EQ(std::string(field.typeName()), "number in [1, 65535]");
  EXPECT_FALSE(field.consumesRemainder());
}
