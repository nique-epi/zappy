/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Scheduler exception hierarchy
*/

#include "App/Scheduler/Exceptions/SchedulerException.hpp"
#include <string>
#include "Error/Messages/SchedulerMessages.hpp"

namespace zappy::server {

SchedulerException::SchedulerException(const std::string &message)
    : std::runtime_error(message) {}

InvalidFrequencyException::InvalidFrequencyException(int frequency)
    : SchedulerException(error::messages::INVALID_FREQUENCY_PREFIX +
                         std::to_string(frequency) +
                         error::messages::INVALID_FREQUENCY_SUFFIX) {}

}  // namespace zappy::server
