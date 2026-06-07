/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** BoundedNumberFieldType — decimal integer constrained to [minimum, maximum]
*/

#include "Schema/Fields/BoundedNumberFieldType.hpp"
#include <charconv>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <utility>
#include "Error/Messages/SchemaMessages.hpp"

namespace zappy::schema {

BoundedNumberFieldType::BoundedNumberFieldType(std::string name,
                                               std::int64_t minimum,
                                               std::int64_t maximum)
    : base_(std::move(name)),
      typeName_(
          error::messages::BOUNDED_NUMBER_PREFIX + std::to_string(minimum) +
          error::messages::BOUNDED_NUMBER_SEPARATOR + std::to_string(maximum) +
          error::messages::BOUNDED_NUMBER_SUFFIX),
      minimum_(minimum),
      maximum_(maximum) {
  if (minimum > maximum) {
    throw std::invalid_argument(
        error::messages::INVERTED_BOUNDS_PREFIX + std::to_string(minimum) +
        error::messages::INVERTED_BOUNDS_INFIX + std::to_string(maximum) +
        error::messages::INVERTED_BOUNDS_SUFFIX);
  }
}

const std::string &BoundedNumberFieldType::name() const { return base_.name(); }

const char *BoundedNumberFieldType::typeName() const {
  return typeName_.c_str();
}

bool BoundedNumberFieldType::validate(const std::string &value) const {
  if (!base_.validate(value)) {
    return false;
  }
  std::int64_t parsed = 0;
  const char *begin = value.data();
  const char *end = begin + value.size();
  const std::from_chars_result result = std::from_chars(begin, end, parsed);
  if (result.ec != std::errc() || result.ptr != end) {
    return false;
  }
  return parsed >= minimum_ && parsed <= maximum_;
}

bool BoundedNumberFieldType::consumesRemainder() const { return false; }

std::int64_t BoundedNumberFieldType::minimum() const { return minimum_; }

std::int64_t BoundedNumberFieldType::maximum() const { return maximum_; }

}  // namespace zappy::schema
