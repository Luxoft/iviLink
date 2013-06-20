#ifndef CREATE_PLATFORM_SPECIFIC_FINDERS_HPP
#define CREATE_PLATFORM_SPECIFIC_FINDERS_HPP

#include <vector>

#include "CConnectionFinder.hpp"
#include "IFoundConnectionHandler.hpp"
#include "EGenderType.hpp"

namespace iviLink
{
namespace ConnectivityAgent
{
namespace HAL
{
    typedef std::vector<CConnectionFinder*> tFinders;
    /**
     * Returned vector should contain CConectionFinder implementations, specific to current platform.
     */
    tFinders createListOfFinders(IFoundConnectionHandler& handler, EGenderType gender, bool isActive = true);
}
}
}

#endif //CREATE_PLATFORM_SPECIFIC_FINDERS_HPP
