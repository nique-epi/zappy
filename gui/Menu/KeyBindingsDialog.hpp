/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** KeyBindingsDialog — rebindable key configuration overlay
*/

#pragma once  // NOLINT(llvm-header-guard)

#include <cstdint>
#include <string>
#include "Bindings/KeyBindings.hpp"

namespace zappy::gui {

enum class BindingsResult : std::uint8_t { Open, Closed };

class KeyBindingsDialog {
 public:
  void open();
  BindingsResult draw();
  [[nodiscard]] const KeyBindings& result() const;

 private:
  enum class InternalState : std::uint8_t { Idle, ConfirmDiscard };

  [[nodiscard]] bool hasConflict() const;
  [[nodiscard]] bool hasUnsavedChanges() const;

  KeyBindings committed_{};
  KeyBindings draft_{};
  InternalState state_{InternalState::Idle};
  int listeningRow_{-1};
  bool justOpened_{false};
  std::string conflictError_;
};

}  // namespace zappy::gui
