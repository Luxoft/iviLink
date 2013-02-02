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


#ifndef CPMPPIMPROTOCOL_HPP_
#define CPMPPIMPROTOCOL_HPP_

#include "Logger.hpp"
#include "UID.hpp"

namespace iviLink
{

namespace PMP
{
class PmpProtocol;
class EventFactory;
struct PMPFrame;

class PmpPimProtocol
{
public:
    /**
    * Constructor
    */
    PmpPimProtocol();

    void initEventFactory(EventFactory * eventFactory);

    void initPmpProtocol(PmpProtocol * pPmpProtocol);

    /**
    * Virtual Destructor
    */
    virtual ~PmpPimProtocol();

    /**
    *
    * @param profileUid
    * @param piuid
    * @param sid
    * @return
    */
    bool createProfile(iviLink::Profile::Uid const& profileUid,
            iviLink::Profile::IUid const& piuid,
            iviLink::Service::Uid const& sid);

    /**
    *
    * @param piuid
    * @param sid
    * @return
    */
    bool profileDied(iviLink::Profile::IUid const& piuid,
            iviLink::Service::Uid const& sid);

    void onCreateProfile(PMPFrame * pFrame);
    void onProfileDied(PMPFrame * pFrame);

private:
    PmpProtocol * mPmpProtocol; ///< Poiner to PMP Protocol class
    EventFactory * mEventFactory;
};

}

}

#endif /* CPMPPIMPROTOCOL_HPP_ */
