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


#ifndef CTRANSMIT_DISPATCHER_HPP
#define CTRANSMIT_DISPATCHER_HPP
 
#include <vector>
#include <list>
 
#include "Types.hpp"
#include "CThread.hpp"
#include "CMutex.hpp"
#include "ConnectivityAgentError.hpp"

namespace iviLink
{
namespace ConnectivityAgent
{
namespace HAL
{
    struct Frame;
    class CCarrierAdapter;
}  // namespace HAL

namespace L1
{
    class IBufferProducer;
}  // namespace L1

namespace L0
{
    class CSource;
    class CTransmittedFramesQueue;

/**
 * The CTransmitDispatcher class uses CThread functionality.
 * Implements the dispatching strategy RR with priorities of sending outgoing frames from different
 * data sources.
 */
class CTransmitDispatcher: public CThread
{
private:
    static Logger logger;

public:
    /**
     * TSourceList type represents list of data sources with same priority
     */
    typedef std::list<CSource*> TSourceList;

    /**
     * TRegistry type represents vector of list of data sources with same priority.
     * Basically it contains three lists as soon as we have three priority types
     */
    typedef std::vector<TSourceList> TRegistry;

    /**
     * This enum is used to simply declare constant quotes for data sources depending of
     * their priority
     */
    enum
    {
        SC_QUOTE = 4000,
        RT_QUOTE = 4000,
        PD_QUOTE = 4000
    };

    /**
     * Constructor
     * @param pCarrierAdaptor is the pointer to current carrier adapter which interacts with
     *                        physical connection
     * @param pTransmittedFrameQueue is the pointer to queue where the transmitted frames are
     *                        stored in waiting for acknowledge
     */
    CTransmitDispatcher(iviLink::ConnectivityAgent::HAL::CCarrierAdapter* pCarrierAdaptor,
                        CTransmittedFramesQueue* pTransmittedFrameQueue);

    /**
     * Destructor
     */
    virtual ~CTransmitDispatcher();

    /**
     * A threading routine method inherited from CThread class
     */
    virtual void threadFunc();

    /**
     * Open the outgoing channel with fixed number and priority. It creates CSource object
     * for given channel, put it to dispathcing registry, and establish connections between
     * SourceAgent and appropriate Source
     *
     * @param channel_id is the channel number
     * @param prio is the priority of a channel
     * @param bufferProducer is the reference to L0 source representaion i.e.
     *                       SourceAgent to exchange the freed buffers
     * @return pointer to the created HAL source representation i.e. Source for L0 SourceAgent
     */
    ConnectivityAgentError openChannel(const UInt32 channel_id,
                               const TChannelPriority prio,
                               iviLink::ConnectivityAgent::L1::IBufferProducer& bufferProducer);

    /**
     * Close the outgoing channel by its number
     *
     * @param channel_id is the channel number
     */
    ConnectivityAgentError closeChannel(const UInt32 channel_id);

    /**
     * Change Carrier adapter in case of reconnection or changing current carrier
     *
     * @param pCarrierAdaptor  pCarrierAdaptor is the pointer to current carrier adapter which
     * interacts with physical connection
     */
    void replaceCarrier(iviLink::ConnectivityAgent::HAL::CCarrierAdapter* pCarrierAdaptor);

private:
    /**
     * Copying constructor
     *
     * Unimplemented to avoid missusing
     *
     * @param classRef The reference to the object the class is constructing on
     */
    CTransmitDispatcher(const CTransmitDispatcher& classRef);

    /**
     * Assigning operator
     *
     * Unimplemented to avoid missusing
     *
     * @param classRef The reference to the object the class is assiging from
     */
    CTransmitDispatcher& operator=(const CTransmitDispatcher& classRef);

    /**
     * Get quote by source type (by source channel priority)
     *
     * @param type is channel priority
     */
    UInt32 getQuoteBySourceType(TChannelPriority type);

    /**
     * Get info about awaiting data of chosen priority
     *
     * @param priority is channels priority, where we are looking for the data
     */
    bool hasCertainPriorityData(const TChannelPriority priority);

    iviLink::ConnectivityAgent::HAL::CCarrierAdapter* mpCarrier;
    CTransmittedFramesQueue* mpTransmittedFrameQueue;
    CMutex mRegistryMutex;
    TRegistry mRegistry;
    bool mDestroyed;

    CSignalSemaphore mEventSem;
};
} //namespace L0
} //namespace ConnectivityAgent
} //namespace iviLink

#endif
