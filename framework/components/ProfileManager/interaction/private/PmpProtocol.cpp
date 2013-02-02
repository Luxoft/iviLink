/* 
 * iviLINK SDK, version 1.2
 * http://www.ivilink.net
 * Cross Platform Application Communication Stack for In-Vehicle Applications
 * 
 * Copyright (C) 2012-2013, Luxoft Professional Corp., member of IBS group
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; version 2.1.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 * 
 */ 


#include <cassert>

#include "byteOrder.hpp"
#include "ChannelSupervisorTube.hpp"
#include "EventFactory.hpp"
#include "PmpProtocol.hpp"
#include "PmpCoreProtocol.hpp"
#include "PmpPimProtocol.hpp"
#include "PmpSessionProtocol.hpp"

namespace iviLink
{

namespace PMP
{

Logger PmpProtocol::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("profileManager.PMP.interaction.PmpProtocol"));

PmpProtocol::PmpProtocol()
    : mChannelId(0)
    , mTag("ProfileManagerPProtocol")
    , mGenId(0)
    , mEventFactory(NULL)
    , mReady(false)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
}

PmpProtocol::~PmpProtocol()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
}

void PmpProtocol::init(EventFactory * const eventFactory, PmpCoreProtocol * const coreProtocol,
        PmpPimProtocol * const pimProtocol, PmpSessionProtocol * const sessionProtocol)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    mEventFactory = eventFactory;
    mCoreProtocol = coreProtocol;
    mPimProtocol = pimProtocol;
    mSessionProtocol = sessionProtocol;

    assert(mEventFactory);
    assert(mCoreProtocol);
    assert(mPimProtocol);
    assert(mSessionProtocol);
}

void PmpProtocol::onChannelDeleted(const UInt32 channelId)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    LOG4CPLUS_INFO(msLogger, "channelId = " + convertIntegerToString(static_cast<int>(channelId)));

    assert(mEventFactory);
    mEventFactory->exceptionProtocolChannelDeleted();
}

void PmpProtocol::onConnectionLost()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    assert(mEventFactory);
    mEventFactory->exceptionProtocolConnectionLost();
}

bool PmpProtocol::connect()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    UInt32 chid = 0;
    BaseError err = iviLink::ChannelSupervisor::allocateChannel(this,mTag.c_str(), chid, eSafetyCritical);
    if (err.isNoError())
    {
        LOG4CPLUS_INFO(msLogger, "ok chid = " +
        convertIntegerToString(static_cast<int>(chid)));
        mChannelId = chid;
        return true;
    }
    else
    {
        LOG4CPLUS_ERROR(msLogger, static_cast<std::string>(err));
        return false;
    }
}

void PmpProtocol::disconnect()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    if (0 != mChannelId)
    {
        ChannelSupervisor::deallocateChannel(mChannelId);
        mChannelId = 0;
    }
}

void PmpProtocol::onDataReceived(const UInt32 channelId, const UInt32 readSize)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    LOG4CPLUS_INFO(msLogger, "onDataReceived: channel " + convertIntegerToString(channelId) 
        + " size " + convertIntegerToString(readSize));

    UInt8 * data = new UInt8[readSize];
    UInt32 recSize = 0;
    BaseError err =  iviLink::ChannelSupervisor::receiveData(channelId, data, recSize, readSize);
    if(!err.isNoError())
    {
        LOG4CPLUS_ERROR(msLogger, static_cast<std::string>(err));
        delete [] data;
        return;
    }

    PMPFrame * pFrame = reinterpret_cast<PMPFrame*>(data);

    pFrame->id = ByteOrder::ntoh32(pFrame->id);
    pFrame->size = ByteOrder::ntoh32(pFrame->size);

    LOG4CPLUS_INFO(msLogger, "id = " + convertIntegerToString(static_cast<int>(pFrame->id)) +
            " size = " + convertIntegerToString(static_cast<int>(pFrame->size)) +
            " client = " + convertIntegerToString(static_cast<int>(pFrame->client)) +
            " reqType = " + convertIntegerToString(static_cast<int>(pFrame->reqType)));

    onRequest(pFrame);
    delete [] data;
}

