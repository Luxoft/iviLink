/**
 *
 */

/**
 * @ingroup iOS Port
 * @author Bratanova Elena <ebratanova@luxoft.com>
 * @date 21.03.2013
 */

#include <cassert>
#include <set>

#include "FatalErrorHandler.h"

#include "PmpCommunicator.hpp"
#include "ChannelSupervisorTube.hpp"
#include "ByteOrder.hpp"
#include "Serialize.hpp"
#include "Database.hpp"

#include "SystemControllerStub.hpp"

#define CHANNEL_TAG "ProfileManagerPProtocol"

namespace iviLink
{
namespace PMP
{
Logger PmpCommunicator::mLogger = Logger::getInstance("iviLink::PMP::iOS::PmpCommunicator");

PmpCommunicator::PmpCommunicator()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    mCore = PmpCore::getInstance();
    mPIM = PmpPIM::getInstance();
    mChannelID = 0;
    mProcessingPool = dispatch_queue_create("PmpCommunicator receive", DISPATCH_QUEUE_CONCURRENT);
    mSendingQueue = dispatch_queue_create("PmpCommunicator send", NULL);
}

PmpCommunicator::~PmpCommunicator()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    dispatch_release(mProcessingPool);
    dispatch_release(mSendingQueue);
}

PmpCommunicator * PmpCommunicator::getInstance()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    static PmpCommunicator * instance;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^
            {
                LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
                instance = new PmpCommunicator;
            });
    return instance;
}

void PmpCommunicator::startCommunication()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    if (!ChannelSupervisor::allocateChannel(this, CHANNEL_TAG, mChannelID, eSafetyCritical).isNoError())
    {
        showDialogAndDie("Sorry! Could not establish Profile Manager communication");
    } else
    {
        sendReady();
    }
}

void PmpCommunicator::onDataReceived(const UInt32 channelID, const UInt32 readSize)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    UInt8 * data = new UInt8[readSize];
    UInt32 recSize = 0;
    assert(ChannelSupervisor::receiveData(channelID, data, recSize, readSize).isNoError());
    PMPFrame * pFrame = reinterpret_cast<PMPFrame*>(data);
    pFrame->id = ByteOrder::ntoh32(pFrame->id);
    pFrame->size = ByteOrder::ntoh32(pFrame->size);
    onRequest(pFrame);
}

void PmpCommunicator::onBufferOverflow(const UInt32 channelID)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    assert(false);
}

void PmpCommunicator::onChannelDeleted(const UInt32 channelID)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    assert(false);
}

void PmpCommunicator::onConnectionLost()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    assert(false);
}

void PmpCommunicator::onRequest(PMPFrame * frame)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    switch (frame->client)
    {
    case PMP_CLIENT_CORE:
        LOG4CPLUS_INFO(mLogger, "PMP_CLIENT_CORE");
        dispatch_async(mProcessingPool, ^
                {
                    mCore->processFrame(frame);
                    delete[] frame;
                });
        break;
    case PMP_CLIENT_CMP_MGR:
        LOG4CPLUS_INFO(mLogger, "PMP_CLIENT_CMP_MGR");
        dispatch_async(mProcessingPool, ^
                {
                    onCmpMgrRequest(frame);
                    delete[] frame;
                });
        break;
    case PMP_CLIENT_PI_SRV:
        LOG4CPLUS_INFO(mLogger, "PMP_CLIENT_PI_SRV");
        dispatch_async(mProcessingPool, ^
                {
                    mPIM->processFrame(frame);
                    delete[] frame;
                });
        break;
    case PMP_CLIENT_SESSION:
        LOG4CPLUS_INFO(mLogger, "PMP_CLIENT_SESSION");
        dispatch_async(mProcessingPool, ^
                {
                    mSessionManager.processFrame(frame);
                    delete[] frame;
                });
        break;
    default:
        assert(false);
        break;
    }

}

void PmpCommunicator::onCmpMgrRequest(PMPFrame * frame)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    if (frame->reqType != PMP_REQ_CMP_MGR_READY)
    {
        LOG4CPLUS_ERROR(mLogger, "Unknown request for component manager");
        assert(false);
    }
    requestAvailableComplements();
    mSessionManager.onSendNegotiateNumber();
}

void PmpCommunicator::requestAvailableComplements()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    int size = sizeof(PMPFrame);
    PMPFrame * complements = reinterpret_cast<PMPFrame*>(new UInt8[size]);
    complements->reqType = PMP_REQ_CORE_GET_COMPLEMENTS_REQUEST;
    complements->client = PMP_CLIENT_CORE;
    complements->size = size;
    sendFrame(complements);
}

void PmpCommunicator::sendReady()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    int size = sizeof(PMPFrame);
    PMPFrame * ready = reinterpret_cast<PMPFrame*>(new UInt8[size]);
    ready->reqType = PMP_REQ_CMP_MGR_READY;
    ready->client = PMP_CLIENT_CMP_MGR;
    ready->size = size;
    sendFrame(ready);
}

void PmpCommunicator::sendFrame(iviLink::PMP::PMPFrame * frame)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
dispatch_async(mSendingQueue, ^
        {
            frame->id = ByteOrder::hton32(frame->id);
            UInt32 size = frame->size;
            frame->size = ByteOrder::hton32(frame->size);
            if (mChannelID != 0)
            {
                ChannelSupervisor::sendData(mChannelID, reinterpret_cast<UInt8*>(frame), size);
            }
            else
            {
                LOG4CPLUS_WARN(mLogger, "Link is not alive, skipping a frame");
            }
            delete[] frame;
        });
}

void PmpCommunicator::unlockAll()
{
LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
mCore->unlockAll();
mCore->waitForAllUnlocked();
}

void PmpCommunicator::resetState()
{
LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
mChannelID = 0;
mCore->lockAll();
}

void PmpCommunicator::unlockAuthenticationProfile()
{
LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
mCore->unlockAuthenticationProfile();
mCore->waitForAuthUnlocked();
}
}
}
