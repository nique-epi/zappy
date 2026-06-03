/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** StringFieldType — non-empty string field
*/

#pragma once

#include <string>
#include "Schema/Fields/IFieldType.hpp"

namespace zappy::schema {

/**
 * @brief Field type accepting any non-empty token.
 */
class StringFieldType : public IFieldType {
 public:
  explicit StringFieldType(std::string name);
  const std::string &name() const override;
  const char *typeName() const override;
  bool validate(const std::string &value) const override;
  bool consumesRemainder() const override;

 private:
  std::string name_;
};

}  // namespace zappy::schema
