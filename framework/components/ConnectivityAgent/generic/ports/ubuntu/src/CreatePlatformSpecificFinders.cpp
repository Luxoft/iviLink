#include "CreatePlatformSpecificFinders.hpp"

using namespace iviLink::ConnectivityAgent::HAL;

#include "CTcpAutoConnectionFinder.hpp"
#include "BluetoothConnectionFinder.hpp"


tFinders iviLink::ConnectivityAgent::HAL::createListOfFinders(IFoundConnectionHandler& handler, EGenderType gender, bool isActive)
{
    tFinders result;
    result.push_back(new CTcpAutoConnectionFinder(handler, gender));
    result.push_back(new BluetoothConnectionFinder(handler, gender));
    result[0]->setAutoConnectionAllowed(isActive);
    return result;
}
