/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Tokenizer — splits a protocol line into a vector of string tokens
*/

#include "Schema/Tokenizer.hpp"
#include <cctype>
#include <utility>

namespace zappy::schema {

std::vector<std::string> tokenize(const std::string &line) {
  std::vector<std::string> tokens;
  std::size_t index = 0;
  const std::size_t length = line.size();

  while (index < length) {
    while (index < length &&
           std::isspace(static_cast<unsigned char>(line[index]))) {
      ++index;
    }
    if (index == length) {
      break;
    }

    if (line[index] == '"') {
      ++index;
      std::string token;
      while (index < length) {
        if (line[index] == '\\' && index + 1 < length &&
            line[index + 1] == '"') {
          token += '"';
          index += 2;
        } else if (line[index] == '"') {
          ++index;
          break;
        } else {
          token += line[index++];
        }
      }
      tokens.push_back(std::move(token));
    } else {
      std::size_t start = index;
      while (index < length &&
             !std::isspace(static_cast<unsigned char>(line[index]))) {
        ++index;
      }
      tokens.push_back(line.substr(start, index - start));
    }
  }

  return tokens;
}

}  // namespace zappy::schema
