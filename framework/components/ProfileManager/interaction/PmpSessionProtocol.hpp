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


#ifndef PMPSESSIONPROTOCOL_HPP_
#define PMPSESSIONPROTOCOL_HPP_

#include <list>

#include "Logger.hpp"
#include "UID.hpp"

namespace iviLink
{
namespace PMP
{

    class PmpProtocol;
    class EventFactory;
    struct PMPFrame;

class PmpSessionProtocol
{
public:
    PmpSessionProtocol();
    ~PmpSessionProtocol();

    void initEventFactory(EventFactory * factory);
    void initPmpProtocol(PmpProtocol * pmpProtocol);

    bool sendNegotiateNumber(UInt32 number);
    void onGotNegotiateNuber(PMPFrame * frame);

    bool sendWaitingServicesList(const std::list<Service::Uid> & services);
    void onReceivedServicesList(PMPFrame * frame);

private:
    Logger mLogger;
    PmpProtocol * mPmpProtocol;
    EventFactory * mEventFactory;
};

} /* namespace PMP */
} /* namespace iviLink */
#endif /* PMPSESSIONPROTOCOL_HPP_ */
