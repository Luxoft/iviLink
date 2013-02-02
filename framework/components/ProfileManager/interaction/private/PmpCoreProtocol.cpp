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

#include "PmpCoreProtocol.hpp"
#include "PmpProtocol.hpp"
#include "Serialize.hpp"
#include "byteOrder.hpp"
#include "EventFactory.hpp"

namespace iviLink
{

namespace PMP
{

Logger PmpCoreProtocol::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("profileManager.PMP.interaction.PmpCoreProtocol"));

PmpCoreProtocol::PmpCoreProtocol()
    : mPmpProtocol(NULL)
    , mEventFactory(NULL)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
}

PmpCoreProtocol::~PmpCoreProtocol()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
}

void PmpCoreProtocol::initEventFactory(EventFactory * factory)
{
    mEventFactory = factory;
}
void PmpCoreProtocol::initPmpProtocol(PmpProtocol * pmpProtocol)
{
    mPmpProtocol = pmpProtocol;
}

bool PmpCoreProtocol::getAvailableProfileComplementsRequest()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    assert(mPmpProtocol);
    PMPFrame req;
    req.size = sizeof(PMPFrame);
    req.reqType = PMP_REQ_CORE_GET_COMPLEMENTS_REQUEST;

    return mPmpProtocol->makeCoreRequest(req);
}

bool PmpCoreProtocol::getAvailableProfileComplementsResponse(const std::list<Profile::Uid> & complements)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    assert(mPmpProtocol);

    UInt32 size = sizeof(PMPFrame) + 4;
    for (std::list<Profile::Uid>::const_iterator it = complements.begin();
    complements.end() != it; ++it)
    {
        size += stringInBufSize(it->value());
    }

    PMPFrame * pReq = reinterpret_cast<PMPFrame*>(new UInt8[size]);
    pReq->size = size;
    pReq->reqType = PMP_REQ_CORE_GET_COMPLEMENTS_RESPONSE;

    UInt32 count = complements.size();
    LOG4CPLUS_INFO(msLogger, "count: " + convertIntegerToString(count));
    count = ByteOrder::hton32(count);
    memcpy(pReq->data,&count,4);
    UInt32 pos = 4;
    for (std::list<Profile::Uid>::const_iterator it = complements.begin();
    complements.end() != it; ++it)
    {
        stringToBuffer(it->value(),pReq->data + pos,true);
        pos += stringInBufSize(it->value());
    }

    assert(pos == size-sizeof(PMPFrame));
    bool res = mPmpProtocol->makeCoreRequest(*pReq);
    delete [] pReq;
    return res;
}

bool PmpCoreProtocol::reloadProfilesFromRepository()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    assert(mPmpProtocol);

    PMPFrame req;
    req.size = sizeof(PMPFrame);
    req.reqType = PMP_REQ_CORE_RELOAD_PROFS_FROM_REPO;

    return mPmpProtocol->makeCoreRequest(req);
}

bool PmpCoreProtocol::resetProfileState()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    assert(mPmpProtocol);

    PMPFrame req;
    req.size = sizeof(PMPFrame);
    req.reqType = PMP_REQ_CORE_RESET_PROF_STATE;

    return mPmpProtocol->makeCoreRequest(req);
}

bool PmpCoreProtocol::lock(const BaseUid & id)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    assert(mPmpProtocol);

    LOG4CPLUS_INFO(msLogger, "id: " + id.value());
    UInt32 reqSize = sizeof(PMPFrame);
    reqSize += stringInBufSize(id.value());
    PMPFrame * pReq = reinterpret_cast<PMPFrame*>(new UInt8[reqSize]);
    pReq->size = reqSize;
    pReq->reqType = PMP_REQ_CORE_LOCK;
    stringToBuffer(id.value(), pReq->data,true);

    bool res = mPmpProtocol->makeCoreRequest(*pReq);
    delete [] pReq;
    return res;
}

bool PmpCoreProtocol::unlock(const BaseUid & id)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    assert(mPmpProtocol);

    LOG4CPLUS_INFO(msLogger, "id: " + id.value());
    UInt32 reqSize = sizeof(PMPFrame);
    reqSize += stringInBufSize(id.value());
    PMPFrame * pReq = reinterpret_cast<PMPFrame*>(new UInt8[reqSize]);
    pReq->size = reqSize;
    pReq->reqType = PMP_REQ_CORE_UNLOCK;
    stringToBuffer(id.value(), pReq->data,true);

    bool res = mPmpProtocol->makeCoreRequest(*pReq);
    delete [] pReq;
    return res;
}

bool PmpCoreProtocol::lockAll()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    assert(mPmpProtocol);

    PMPFrame req;
    req.size = sizeof(PMPFrame);
    req.reqType = PMP_REQ_CORE_LOCK_ALL;

    return mPmpProtocol->makeCoreRequest(req);
}

