/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** NumberFieldType — decimal integer field (optionally negative)
*/

#pragma once

#include <string>
#include "Schema/Fields/IFieldType.hpp"

namespace zappy::schema {

/**
 * @brief Field type accepting a signed decimal integer token.
 */
class NumberFieldType : public IFieldType {
 public:
  explicit NumberFieldType(std::string name);
  const std::string &name() const override;
  const char *typeName() const override;
  bool validate(const std::string &value) const override;
  bool consumesRemainder() const override;

 private:
  std::string name_;
};

}  // namespace zappy::schema