void PmpProtocol::onRequest(PMPFrame * pFrame)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    switch(pFrame->client)
    {
    case PMP_CLIENT_CORE:
        onCoreRequest(pFrame);
        break;
    case PMP_CLIENT_PI_SRV:
        onPimRequest(pFrame);
        break;
    case PMP_CLIENT_CMP_MGR:
        onCmpMgrRequest(pFrame);
        break;
    case PMP_CLIENT_SESSION:
        onSessionRequest(pFrame);
        break;
    default:
        LOG4CPLUS_WARN(msLogger, "undefined client");
        break;
    }
}

void PmpProtocol::onCoreRequest(PMPFrame * pFrame)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    if (!mCoreProtocol)
    {
        LOG4CPLUS_WARN(msLogger, "no callback for request");
        return;
    }

    switch (pFrame->reqType)
    {
    case PMP_REQ_CORE_GET_COMPLEMENTS_REQUEST:
        LOG4CPLUS_INFO(msLogger, "PMP_REQ_CORE_GET_COMPLEMENTS");
        mCoreProtocol->onGetAvailableProfileComplementsRequest(pFrame);
        break;
    case PMP_REQ_CORE_GET_COMPLEMENTS_RESPONSE:
        LOG4CPLUS_INFO(msLogger, "PMP_REQ_CORE_GET_COMPLEMENTS");
        mCoreProtocol->onGetAvailableProfileComplementsResponse(pFrame);
        break;
    case PMP_REQ_CORE_RELOAD_PROFS_FROM_REPO:
        LOG4CPLUS_INFO(msLogger, "PMP_REQ_CORE_RELOAD_PROFS_FROM_REPO");
        mCoreProtocol->onReloadProfilesFromRepository(pFrame);
        break;
    case PMP_REQ_CORE_RESET_PROF_STATE:
        LOG4CPLUS_INFO(msLogger, "PMP_REQ_CORE_RESET_PROF_STATE");
        mCoreProtocol->onResetProfilesState(pFrame);
        break;
    case PMP_REQ_CORE_LOCK:
        LOG4CPLUS_INFO(msLogger, "PMP_REQ_CORE_LOCK");
        mCoreProtocol->onLock(pFrame);
        break;
    case PMP_REQ_CORE_UNLOCK:
        LOG4CPLUS_INFO(msLogger, "PMP_REQ_CORE_UNLOCK");
        mCoreProtocol->onUnlock(pFrame);
        break;
    case PMP_REQ_CORE_LOCK_ALL:
        LOG4CPLUS_INFO(msLogger, "PMP_REQ_CORE_LOCK_ALL");
        mCoreProtocol->onLockAll(pFrame);
        break;
    case PMP_REQ_CORE_UNLOCK_ALL:
        LOG4CPLUS_INFO(msLogger, "PMP_REQ_CORE_UNLOCK_ALL");
        mCoreProtocol->onUnlockAll(pFrame);
        break;
    case PMP_REQ_CORE_DIS_BY_CLIENT:
        LOG4CPLUS_INFO(msLogger, "PMP_REQ_CORE_DIS_BY_CLIENT");
        mCoreProtocol->onDisableByClient(pFrame);
        break;
    case PMP_REQ_CORE_ENABLE_BY_CLNT:
        LOG4CPLUS_INFO(msLogger, "PMP_REQ_CORE_ENABLE_BY_CLNT");
        mCoreProtocol->onEnableByClient(pFrame);
        break;
    case PMP_REQ_CORE_ENABLE_BY_CLNT_ALL:
        LOG4CPLUS_INFO(msLogger, "PMP_REQ_CORE_ENABLE_BY_CLNT_ALL");
        mCoreProtocol->onEnableByClientAll(pFrame);
        break;
    default:
        LOG4CPLUS_WARN(msLogger, "Undefined core request");
        break;
    }
}

