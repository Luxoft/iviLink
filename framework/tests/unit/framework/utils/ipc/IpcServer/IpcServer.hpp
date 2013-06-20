#include <vector>

#include "ICallbackHandler.hpp"
#include "ipc_common.hpp"
#include "CIpc.hpp"

#include "Logger.hpp"
#include "CSignalSemaphore.hpp"
#include "CommonMessage.hpp"


namespace iviLink
{
namespace Test
{

class IpcServer : public iviLink::Ipc::ICallbackHandler
{

public:
    explicit IpcServer(const std::string& connectionName, int clientsNumber);
    virtual ~IpcServer();

    void testCaseShutdownOnRequest();

protected:
    // from ICallbackHandler
    virtual void OnConnection(iviLink::Ipc::DirectionID);
    virtual void OnConnectionLost(iviLink::Ipc::DirectionID);
    virtual void OnRequest(iviLink::Ipc::MsgID id, UInt8 const* pPayload, UInt32 payloadSize,
            UInt8* const pResponseBuffer, UInt32& bufferSize, iviLink::Ipc::DirectionID);
    virtual void OnAsyncRequest(iviLink::Ipc::MsgID id, UInt8 const* pPayload, UInt32 payloadSize,
            iviLink::Ipc::DirectionID);

    void sendPing(iviLink::Ipc::DirectionID * pDirID = NULL);
    void sendKill(iviLink::Ipc::DirectionID * pDirID = NULL);
    
    CMsgIdGen mMsgIdGen;
    static Logger mLogger;
    iviLink::Ipc::CIpc* mIpc;

    CSignalSemaphore mConnectionSema;

    int mClientsNumber;
    std::vector<iviLink::Ipc::DirectionID> mClients;
};
}
}