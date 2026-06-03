/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Schema — typed struct-binding schema with parse + validation
*/

#pragma once

#include <cstddef>
#include <memory>
#include <optional>
#include <regex>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>
#include "Schema/Fields/Fields.hpp"
#include "Schema/SchemaError.hpp"
#include "Schema/Tokenizer.hpp"

namespace zappy::schema {

/**
 * @brief Result of Schema::parse() — either a valid T or a SchemaError.
 */
template <typename T>
class ParseResult {
 public:
  ParseResult(T value) : data_(std::move(value)) {}
  ParseResult(SchemaError error) : data_(std::move(error)) {}

  bool ok() const { return std::holds_alternative<T>(data_); }
  explicit operator bool() const { return ok(); }

  T &value() { return std::get<T>(data_); }
  const T &value() const { return std::get<T>(data_); }
  T *operator->() { return &value(); }
  const T *operator->() const { return &value(); }

  SchemaError &error() { return std::get<SchemaError>(data_); }
  const SchemaError &error() const { return std::get<SchemaError>(data_); }

 private:
  std::variant<T, SchemaError> data_;
};

/**
 * @brief Schema bound to a struct T — validates and populates fields by
 *        position.
 *
 * Each field declares a FieldType for validation and a member pointer to
 * populate. All struct fields must be std::string — protocol values are
 * strings. Two modifiers chain right after .field(...), both mutating the
 * most recently declared field: .optional() and .regex(pattern).
 */
template <typename T>
class Schema {
 public:
  Schema() = default;
  Schema(const Schema &) = delete;
  Schema &operator=(const Schema &) = delete;
  Schema(Schema &&) = default;
  Schema &operator=(Schema &&) = default;

  /**
   * @brief Declare the next positional field.
   * @tparam FieldType Validation type (StringFieldType, NumberFieldType, …).
   * @param name Human-readable field name, surfaced in SchemaError::field.
   * @param member Pointer to the std::string member to populate in T.
   */
  template <typename FieldType>
  Schema &field(std::string name, std::string T::*member) & {
    fields_.push_back({
        std::make_unique<FieldType>(std::move(name)),
        member,
        std::nullopt,
        {},
        false,
    });
    return *this;
  }

  template <typename FieldType>
  Schema &&field(std::string name, std::string T::*member) && {
    field<FieldType>(std::move(name), member);
    return std::move(*this);
  }

  /**
   * @brief Mark the most recently declared field as optional.
   * @throws std::logic_error if called before any field() declaration.
   */
  Schema &optional() & {
    if (fields_.empty()) {
      throw std::logic_error(
          "Schema::optional() called before any field() declaration");
    }
    fields_.back().optional = true;
    return *this;
  }

  Schema &&optional() && {
    optional();
    return std::move(*this);
  }

  /**
   * @brief Attach a regex rule to the most recently declared field.
   * @param pattern ECMAScript regex used with std::regex_match (full match).
   * @throws std::logic_error if called before any field() declaration.
   * @throws std::regex_error if the pattern is invalid.
   */
  Schema &regex(const std::string &pattern) & {
    if (fields_.empty()) {
      throw std::logic_error(
          "Schema::regex() called before any field() declaration");
    }
    fields_.back().pattern = std::regex(pattern);
    fields_.back().patternSource = pattern;
    return *this;
  }

  Schema &&regex(const std::string &pattern) && {
    regex(pattern);
    return std::move(*this);
  }

  /**
   * @brief Parse and validate a raw protocol line.
   */
  ParseResult<T> parse(const std::string &line) const {
    return parse(tokenize(line));
  }

  /**
   * @brief Parse and validate pre-tokenized arguments.
   */
  ParseResult<T> parse(const std::vector<std::string> &tokens) const {
    T result{};
    auto error = parseInto(tokens, result);
    if (error) {
      return *error;
    }
    return result;
  }

  /**
   * @brief Parse into a pre-constructed T.
   * @returns std::nullopt on success, or a SchemaError on failure.
   */
  std::optional<SchemaError> parseInto(const std::vector<std::string> &tokens,
                                       T &out) const {
    const bool greedyTail =
        !fields_.empty() && fields_.back().fieldType->consumesRemainder();
    if (tokens.size() > fields_.size() && !greedyTail) {
      return SchemaError{
          "",
          "expected at most " + std::to_string(fields_.size()) +
              " argument(s), got " + std::to_string(tokens.size()),
      };
    }
    std::size_t requiredCount = 0;
    for (const auto &field : fields_) {
      if (!field.optional) {
        ++requiredCount;
      }
    }
    for (std::size_t i = 0; i < fields_.size(); ++i) {
      const auto &fieldType = fields_[i].fieldType;
      const bool hasToken = i < tokens.size();
      const bool isGreedyTail = greedyTail && i + 1 == fields_.size();
      const std::string value =
          hasToken ? (isGreedyTail ? joinFrom(tokens, i) : tokens[i])
                   : std::string();

      if (value.empty() && fields_[i].optional) {
        out.*fields_[i].member = std::string();
        continue;
      }

      if (!hasToken) {
        return SchemaError{
            fieldType->name(),
            "expected at least " + std::to_string(requiredCount) +
                " argument(s), got " + std::to_string(tokens.size()),
        };
      }

      if (!fieldType->validate(value)) {
        return SchemaError{
            fieldType->name(),
            std::string("expected ") + fieldType->typeName() + ", got '" +
                value + "'",
        };
      }
      if (fields_[i].pattern && !std::regex_match(value, *fields_[i].pattern)) {
        return SchemaError{
            fieldType->name(),
            std::string("value '") + value + "' does not match pattern " +
                fields_[i].patternSource,
        };
      }
      out.*fields_[i].member = value;
    }
    return std::nullopt;
  }

  std::size_t fieldCount() const { return fields_.size(); }

 private:
  static std::string joinFrom(const std::vector<std::string> &tokens,
                              std::size_t start) {
    std::string joined = tokens[start];
    for (std::size_t i = start + 1; i < tokens.size(); ++i) {
      joined += ' ';
      joined += tokens[i];
    }
    return joined;
  }

  struct FieldDescriptor {
    std::unique_ptr<IFieldType> fieldType;
    std::string T::*member;
    std::optional<std::regex> pattern;
    std::string patternSource;
    bool optional;
  };

  std::vector<FieldDescriptor> fields_;
};

}  // namespace zappy::schema
