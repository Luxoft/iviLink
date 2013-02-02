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


#ifndef CPMPCOREPROTOCOL_HPP_
#define CPMPCOREPROTOCOL_HPP_

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

/**
* Class is used for make requests on another size
*/
class PmpCoreProtocol
{
public:
    /**
    * Constructor
    * @param pPmpProtocol is pointer to object thar implements PMP Protocol interface
    */
    PmpCoreProtocol();
    void initEventFactory(EventFactory * factory);
    void initPmpProtocol(PmpProtocol * pmpProtocol);

    /**
    * Destructor
    */
    virtual ~PmpCoreProtocol();

    /**
    * Functions below makes requests on another side, serializes data and waits for responce
    */
    bool getAvailableProfileComplementsRequest();
    bool getAvailableProfileComplementsResponse(const std::list<Profile::Uid> & complements);
    bool reloadProfilesFromRepository();
    bool resetProfileState();
    bool lock(const BaseUid & id);
    bool unlock(const BaseUid & id);
    bool lockAll();
    bool unlockAll();
    bool disableByClient(const BaseUid & id);
    bool enableByClient(const BaseUid & id);
    bool enableByClientAll();

    void onGetAvailableProfileComplementsRequest(PMPFrame * frame);
    void onGetAvailableProfileComplementsResponse(PMPFrame * frame);
    void onReloadProfilesFromRepository (PMPFrame * frame);
    void onResetProfilesState(PMPFrame * frame);
    void onLock(PMPFrame * frame);
    void onUnlock(PMPFrame * frame);
    void onLockAll(PMPFrame * frame);
    void onUnlockAll(PMPFrame * frame);
    void onDisableByClient(PMPFrame * frame);
    void onEnableByClient(PMPFrame * frame);
    void onEnableByClientAll(PMPFrame * frame);

private:
    PmpProtocol * mPmpProtocol; ///< Pointer to PMP Protocol Interface
    EventFactory * mEventFactory;

    static Logger msLogger;       ///< object of logger
};

}

}

#endif /* CPMPCOREPROTOCOL_HPP_ */
