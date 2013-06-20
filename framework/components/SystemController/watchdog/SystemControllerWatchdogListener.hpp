#ifndef SYSTEM_CONTROLLER_WATCHDOG_LISTENER_HPP
#define SYSTEM_CONTROLLER_WATCHDOG_LISTENER_HPP

#include "SystemControllerProxy.hpp"
#include "IStackRestarter.hpp"
#include "Logger.hpp"

namespace iviLink
{
namespace SystemController
{

class SystemControllerWatchdogListener : public SystemControllerMsgProtocol::SystemControllerProxy
{
public:
    SystemControllerWatchdogListener(IStackRestarter * restarter);
    virtual void onTimeout();
    virtual ~SystemControllerWatchdogListener() 
    {
    }

private:
    virtual void onSystemControllerNotAvaliable();

    virtual void onPhysicalLayerStarted();
    virtual void onChannelLayerStarted();
    virtual void onProfileLayerStarted();
    virtual void onApplicationLayerStarted();
    virtual void onAuthenticationStarted();
    virtual void onAuthenticationStopped();
    static Logger mLogger;

    IStackRestarter * mRestarter;
};

}; // namespace SystemController
}; // namespace iviLink

#endif //SYSTEM_CONTROLLER_WATCHDOG_LISTENER_HPP
