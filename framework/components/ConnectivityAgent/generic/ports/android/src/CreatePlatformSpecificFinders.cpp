#include "CreatePlatformSpecificFinders.hpp"

using namespace iviLink::ConnectivityAgent::HAL;

#include "CAndroidBluetoothConnectionFinder.hpp"
#include "TcpMobileConnectionFinder.hpp"


tFinders iviLink::ConnectivityAgent::HAL::createListOfFinders(IFoundConnectionHandler& handler, EGenderType gender, bool isActive)
{
    tFinders result;
    result.push_back(new TcpMobileConnectionFinder(handler, gender));
    result.push_back(new CAndroidBluetoothConnectionFinder(handler, gender));
    result[0]->setAutoConnectionAllowed(isActive);
    return result;
}
