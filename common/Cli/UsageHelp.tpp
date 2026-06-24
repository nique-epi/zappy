/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** UsageHelp.tpp — template implementation of the UsageHelp class
*/

#pragma once

#include <algorithm>
#include <cstddef>
#include <string>
#include <utility>
#include <vector>
#include "Cli/OptionSchema.hpp"
#include "Error/Messages/ParserMessages.hpp"
#include "Schema/Fields/IFieldType.hpp"

namespace zappy::cli {

template <typename Config>
UsageHelp<Config>::UsageHelp(std::string synopsis,
                             std::vector<OptionSchema<Config>> options)
    : synopsis_(std::move(synopsis)), options_(std::move(options)) {}

template <typename Config>
bool UsageHelp<Config>::reservesShortFlag() const {
  return std::ranges::any_of(options_, [](const OptionSchema<Config> &option) {
    return option.flag == shortFlag;
  });
}

template <typename Config>
bool UsageHelp<Config>::isRequested(int argumentCount, char **arguments) const {
  const bool shortMeansHelp = !reservesShortFlag();
  for (int index = 1; index < argumentCount; ++index) {
    const std::string token(arguments[index]);
    if (token == longFlag || (shortMeansHelp && token == shortFlag)) {
      return true;
    }
  }
  return false;
}

template <typename Config>
std::string UsageHelp<Config>::text() const {
  std::vector<std::string> rows;
  rows.reserve(options_.size());
  std::size_t flagWidth = 0;
  for (const OptionSchema<Config> &option : options_) {
    std::string row = option.flag;
    if (option.fieldType) {
      row += ' ';
      row += option.fieldType->name();
    }
    flagWidth = std::max(flagWidth, row.size());
    rows.push_back(std::move(row));
  }

  std::string usage = zappy::error::messages::USAGE_PREFIX;
  usage += synopsis_;
  for (std::size_t index = 0; index < options_.size(); ++index) {
    usage += '\n';
    usage += rowIndent;
    usage += rows[index];
    usage.append(flagWidth - rows[index].size() + rowColumnGap, ' ');
    usage += options_[index].description;
  }
  return usage;
}

}  // namespace zappy::cli
