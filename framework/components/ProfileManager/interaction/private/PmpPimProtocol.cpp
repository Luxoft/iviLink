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

#include "PmpPimProtocol.hpp"
#include "PmpProtocol.hpp"
#include "Serialize.hpp"
#include "EventFactory.hpp"

namespace iviLink
{

namespace PMP
{

PmpPimProtocol::PmpPimProtocol()
    : mPmpProtocol(NULL)
    , mEventFactory(NULL)
{
}

void PmpPimProtocol::initEventFactory(EventFactory * eventFactory)
{
    mEventFactory = eventFactory;
}

void PmpPimProtocol::initPmpProtocol(PmpProtocol * pPmpProtocol)
{
    mPmpProtocol = pPmpProtocol;
}

PmpPimProtocol::~PmpPimProtocol()
{
}

bool PmpPimProtocol::createProfile(iviLink::Profile::Uid const& profileUid,
        iviLink::Profile::IUid const& piuid,
        iviLink::Service::Uid const& sid)
{
    UInt32 size = sizeof(PMPFrame);
    size += stringInBufSize(profileUid.value());
    size += stringInBufSize(piuid.value());
    size += stringInBufSize(sid.value());

    UInt8 * pBuf = new UInt8[size];
    PMPFrame * pReq = reinterpret_cast<PMPFrame*>(pBuf);
    pReq->size = size;
    pReq->reqType = PMP_REQ_PIM_CREATE_PROFILE;

    stringToBuffer(profileUid.value(),pReq->data,true);
    unsigned int pos = stringInBufSize(profileUid.value());
    stringToBuffer(piuid.value(),pReq->data + pos,true);
    pos += stringInBufSize(piuid.value());
    stringToBuffer(sid.value(),pReq->data + pos,true);

    bool res = mPmpProtocol->makePimRequest(*pReq);
    delete [] pReq;

    return res;
}

bool PmpPimProtocol::profileDied(iviLink::Profile::IUid const& piuid,
        iviLink::Service::Uid const& sid)
{
    UInt32 size = sizeof(PMPFrame);
    size += stringInBufSize(piuid.value());
    size += stringInBufSize(sid.value());

    UInt8 * pBuf = new UInt8[size];
    PMPFrame * pReq = reinterpret_cast<PMPFrame*>(pBuf);
    pReq->size = size;
    pReq->reqType = PMP_REQ_PIM_PROFILE_DIED;

    stringToBuffer(piuid.value(),pReq->data,true);
    unsigned int pos = stringInBufSize(piuid.value());
    stringToBuffer(sid.value(),pReq->data + pos,true);

    bool res = mPmpProtocol->makePimRequest(*pReq);
    delete [] pReq;

    return res;
}

void PmpPimProtocol::onCreateProfile(PMPFrame * pFrame)
{
    assert(pFrame);
    assert(mEventFactory);

    std::string str = bufferToString(pFrame->data,true);
    unsigned int pos = stringInBufSize(str);
    iviLink::Profile::Uid profileUid(str);
    str = bufferToString(pFrame->data + pos,true);
    pos += stringInBufSize(str);
    iviLink::Profile::IUid piuid(str);
    str = bufferToString(pFrame->data + pos,true);
    iviLink::Service::Uid sid(str);

    mEventFactory->pimProtocolCreateProfile(profileUid, piuid, sid);
}

void PmpPimProtocol::onProfileDied(PMPFrame * pFrame)
{
    assert(pFrame);
    assert(mEventFactory);

    std::string str = bufferToString(pFrame->data,true);
    unsigned int pos = stringInBufSize(str);
    iviLink::Profile::IUid piuid(str);
    str = bufferToString(pFrame->data + pos,true);
    iviLink::Service::Uid sid(str);

    mEventFactory->pimProtocolProfileDied(piuid, sid);
}

}

}
