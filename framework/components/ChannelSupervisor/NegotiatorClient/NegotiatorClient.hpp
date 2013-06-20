/**
 * Copyright placeholder
 */
/**
 * @ingroup             Channel Supervisor
 * @author              Elena Bratanova <ebratanova@luxoft.com>
 * @date                23.04.2013
 *
 * Communicates with the Negotiator Process.
 */

#ifndef NEGOTIATOR_CLIENT_HPP
#define NEGOTIATOR_CLIENT_HPP

#include <string>
#include <map>

#include "ICallbackHandler.hpp"
#include "INegotiatorClient.hpp"

#include "CMutex.hpp"
#include "Types.hpp"
#include "Logger.hpp"
#include "CIpc.hpp"
#include "Request.hpp"
#include "NegotiatorResponseData.hpp"

namespace iviLink
{
namespace ChannelSupervisor
{
class NegotiatorClientDeleter;

typedef std::map<iviLink::Ipc::MsgID, NegotiatorResponseData*> tResponsesMap;

class NegotiatorClient: public iviLink::Ipc::ICallbackHandler, public INegotiatorClient
{
    friend class NegotiatorClientDeleter;
    friend class NegotiatorClientHolder;
public:
    // INegotiatorClient interface
    virtual BaseError negotiateChannel(std::string const& tag, UInt32 & channelID);
    virtual BaseError updateMapWithCID(std::string const& tag, UInt32 const& channelID);
    virtual BaseError freeChannel(UInt32 const& channelID);
    virtual BaseError updateChannelInfo(std::string const& tag, UInt32 const& channelID);

    // ICallbackHandler interface
    virtual void OnConnection(iviLink::Ipc::DirectionID dirId) {}
    virtual void OnConnectionLost(iviLink::Ipc::DirectionID dirId) {}
    virtual void OnRequest(iviLink::Ipc::MsgID id, UInt8 const* pPayload, UInt32 payloadSize,
                    UInt8* const pResponseBuffer, UInt32& bufferSize,
                    iviLink::Ipc::DirectionID dirId) {}
    virtual void OnAsyncRequest(iviLink::Ipc::MsgID id, UInt8 const* pPayload, UInt32 payloadSize,
                    iviLink::Ipc::DirectionID dirId);
private:
    void signalResponseSemaphore(Messages::Response*, iviLink::Ipc::MsgID const&);
    Messages::Response* waitForResponse(iviLink::Ipc::MsgID const&);
    NegotiatorResponseData * getDataForMessageId(iviLink::Ipc::MsgID const&);
    void eraseDataForMessageId(iviLink::Ipc::MsgID const&);

    bool ensureConnection();
    iviLink::Ipc::MsgID getNextMsgId();

    /**
     * Note: request will be deallocated inside the method.
     */
    BaseError sendMessageToNegotiator(Messages::Request*, iviLink::Ipc::MsgID const&);

    BaseError communicateWithNegotiator(Messages::Request::RequestType, std::string const&tag, UInt32& channelID);

private:
    NegotiatorClient();
    NegotiatorClient(NegotiatorClient const&);
    void operator =(NegotiatorClient const&);
    virtual ~NegotiatorClient() {}

private:
    iviLink::Ipc::CIpc mIpc;
    CMutex mResponsesMapMutex;
    tResponsesMap mResponsesMap;

    iviLink::Ipc::MsgID mMessageId;
    CMutex mMessageIdMutex;

    static Logger mLogger;
};

}  // namespace ChannelSupervisor
}  // namespace AXIS

#endif //NEGOTIATORCLIENT_HPP
