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

#include "Request.hpp"
#include "Response.hpp"
#include "CIpc.hpp"
#include "CSError.hpp"
#include "NegotiatorClient.hpp"

#include "NegotiatorConstants.hpp"

#define RECONNECT_SLEEP_MS 50
#define MAX_RECONNECT_ATTEMPTS 50

char const * gpNEGOTIATOR_IPC_ADDR = NULL;

namespace
{
const char * getNegotiatorAddress()
{
    if (gpNEGOTIATOR_IPC_ADDR == NULL)
    {
        return IPC_NEGOTIATOR_ADDRESS;
    }
    return gpNEGOTIATOR_IPC_ADDR;
}
}

using namespace iviLink::ChannelSupervisor;
using namespace iviLink::ChannelSupervisor::Messages;
using namespace iviLink::Ipc;

Logger NegotiatorClient::mLogger = Logger::getInstance(
                LOG4CPLUS_TEXT("ChannelSupervisor.NegotiatorClient"));

NegotiatorClient::NegotiatorClient()
: mIpc(getNegotiatorAddress(), *this)
, mMessageId(1)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
}

BaseError NegotiatorClient::negotiateChannel(std::string const& tag, UInt32 & channelID)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    return communicateWithNegotiator(Request::REQUESTTYPE_ALLOCATE_CHANNEL, tag, channelID);
}

BaseError NegotiatorClient::updateMapWithCID(std::string const& tag, UInt32 const& channelID)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    UInt32 tempID = channelID;
    return communicateWithNegotiator(Request::REQUESTTYPE_CA_ALLOCATION_DONE, tag, tempID);
}

BaseError NegotiatorClient::freeChannel(UInt32 const& channelID)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    UInt32 tempID = channelID;
    // no waiting for response here
    return sendMessageToNegotiator(new Request("", channelID, Request::REQUESTTYPE_DEALLOCATE_CHANNEL)
        , getNextMsgId());
}

BaseError NegotiatorClient::updateChannelInfo(std::string const& tag, UInt32 const& channelID)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    UInt32 tempID = channelID;
    return communicateWithNegotiator(Request::REQUESTTYPE_UPDATE_CHANNEL_INFO, tag, tempID);
}

BaseError NegotiatorClient::communicateWithNegotiator(Request::RequestType requestType, 
                                                      std::string const& tag, 
                                                      UInt32& channelID)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    MsgID newId = getNextMsgId();
    if (!sendMessageToNegotiator(new Request(tag.c_str(), channelID, requestType), newId).isNoError())
    {
        LOG4CPLUS_TRACE(mLogger, "could not send data to negotiator");
        return CSError(CSError::IPC_NO_DATA_ERROR, "no data received from negotiator ipc server");
    }
    Response * response = waitForResponse(newId);
    if (response == NULL)
    {
        LOG4CPLUS_ERROR(mLogger, "negotiator response timeout");
        return CSError(CSError::ERROR_TIMEOUT, "response timeout");
    }
    ErrorCode errc;
    const char * errstr;
    response->getError(errc, errstr);
    channelID = response->getChannelID();
    delete response;
    if (errc == ERRORCODE_SUCCESS)
    {
        LOG4CPLUS_INFO(mLogger, "successful operation with channel " + tag 
            + " id = " + convertIntegerToString(channelID));
        return BaseError::NoError();
    }
    else // TODO parse errors correctly
    {
        channelID = 0;
        LOG4CPLUS_ERROR(mLogger, "Error occurred. Err string: " + (std::string) errstr);
        return CSError(CSError::ERROR_OTHER, errstr);
    }
}

void NegotiatorClient::OnAsyncRequest(iviLink::Ipc::MsgID id, UInt8 const* pPayload,
                UInt32 payloadSize, iviLink::Ipc::DirectionID dirId)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    pugi::xml_document * doc = new pugi::xml_document;
    doc->load((const char *) pPayload);
    Response * response = new Response(doc);
    LOG4CPLUS_INFO(mLogger, "Got response: \n" + response->toString());
    signalResponseSemaphore(response, id);
}

void NegotiatorClient::signalResponseSemaphore(Messages::Response* response,
                iviLink::Ipc::MsgID const& msgId)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    NegotiatorResponseData * data = getDataForMessageId(msgId);
    data->mResponse = response;
    data->mSema->signal();
}

MsgID NegotiatorClient::getNextMsgId()
{
    MsgID result;
    mMessageIdMutex.lock();
    result = mMessageId;
    mMessageId++;
    mMessageIdMutex.unlock();
    return result;
}

bool NegotiatorClient::ensureConnection()
{
    static CMutex connectionMutex;
    MutexLocker lock(connectionMutex);
    if (mIpc.isConnected())
    {
        return true;
    }
    for (int tryNum = 1; tryNum <= MAX_RECONNECT_ATTEMPTS; ++tryNum)
    {
        if (mIpc.connect().isNoError())
        {
            return true;
        }
        usleep(RECONNECT_SLEEP_MS * 1000);
    }
    return false;
}

Response* NegotiatorClient::waitForResponse(iviLink::Ipc::MsgID const& messageId)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    Response * result = NULL;
    NegotiatorResponseData * responseData = getDataForMessageId(messageId);
    int waitResult = responseData->mSema->waitTimeout(NEGOTIATOR_RESP_TIMEOUT_MS);
    result = responseData->mResponse;
    eraseDataForMessageId(messageId);
    if (waitResult == -1)
    {
        LOG4CPLUS_ERROR(mLogger, "response from negotiator timed out!");
        sendMessageToNegotiator(new Request("", 0, Request::REQUESTTYPE_INVALIDATE_TIMEDOUT_REQUEST),
            messageId);
        return NULL;
    }
    return result;
}

NegotiatorResponseData * NegotiatorClient::getDataForMessageId(iviLink::Ipc::MsgID const& messageId)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    MutexLocker lock(mResponsesMapMutex);
    tResponsesMap::iterator iter = mResponsesMap.find(messageId);
    if (iter != mResponsesMap.end())
    {
        LOG4CPLUS_INFO(mLogger, "Found data for key: " + convertIntegerToString(messageId));
        return iter->second;
    }
    else
    {
        LOG4CPLUS_INFO(mLogger,
                        "Not found for key: " + convertIntegerToString(messageId) + ", creating!");
        NegotiatorResponseData * newData = new NegotiatorResponseData();
        mResponsesMap[messageId] = newData;
        return newData;
    }
}

void NegotiatorClient::eraseDataForMessageId(iviLink::Ipc::MsgID const& messageId)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    MutexLocker lock(mResponsesMapMutex);
    tResponsesMap::iterator iter = mResponsesMap.find(messageId);
    if (iter != mResponsesMap.end())
    {
        LOG4CPLUS_INFO(mLogger, "erasing data for key: " + convertIntegerToString(messageId));
        delete iter->second;
        mResponsesMap.erase(iter);
    }
}

BaseError NegotiatorClient::sendMessageToNegotiator(Messages::Request * message,
                iviLink::Ipc::MsgID const& messageId)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    if (!ensureConnection())
    {
        return CSError(CSError::IPC_NO_DATA_ERROR, "no connection");
    }
    std::string serializedMessage = message->toString();
    LOG4CPLUS_INFO(mLogger, "Sending to negotiator: \n" + serializedMessage);
    UInt8 dataToSend[serializedMessage.length()];
    memcpy(dataToSend, serializedMessage.c_str(), sizeof(dataToSend));
    return mIpc.asyncRequest(messageId, dataToSend, sizeof(dataToSend));
}
