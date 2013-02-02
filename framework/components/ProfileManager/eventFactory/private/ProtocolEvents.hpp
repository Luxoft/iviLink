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


#ifndef PROTOCOLEVENTS_HPP_
#define PROTOCOLEVENTS_HPP_

#include <list>

#include "UID.hpp"
#include "Event.hpp"
#include "Logger.hpp"

namespace iviLink
{
namespace PMP
{
class PmpComponentManager;
class PmpInteractionProtocol;
class PmpPim;
class PmpCore;

class CoreProtocolGetAvailableProfileComplementsRequest : public Utils::Event
{
public:
    CoreProtocolGetAvailableProfileComplementsRequest(PmpCore * core);

private:
    virtual void handleEvent();

    PmpCore * mCore;
    Logger mLogger;
};

class CoreProtocolGetAvailableProfileComplementsResponse : public Utils::Event
{
public:
    CoreProtocolGetAvailableProfileComplementsResponse(PmpCore * core,
            const std::list<Profile::Uid> & complements);

private:
    virtual void handleEvent();

    PmpCore * mCore;
    std::list<Profile::Uid> mComplements;
    Logger mLogger;
};

class CoreProtocolReloadProfilesFromRepository : public Utils::Event
{
public:
    CoreProtocolReloadProfilesFromRepository(PmpCore * core);

private:
    virtual void handleEvent();

    PmpCore * mCore;
    Logger mLogger;
};

class CoreProtocolResetProfileState : public Utils::Event
{
public:
    CoreProtocolResetProfileState(PmpCore * core);

private:
    virtual void handleEvent();

    PmpCore * mCore;
    Logger mLogger;
};

class CoreProtocolLock : public Utils::Event
{
public:
    CoreProtocolLock(PmpCore * core, const BaseUid & id);

private:
    virtual void handleEvent();

    PmpCore * mCore;
    BaseUid mId;
    Logger mLogger;
};

class CoreProtocolUnlock : public Utils::Event
{
public:
    CoreProtocolUnlock(PmpCore * core, const BaseUid & id);

private:
    virtual void handleEvent();

    PmpCore * mCore;
    BaseUid mId;
    Logger mLogger;
};

class CoreProtocolLockAll : public Utils::Event
{
public:
    CoreProtocolLockAll(PmpCore * core);

private:
    virtual void handleEvent();

    PmpCore * mCore;
    Logger mLogger;
};

class CoreProtocolUnlockAll : public Utils::Event
{
public:
    CoreProtocolUnlockAll(PmpCore * core);

private:
    virtual void handleEvent();

    PmpCore * mCore;
    Logger mLogger;
};

class CoreProtocolDisableByClient : public Utils::Event
{
public:
    CoreProtocolDisableByClient(PmpCore * core, const BaseUid & id);
private:
    virtual void handleEvent();

    PmpCore * mCore;
    BaseUid mId;
    Logger mLogger;
};

class CoreProtocolEnableByClient : public Utils::Event
{
public:
    CoreProtocolEnableByClient(PmpCore * core, const BaseUid & id);

private:
    virtual void handleEvent();

    PmpCore * mCore;
    BaseUid mId;
    Logger mLogger;
};

class CoreProtocolEnableByClientAll : public Utils::Event
{
public:
    CoreProtocolEnableByClientAll(PmpCore * core);

private:
    virtual void handleEvent();

    PmpCore * mCore;
    Logger mLogger;
};

/// Events from Interaction Protocol for PIM
class PimProtocolProfileDied : public Utils::Event
{
public:
    PimProtocolProfileDied(PmpPim * pim, const Profile::IUid & piuid, const Service::Uid & sid);

private:
    virtual void handleEvent();

    PmpPim * mPim;
    Profile::IUid mPIUid;
    Service::Uid mSUid;
    Logger mLogger;
};

class PimProtocolCreateProfile : public Utils::Event
{
public:
    PimProtocolCreateProfile(PmpPim * pim, const Profile::Uid & profileUid,
            const Profile::IUid & piuid, const Service::Uid & sid);

private:
    virtual void handleEvent();
    virtual int getTimeout();

    PmpPim * mPim;
    Profile::Uid mProfileUid;
    Profile::IUid mProfileIUid;
    Service::Uid mSUid;
    Logger mLogger;
};

/// Exceptions from Interaction protocol
class ExceptionProtocolChannelDeleted : public Utils::Event
{
public:
    ExceptionProtocolChannelDeleted(PmpComponentManager * manager);

private:
    virtual void handleEvent();

    PmpComponentManager * mManager;
    Logger mLogger;
};

class ExceptionProtocolConnectionLost : public Utils::Event
{
public:
    ExceptionProtocolConnectionLost(PmpComponentManager * manager);

private:
    virtual void handleEvent();

    PmpComponentManager * mManager;
    Logger mLogger;
};

class ProtocolEventNegotiateRole : public Utils::Event
{
public:
    ProtocolEventNegotiateRole(PmpComponentManager * manager, UInt32 number);

private:
    virtual void handleEvent();

    Logger mLogger;
    PmpComponentManager * mManager;
    UInt32 mNumber;
};

} /* namespace PMP */
} /* namespace iviLink */
#endif /* PROTOCOLEVENTS_HPP_ */