bool PmpCoreProtocol::unlockAll()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    assert(mPmpProtocol);

    PMPFrame req;
    req.size = sizeof(PMPFrame);
    req.reqType = PMP_REQ_CORE_UNLOCK_ALL;

    return mPmpProtocol->makeCoreRequest(req);
}

bool PmpCoreProtocol::disableByClient(const BaseUid & id)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    assert(mPmpProtocol);

    LOG4CPLUS_INFO(msLogger, "id: " + id.value());
    UInt32 reqSize = sizeof(PMPFrame);
    reqSize += stringInBufSize(id.value());
    PMPFrame * pReq = reinterpret_cast<PMPFrame*>(new UInt8[reqSize]);
    pReq->size = reqSize;
    pReq->reqType = PMP_REQ_CORE_DIS_BY_CLIENT;
    stringToBuffer(id.value(),pReq->data,true);

    bool res = mPmpProtocol->makeCoreRequest(*pReq);
    delete [] pReq;
    return res;
}

bool PmpCoreProtocol::enableByClient(const BaseUid & id)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    assert(mPmpProtocol);

    LOG4CPLUS_INFO(msLogger, "id: " + id.value());
    UInt32 reqSize = sizeof(PMPFrame);
    reqSize += stringInBufSize(id.value());
    PMPFrame * pReq = reinterpret_cast<PMPFrame*>(new UInt8[reqSize]);
    pReq->size = reqSize;
    pReq->reqType = PMP_REQ_CORE_ENABLE_BY_CLNT;
    stringToBuffer(id.value(), pReq->data,true);

    bool res = mPmpProtocol->makeCoreRequest(*pReq);
    delete [] pReq;
    return res;
}

bool PmpCoreProtocol::enableByClientAll()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    assert(mPmpProtocol);

    PMPFrame req;
    req.size = sizeof(PMPFrame);
    req.reqType = PMP_REQ_CORE_ENABLE_BY_CLNT_ALL;

    return mPmpProtocol->makeCoreRequest(req);
}

void PmpCoreProtocol::onGetAvailableProfileComplementsRequest(PMPFrame *)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    assert(mEventFactory);

    mEventFactory->coreProtocolGetAvailableProfileComplementsRequest();
}

void PmpCoreProtocol::onGetAvailableProfileComplementsResponse(PMPFrame * frame)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    assert(mEventFactory);


    if (!frame)
    {
        LOG4CPLUS_ERROR(msLogger, "No frame");
        return;
    }

    UInt32 count;
    memcpy(&count,frame->data,4);
    count = ByteOrder::ntoh32(count);
    int pos = 4;
    LOG4CPLUS_INFO(msLogger, "RES COUNT : " + convertIntegerToString(count));
    std::list<Profile::Uid> complements;

    for (UInt32 i = 0; i<count; ++i)
    {
        std::string str = bufferToString(frame->data+pos,true);
        complements.push_back(Profile::Uid(str));
        pos += stringInBufSize(str);
    }

    mEventFactory->coreProtocolGetAvailableProfileComplementsResponse(complements);
}

void PmpCoreProtocol::onReloadProfilesFromRepository (PMPFrame *)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    assert(mEventFactory);

    mEventFactory->coreProtocolReloadProfilesFromRepository();
}

void PmpCoreProtocol::onResetProfilesState(PMPFrame *)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    assert(mEventFactory);

    mEventFactory->coreProtocolResetProfileState();
}

void PmpCoreProtocol::onLock(PMPFrame * frame)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    assert(frame);
    assert(mEventFactory);


    iviLink::BaseUid id(bufferToString(frame->data,true));
    mEventFactory->coreProtocolLock(id);
}

void PmpCoreProtocol::onUnlock(PMPFrame * frame)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    assert(mEventFactory);
    assert(frame);

    iviLink::BaseUid id(bufferToString(frame->data,true));
    mEventFactory->coreProtocolUnlock(id);
}

void PmpCoreProtocol::onLockAll(PMPFrame *)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    assert(mEventFactory);

    mEventFactory->coreProtocolLockAll();
}

void PmpCoreProtocol::onUnlockAll(PMPFrame *)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    mEventFactory->coreProtocolUnlockAll();
}

void PmpCoreProtocol::onDisableByClient(PMPFrame * frame)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    assert(mEventFactory);
    assert(frame);

    iviLink::BaseUid id(bufferToString(frame->data,true));
    mEventFactory->coreProtocolDisableByClient(id);
}

void PmpCoreProtocol::onEnableByClient(PMPFrame * frame)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    assert(mEventFactory);
    assert(frame);

    BaseUid id(bufferToString(frame->data,true));
    mEventFactory->coreProtocolEnableByClient(id);
}

void PmpCoreProtocol::onEnableByClientAll(PMPFrame * frame)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    assert(mEventFactory);

    mEventFactory->coreProtocolEnableByClientAll();
}

}

}
