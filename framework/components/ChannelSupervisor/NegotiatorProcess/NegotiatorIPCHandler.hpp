/**
 * Copyright placeholder
 */
/**
 * @ingroup             Channel Supervisor
 * @author              Elena Bratanova <ebratanova@luxoft.com>
 * @date                23.04.2013
 *
 * Communicates with negotiator clients.
 */

#ifndef __NEGOTIATOR_IPC_HANDLER__
#define __NEGOTIATOR_IPC_HANDLER__

#include <deque>

#include "ICallbackHandler.hpp"
#include "CIpc.hpp"
#include "CThread.hpp"
#include "NegotiatorTube.hpp"
#include "Processor.hpp"

namespace iviLink
{
namespace ChannelSupervisor
{
class NegotiatorIPCHandler: public iviLink::Ipc::ICallbackHandler,
                public IResponseSender,
                public CThread
{
public:
    NegotiatorIPCHandler(std::string const& addr, NegotiatorTube * tube);
    virtual ~NegotiatorIPCHandler();

    // not used
    virtual void OnConnection(iviLink::Ipc::DirectionID dirId) {}
    /**
     * all local processors affiliated with this direction are erased, 
     * and a notification is sent to the other side
     */
    virtual void OnConnectionLost(iviLink::Ipc::DirectionID dirId);
    // does nothing
    virtual void OnRequest(iviLink::Ipc::MsgID id, UInt8 const* pPayload, UInt32 payloadSize,
                    UInt8* const pResponseBuffer, UInt32& bufferSize,
                    iviLink::Ipc::DirectionID dirId);

    virtual void OnAsyncRequest(iviLink::Ipc::MsgID id, UInt8 const* pPayload, UInt32 payloadSize,
                    iviLink::Ipc::DirectionID dirId);

    // sends IpcResponses asynchronously
    virtual void threadFunc();

    // enqueues a response to be sent
    virtual void sendResponse(ResponseContainer);
private:
    CMutex mResponseQueueMutex;
    std::deque<ResponseContainer> mResponseQueue;
    CSignalSemaphore mResponseQueueSema;

    NegotiatorTube * mTube;
    iviLink::Ipc::CIpc mIpc;

    static Logger mLogger;
};

}
}

#endif // __NEGOTIATOR_IPC_HANDLER__
