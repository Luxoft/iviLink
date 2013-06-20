#ifndef SYSTEM_CONTROLLER_WATCHDOG_PROCESS_HPP
#define SYSTEM_CONTROLLER_WATCHDOG_PROCESS_HPP

#include "IStackRestarter.hpp"

namespace iviLink
{
namespace SystemController
{

int watchdogProcessEntryPoint(IStackRestarter * restarter);

} // namespace SystemController
} // namespace iviLink


#endif //SYSTEM_CONTROLLER_WATCHDOG_PROCESS_HPP