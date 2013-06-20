#ifndef SYSTEM_CONTROLLER_PROXY_HPP
#define SYSTEM_CONTROLLER_PROXY_HPP

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

class SystemControllerProxy : public iviLink::Ipc::ICallbackHandler
{
    const char* getName()
    {
        return "SystemControllerProxy";
    }

public:
    explicit SystemControllerProxy(std::string const& connectionName);
    virtual ~SystemControllerProxy();

protected:
    // incoming requests
    virtual void onPhysicalLayerStarted() = 0;
    virtual void onChannelLayerStarted() = 0;
    virtual void onProfileLayerStarted() = 0;
    virtual void onApplicationLayerStarted() = 0;
    virtual void onAuthenticationStarted() = 0;
    virtual void onAuthenticationStopped() = 0;

protected:
    // from ICallbackHandler
    virtual void OnConnection(iviLink::Ipc::DirectionID);
    virtual void OnConnectionLost(iviLink::Ipc::DirectionID);
    virtual void OnRequest(iviLink::Ipc::MsgID id, UInt8 const* pPayload, UInt32 payloadSize,
            UInt8* const pResponseBuffer, UInt32& bufferSize, iviLink::Ipc::DirectionID);
    virtual void OnAsyncRequest(iviLink::Ipc::MsgID id, UInt8 const* pPayload, UInt32 payloadSize,
            iviLink::Ipc::DirectionID);

    virtual void onSystemControllerNotAvaliable() = 0;

    CMsgIdGen mMsgIdGen;

    BaseError connectSystemController();

    bool isConnected() const;

    static Logger mLogger;
    iviLink::Ipc::CIpc* mIpc;
};

}

#endif //SYSTEM_CONTROLLER_WATCHDOG_PROXY_HPP