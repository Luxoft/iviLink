#include "CreatePlatformSpecificFinders.hpp"

using namespace iviLink::ConnectivityAgent::HAL;

#include "TcpMobileConnectionFinder.hpp"


tFinders iviLink::ConnectivityAgent::HAL::createListOfFinders(IFoundConnectionHandler& handler, EGenderType gender, bool isActive)
{
    tFinders result;
    result.push_back(new TcpMobileConnectionFinder(handler, gender));
    return result;
}