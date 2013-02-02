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

#include "PmpSessionProtocol.hpp"
#include "PmpProtocol.hpp"
#include "Serialize.hpp"
#include "byteOrder.hpp"
#include "EventFactory.hpp"

namespace iviLink
{
namespace PMP
{

PmpSessionProtocol::PmpSessionProtocol()
    : mLogger(Logger::getInstance(LOG4CPLUS_TEXT("profileManager.PMP.interaction.PmpSessionProtocol")))
    , mPmpProtocol(NULL)
    , mEventFactory(NULL)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
}

PmpSessionProtocol::~PmpSessionProtocol()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
}

void PmpSessionProtocol::initEventFactory(EventFactory * factory)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
    mEventFactory = factory;
}

void PmpSessionProtocol::initPmpProtocol(PmpProtocol * pmpProtocol)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
    mPmpProtocol = pmpProtocol;
}

bool PmpSessionProtocol::sendNegotiateNumber(UInt32 number)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );

    assert(mPmpProtocol);

    UInt32 size = sizeof(PMPFrame) + sizeof(number);

    PMPFrame * pReq = reinterpret_cast<PMPFrame*>(new UInt8[size]);
    pReq->size = size;
    pReq->reqType = PMP_REQ_SESSION_NEGOTIATE_NUMBER;

    number = ByteOrder::hton32(number);
    memcpy(pReq->data,&number,sizeof(number));

    bool res = mPmpProtocol->makeSessionRequest(*pReq);
    delete [] pReq;
    return res;
}

void PmpSessionProtocol::onGotNegotiateNuber(PMPFrame * frame)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
    assert(mEventFactory);

    if (!frame)
    {
        LOG4CPLUS_ERROR(mLogger, "No frame");
        return;
    }

    UInt32 number;
    memcpy(&number,frame->data,sizeof(number));
    number = ByteOrder::ntoh32(number);

    mEventFactory->negotiateRole(number);
}

bool PmpSessionProtocol::sendWaitingServicesList(const std::list<Service::Uid> & services)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );

    assert(mPmpProtocol);

    UInt32 count = services.size();
    UInt32 size = sizeof(PMPFrame) + sizeof(count);
    for (std::list<Service::Uid>::const_iterator it = services.begin();
    services.end() != it; ++it)
    {
        size += stringInBufSize(it->value());
    }

    PMPFrame * pReq = reinterpret_cast<PMPFrame*>(new UInt8[size]);
    pReq->size = size;
    pReq->reqType = PMP_REQ_SESSION_SEND_WAITING_SERVICES;

    LOG4CPLUS_INFO(mLogger, "count: " + convertIntegerToString(count));
    count = ByteOrder::hton32(count);
    memcpy(pReq->data, &count, sizeof(count));
    UInt32 pos = sizeof(count);
    for (std::list<Profile::Uid>::const_iterator it = services.begin();
            services.end() != it; ++it)
    {
        stringToBuffer(it->value(),pReq->data + pos,true);
        pos += stringInBufSize(it->value());
    }

    assert(pos == size-sizeof(PMPFrame));
    bool res = mPmpProtocol->makeSessionRequest(*pReq);
    delete [] pReq;
    return res;
}

void PmpSessionProtocol::onReceivedServicesList(PMPFrame * frame)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
    assert(mEventFactory);

    if (!frame)
    {
        LOG4CPLUS_ERROR(mLogger, "No frame");
        return;
    }

    UInt32 count;
    memcpy(&count,frame->data,sizeof(count));
    count = ByteOrder::ntoh32(count);
    int pos = sizeof(count);
    LOG4CPLUS_INFO(mLogger, "RES COUNT : " + convertIntegerToString(count));
    std::list<Service::Uid> services;

    for (UInt32 i = 0; i<count; ++i)
    {
        std::string str = bufferToString(frame->data+pos,true);
        services.push_back(Service::Uid(str));
        pos += stringInBufSize(str);
    }
    mEventFactory->receiveRemoteWaitingServiceList(services);
}

} /* namespace PMP */
} /* namespace iviLink */
