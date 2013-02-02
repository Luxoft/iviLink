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


#ifndef CPMPPROTOCOL_HPP_
#define CPMPPROTOCOL_HPP_

#include <map>

#include "Logger.hpp"
#include "IChannelSupervisorObserver.hpp"

#include "PmpRequestFrame.hpp"

namespace iviLink
{

namespace PMP
{
class EventFactory;
class PmpCoreProtocol;
class PmpPimProtocol;
class PmpSessionProtocol;

/**
* Class that implements PMP Protocol, implements two interfaces:
* IChannelSupervisorTubeObserver and interface
*/
class PmpProtocol : public ChannelSupervisor::IChannelSupervisorTubeObserver
{
public:

    void init(EventFactory * const eventFactory, PmpCoreProtocol * const coreProtocol,
            PmpPimProtocol * const pimProtocol, PmpSessionProtocol * const sessionProtocol);

    /**
    * Connects with Channel supervisor
    */
    bool connect();

    /**
    * Disconnects
    *
    * @param final if true, then connection will not be reestablished
    */
    void disconnect();


    // from IPmpProtocol

    /**
    * Function is used to make requests from PMP Core
    */
    bool makeCoreRequest(PMPFrame & request);

    /**
    * Function is used to make requests from PMP PIM
    */
    bool makePimRequest(PMPFrame & request);

    /**
     * Function is used to make interaction requests for session management
     */
    bool makeSessionRequest(PMPFrame & request);

    /**
    * Function is used to say thar PMP is ready to receive requests from anothre side
    * @return true if OK, false if connection error
    */
    bool ready();

    /**
    * Function is used to know if anothre side is ready to receive our requests
    */
    bool isReady() const;

    /**
    * Constructor
    */
    PmpProtocol();
    /**
    * Destructor
    */
    virtual ~PmpProtocol();

private:
    // from ChannelSupervisor::IChannelSupervisorTubeObserver

    virtual void onDataReceived(const UInt32 channelId, const UInt32 readSize);

    virtual void onBufferOverflow(const UInt32 channelId);

    virtual void onChannelDeleted(const UInt32 channelId);

    virtual void onConnectionLost();

    private:

    /**
    * Is used for request needed for PMP Conmponent manager
    */
    bool makeCmpMgrRequest(PMPFrame & request);

    /**
    * Callback, is used to react on Component manager requests
    */
    void onCmpMgrRequest(PMPFrame * pFrame);

    /**
    * Generates message ID
    */
    MessageId generateId ();

    /**
    * Makes request on another side
    */
    bool makeRequest(PMPFrame & requestFrame);

    /**
    * Callback, is used to react on requests from another side
    */
    void onRequest(PMPFrame * pFrame);

    /**
    * Callback, is used to react on Core requests
    */
    void onCoreRequest(PMPFrame * pFrame);

    /**
    * Callback, is used to react on PIM requests
    */
    void onPimRequest(PMPFrame * pFrame);

    /**
     * Callback, is used to react on session requests
     */
    void onSessionRequest(PMPFrame * pFrame);

    unsigned int mChannelId;        ///< ID of Channel Supervisor's channel for PMP

    std::string mTag;               ///< Tag of channel
    MessageId mGenId;               ///< is used by generateId()
    EventFactory * mEventFactory;
    bool mReady;                    ///< is true if PMP on another side has been inited
    PmpCoreProtocol * mCoreProtocol;
    PmpPimProtocol * mPimProtocol;
    PmpSessionProtocol * mSessionProtocol;

    static Logger msLogger;         ///< object of logger
};

}

}

#endif /* CPMPPROTOCOL_HPP_ */
