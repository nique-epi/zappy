/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** BoundedNumberFieldType — decimal integer constrained to [minimum, maximum]
*/

#pragma once

#include <cstdint>
#include <string>
#include "Schema/Fields/IFieldType.hpp"
#include "Schema/Fields/NumberFieldType.hpp"

namespace zappy::schema {

/**
 * @brief Field type accepting a decimal integer within an inclusive range.
 *
 * Builds on @ref NumberFieldType for the digit check, then rejects any value
 * outside `[minimum, maximum]` (overflow included). Shared by the RPC schema
 * and the server command-line parser.
 */
class BoundedNumberFieldType : public IFieldType {
 public:
  BoundedNumberFieldType(std::string name, std::int64_t minimum,
                         std::int64_t maximum);
  const std::string &name() const override;
  const char *typeName() const override;
  bool validate(const std::string &value) const override;
  bool consumesRemainder() const override;

  /**
   * @brief Inclusive lower bound accepted by @ref validate.
   */
  [[nodiscard]] std::int64_t minimum() const;

  /**
   * @brief Inclusive upper bound accepted by @ref validate.
   */
  [[nodiscard]] std::int64_t maximum() const;

 private:
  NumberFieldType base_;
  std::string typeName_;
  std::int64_t minimum_;
  std::int64_t maximum_;
};

}  // namespace zappy::schema
