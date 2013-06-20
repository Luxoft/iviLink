#ifndef SYSTEM_CONTROLLER_WATCHDOG_PROXY_HPP
#define SYSTEM_CONTROLLER_WATCHDOG_PROXY_HPP

#include <string>

#include "BaseError.hpp"
#include "ICallbackHandler.hpp"
#include "ipc_common.hpp"
#include "CIpc.hpp"

#include "Logger.hpp"
#include "SystemControllerWatchdogProtocol.hpp"
#include "CommonMessage.hpp"


namespace SystemControllerMsgProtocol 
{

class SystemControllerWatchdogProxy : public iviLink::Ipc::ICallbackHandler
{
    const char* getName()
    {
        return "SystemControllerWatchdogProxy";
    }

public:
    explicit SystemControllerWatchdogProxy(std::string const& connectionName);
    virtual ~SystemControllerWatchdogProxy();

protected:
    // outgoing requests
    BaseError onPhysicalLayerStarted();
    BaseError onChannelLayerStarted();
    BaseError onProfileLayerStarted();
    BaseError onApplicationLayerStarted();
    BaseError onAuthenticationStarted();
    BaseError onAuthenticationStopped();
private:
    BaseError sendMessage(SystemControllerToWatchdog message);

protected:
    // from ICallbackHandler
    virtual void OnConnection(iviLink::Ipc::DirectionID);
    virtual void OnConnectionLost(iviLink::Ipc::DirectionID);
    virtual void OnRequest(iviLink::Ipc::MsgID id, UInt8 const* pPayload, UInt32 payloadSize,
            UInt8* const pResponseBuffer, UInt32& bufferSize, iviLink::Ipc::DirectionID);
    virtual void OnAsyncRequest(iviLink::Ipc::MsgID id, UInt8 const* pPayload, UInt32 payloadSize,
            iviLink::Ipc::DirectionID);

    virtual void onSystemControllerWatchdogAvaliable() = 0;
    virtual void onSystemControllerWatchdogNotAvaliable() = 0;

    CMsgIdGen mMsgIdGen;

    BaseError connectSystemControllerWatchdog();

    bool isConnected() const;
    
    static Logger mLogger;
    iviLink::Ipc::CIpc* mIpc;
};

}

#endif //SYSTEM_CONTROLLER_WATCHDOG_PROXY_HPP