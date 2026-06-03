/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Connected client state — pure buffer, no I/O syscalls
*/

#include "Network/Server/Client.hpp"
#include <unistd.h>
#include <utility>

namespace zappy::network {

Client::Client(int fd) : fd_(fd) {}

Client::~Client() {
  if (fd_ >= 0) {
    close(fd_);
  }
}

Client::Client(Client &&other) noexcept
    : fd_(other.fd_),
      inputBuffer_(std::move(other.inputBuffer_)),
      outputBuffer_(std::move(other.outputBuffer_)) {
  other.fd_ = -1;
}

Client &Client::operator=(Client &&other) noexcept {
  if (this != &other) {
    if (fd_ >= 0) {
      close(fd_);
    }
    fd_ = other.fd_;
    inputBuffer_ = std::move(other.inputBuffer_);
    outputBuffer_ = std::move(other.outputBuffer_);
    other.fd_ = -1;
  }
  return *this;
}

int Client::fd() const { return fd_; }

void Client::appendInput(const char *data, std::size_t length) {
  inputBuffer_.append(data, length);
}

bool Client::consumeLine(std::string &out) {
  auto position = inputBuffer_.find('\n');
  if (position == std::string::npos) {
    return false;
  }
  out = inputBuffer_.substr(0, position);
  inputBuffer_.erase(0, position + 1);
  return true;
}

void Client::appendOutput(const std::string &data) { outputBuffer_ += data; }

bool Client::hasPendingOutput() const { return !outputBuffer_.empty(); }

const std::string &Client::peekOutput() const { return outputBuffer_; }

void Client::consumeOutput(std::size_t count) { outputBuffer_.erase(0, count); }

}  // namespace zappy::network
