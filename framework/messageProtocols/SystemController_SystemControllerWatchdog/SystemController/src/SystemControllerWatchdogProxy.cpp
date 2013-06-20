#include <strings.h>

#include "SystemControllerWatchdogProxy.hpp"

using namespace iviLink::Ipc;
namespace SystemControllerMsgProtocol 
{

Logger SystemControllerWatchdogProxy::mLogger = Logger::getInstance("systemController.msgProtocol.SystemControllerWatchdogProxy");

SystemControllerWatchdogProxy::SystemControllerWatchdogProxy(std::string const& connectionName)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    mIpc = new CIpc(connectionName, *this);
}

SystemControllerWatchdogProxy::~SystemControllerWatchdogProxy()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    delete mIpc;
}

BaseError SystemControllerWatchdogProxy::onPhysicalLayerStarted()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    return sendMessage(SC_WD_LAUNCHED_PHYSICAL_LAYER);
}

BaseError SystemControllerWatchdogProxy::onChannelLayerStarted()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    return sendMessage(SC_WD_LAUNCHED_CHANNEL_LAYER);
}

BaseError SystemControllerWatchdogProxy::onProfileLayerStarted()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    return sendMessage(SC_WD_LAUNCHED_PROFILE_LAYER);
}

BaseError SystemControllerWatchdogProxy::onApplicationLayerStarted()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    return sendMessage(SC_WD_LAUNCHED_APPLICATION_LAYER);
}

BaseError SystemControllerWatchdogProxy::onAuthenticationStarted()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    return sendMessage(SC_WD_LAUNCHED_AUTHENTICATON);    
}

BaseError SystemControllerWatchdogProxy::onAuthenticationStopped()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    return sendMessage(SC_WD_STOPPED_AUTHENTICATION);    
}

BaseError SystemControllerWatchdogProxy::sendMessage(SystemControllerToWatchdog message)
{
   LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);

    if (!mIpc)
      return BaseError(1, getName(), BaseError::IVILINK_FATAL, "no ipc");

    iviLink::Ipc::MsgID id = mMsgIdGen.getNext();
    UInt32 const reqSize = sizeof(SystemControllerToWatchdog);
    UInt8 messageArray[reqSize];
    bzero(messageArray, reqSize);
    memcpy(messageArray, &message, reqSize);
    return mIpc->asyncRequest(id, messageArray, reqSize);
}

void SystemControllerWatchdogProxy::OnConnection(iviLink::Ipc::DirectionID)
{
   LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
   onSystemControllerWatchdogAvaliable();
}

void SystemControllerWatchdogProxy::OnConnectionLost(iviLink::Ipc::DirectionID)
{
   LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
   onSystemControllerWatchdogNotAvaliable();
}

void SystemControllerWatchdogProxy::OnRequest(iviLink::Ipc::MsgID id, UInt8 const* pPayload, UInt32 payloadSize,
            UInt8* const pResponseBuffer, UInt32& bufferSize, iviLink::Ipc::DirectionID)
{
   LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
   LOG4CPLUS_WARN(mLogger, "This shouldn't happen");
   bufferSize = 0;
}

void SystemControllerWatchdogProxy::OnAsyncRequest(iviLink::Ipc::MsgID id, UInt8 const* pPayload, UInt32 payloadSize,
            iviLink::Ipc::DirectionID)
{
   LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
   LOG4CPLUS_WARN(mLogger, "This shouldn't happen");
}

BaseError SystemControllerWatchdogProxy::connectSystemControllerWatchdog()
{
   LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
   return mIpc->beginWaitForConnection();
}

bool SystemControllerWatchdogProxy::isConnected() const
{
    return mIpc->isConnected();
}

}