void PmpProtocol::onPimRequest(PMPFrame * pFrame)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    if (!mPimProtocol)
    {
        LOG4CPLUS_WARN(msLogger, "no callback for request");
        return;
    }

    switch (pFrame->reqType)
    {
    case PMP_REQ_PIM_CREATE_PROFILE:
        LOG4CPLUS_INFO(msLogger, "PMP_REQ_PIM_CREATE_PROFILE");
        mPimProtocol->onCreateProfile(pFrame);
        break;
    case PMP_REQ_PIM_PROFILE_DIED:
        LOG4CPLUS_INFO(msLogger, "PMP_REQ_PIM_PROFILE_DIED");
        mPimProtocol->onProfileDied(pFrame);
        break;
    default:
        LOG4CPLUS_WARN(msLogger, "Undefined PI request");
        break;
    }
}

void PmpProtocol::onSessionRequest(PMPFrame * pFrame)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    if (!mSessionProtocol)
    {
        LOG4CPLUS_WARN(msLogger, "no callback for request");
        return;
    }

    switch (pFrame->reqType)
    {
    case PMP_REQ_SESSION_NEGOTIATE_NUMBER:
        LOG4CPLUS_INFO(msLogger, "PMP_REQ_SESSION_NEGOTIATE_NUMBER");
        mSessionProtocol->onGotNegotiateNuber(pFrame);
        break;
    case PMP_REQ_SESSION_SEND_WAITING_SERVICES:
        LOG4CPLUS_INFO(msLogger, "PMP_REQ_SESSION_SEND_WAITING_SERVICES");
        mSessionProtocol->onReceivedServicesList(pFrame);
        break;
    default:
        LOG4CPLUS_WARN(msLogger, "Undefined session request");
        break;
    }
}

void PmpProtocol::onCmpMgrRequest(PMPFrame * pFrame)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    switch (pFrame->reqType)
    {
        case PMP_REQ_CMP_MGR_READY:
        LOG4CPLUS_INFO(msLogger, "PMP_REQ_CMP_MGR_READY");
        mReady = true;
        mEventFactory->pmpReady();
        break;
        default:
        LOG4CPLUS_WARN(msLogger, "Undefined CMP MGR request");
        break;
    }
}

void PmpProtocol::onBufferOverflow(const UInt32 channelId)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
}

bool PmpProtocol::makeCoreRequest(PMPFrame & request)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    request.client = PMP_CLIENT_CORE;
    request.id = generateId();
    return makeRequest(request);
}

bool PmpProtocol::makePimRequest(PMPFrame & request)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    request.client = PMP_CLIENT_PI_SRV;
    request.id = generateId();
    return makeRequest(request);
}

bool PmpProtocol::makeCmpMgrRequest(PMPFrame & request)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    request.client = PMP_CLIENT_CMP_MGR;
    request.id = generateId();
    return makeRequest(request);
}

bool PmpProtocol::makeSessionRequest(PMPFrame & request)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    request.client = PMP_CLIENT_SESSION;
    request.id = generateId();
    return makeRequest(request);
}

bool PmpProtocol::makeRequest(PMPFrame & requestFrame)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    LOG4CPLUS_INFO(msLogger, "id = " + convertIntegerToString(static_cast<int>(requestFrame.id)) +
            " size = " + convertIntegerToString(static_cast<int>(requestFrame.size)) +
            " client = " + convertIntegerToString(static_cast<int>(requestFrame.client)) +
            " reqType = " + convertIntegerToString(static_cast<int>(requestFrame.reqType)));

    requestFrame.id = ByteOrder::hton32(requestFrame.id);
    unsigned int size = requestFrame.size;
    requestFrame.size = ByteOrder::hton32(requestFrame.size);
    BaseError err = ChannelSupervisor::sendData(mChannelId, reinterpret_cast<unsigned char *>(&requestFrame), size);
    return err.isNoError();
}

MessageId PmpProtocol::generateId ()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    MessageId res = mGenId++;
    return res;
}

bool PmpProtocol::ready()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    PMPFrame req;
    req.size = sizeof(PMPFrame);
    req.reqType = PMP_REQ_CMP_MGR_READY;

    return makeCmpMgrRequest(req);
}

bool PmpProtocol::isReady() const
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    return mReady;
}

}

}
