#include "SystemControllerWatchdogListener.hpp"
#include "SystemControllerWatchdogCommon.hpp"
#include "SystemControllerWatchdog.hpp"

Logger iviLink::SystemController::SystemControllerWatchdogListener::mLogger = Logger::getInstance("SystemControllerWatchdog.Listener");

namespace iviLink
{
namespace SystemController
{

SystemControllerWatchdogListener::SystemControllerWatchdogListener(IStackRestarter * restarter)
: SystemControllerProxy("SysCtrl_Watchdog")
, mRestarter(restarter)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);

    BaseError err = SystemControllerProxy::connectSystemController();

    if (!err.isNoError())
    {
        LOG4CPLUS_FATAL(mLogger, "Unable to establish connection to system controller " + (std::string)err);
        mRestarter->restartIviLinkFromWatchdog();
    }
    SystemControllerWatchdog::getInstance()->addComponent(new Component(SYSTEM_CONTROLLER));
}

void SystemControllerWatchdogListener::onTimeout()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    mRestarter->restartIviLinkFromWatchdog();
}

void SystemControllerWatchdogListener::onSystemControllerNotAvaliable()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    LOG4CPLUS_WARN(mLogger, "IPC disconnected");
}

void SystemControllerWatchdogListener::onPhysicalLayerStarted()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    LOG4CPLUS_INFO(mLogger, "CONNECTIVITY_AGENT_LAUNCHED");
    SystemControllerWatchdog::getInstance()->addComponent(new Component(CONNECTIVITY_AGENT));
}

void SystemControllerWatchdogListener::onChannelLayerStarted()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    LOG4CPLUS_INFO(mLogger, "CHANNEL_SUPERVISOR_LAUNCHED");
    SystemControllerWatchdog::getInstance()->addComponent(new Component(CHANNEL_SUPERVISOR));
}

void SystemControllerWatchdogListener::onProfileLayerStarted()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    LOG4CPLUS_INFO(mLogger, "PROFILE_MANAGER_LAUNCHED");
    SystemControllerWatchdog::getInstance()->addComponent(new Component(PROFILE_MANAGER));
}

void SystemControllerWatchdogListener::onApplicationLayerStarted()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    LOG4CPLUS_INFO(mLogger, "APPLICATION_MANAGER_LAUNCHED");
    SystemControllerWatchdog::getInstance()->addComponent(new Component(APPLICATION_MANAGER));
}

void SystemControllerWatchdogListener::onAuthenticationStarted()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    LOG4CPLUS_INFO(mLogger, "AUTHENTICATION_APP");
    SystemControllerWatchdog::getInstance()->addComponent(new Component(AUTHENTICATION_APP));
}

void SystemControllerWatchdogListener::onAuthenticationStopped()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    LOG4CPLUS_INFO(mLogger, "AUTHENTICATION_FINISHED");          
    SystemControllerWatchdog::getInstance()->removeComponent(SystemController::AUTHENTICATION_APP); 
}

} // namespace SystemController
} // namespace iviLink
