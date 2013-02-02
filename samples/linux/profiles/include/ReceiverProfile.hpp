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
#ifndef RECEIVER_PROFILE_HPP
#define RECEIVER_PROFILE_HPP

#include <cstdlib>

#include "ProfileAdaptor.hpp"
#include "Logger.hpp"
#include "Exit.hpp"
#include "ProfileLibCommon.hpp"
#include "casts.hpp"

// You have to implement this function somewhere
template<typename T, typename I>
Logger& logger();


// I - Profile API
// T - Realization of this API
// Using in Seat/Climate receiver profile realization
template<typename T, typename I>
class ReceiverProfile : public iviLink::Channel::ChannelHandler
                         , public ProfileAdaptor<T,I> {
protected:
    explicit ReceiverProfile( iviLink::Profile::IProfileCallbackProxy* pCbProxy );
    virtual ~ReceiverProfile();

    virtual void onEnable();
    virtual void onDisable();

    //from CChannelHandler
    virtual void onBufferReceived(const iviLink::Channel::tChannelId channel, iviLink::Buffer const& buffer);
    virtual void onChannelDeleted(const UInt32 channel_id);
    virtual void onConnectionLost();
private:
   iviLink::Channel::tChannelId                 mChannelID;
   typename I::profile_callback_t              *callbacks;
};

//////// Rock'N'Roll /////////

template<typename T, typename I>
ReceiverProfile<T,I>::ReceiverProfile( iviLink::Profile::IProfileCallbackProxy* pCbProxy )
{
    LOG4CPLUS_TRACE_METHOD( (logger<T,I>()), __PRETTY_FUNCTION__ );
    callbacks = polymorphic_downcast<typename I::profile_callback_t*>( pCbProxy );
}


template<typename T, typename I>
ReceiverProfile<T,I>::~ReceiverProfile()
{
    LOG4CPLUS_TRACE_METHOD( (logger<T,I>()), __PRETTY_FUNCTION__ );
    if( mChannelID )
        iviLink::Channel::deallocateChannel( mChannelID );
}

template<typename T, typename I>
void ReceiverProfile<T,I>::onEnable()
{
    LOG4CPLUS_TRACE_METHOD( (logger<T,I>()), __PRETTY_FUNCTION__ );

    //assert(callbacks);
    if( callbacks )
    {
        LOG4CPLUS_TRACE((logger<T,I>()), "Trying access callbacks pointer "
                        + cast_to_string(callbacks) );
        const std::string ch_id = callbacks->get_channel_id();
        LOG4CPLUS_TRACE((logger<T,I>()), "Trying allocate channel \"" + ch_id + "\"");
        iviLink::Error err = iviLink::Channel::allocateChannel(ch_id, this, mChannelID, eRealTime);
        if (err.isNoError())
        {
            LOG4CPLUS_TRACE((logger<T,I>()), "Channel \""
                           + ch_id
                           + "\" allocated, starting the communication...");
        }
        else
        {
            LOG4CPLUS_ERROR((logger<T,I>()), "allocate Channel \""
                            + ch_id
                            + "\" failed");
            killProcess(1);
        }
    }
    else
        LOG4CPLUS_ERROR((logger<T,I>()), "Can't find application callbacks, so can't get correct channel id");
}

template<typename T, typename I>
void ReceiverProfile<T,I>::onDisable()
{
    LOG4CPLUS_TRACE_METHOD( (logger<T,I>()), __PRETTY_FUNCTION__ );
    iviLink::Error err = iviLink::Channel::deallocateChannel(mChannelID);
    if (!err.isNoError())
    {
        LOG4CPLUS_ERROR((logger<T,I>()), "Unable to deallocate channel: " + static_cast<std::string>(err));
    }
    else
        mChannelID = 0;
}

template<typename T, typename I>
void ReceiverProfile<T,I>::onBufferReceived(const iviLink::Channel::tChannelId channel, iviLink::Buffer const& buffer)
{
    LOG4CPLUS_TRACE_METHOD((logger<T,I>()), __PRETTY_FUNCTION__ );

    if (mChannelID != channel)
    {
        LOG4CPLUS_INFO((logger<T,I>()), "mChannelID != channel");
        return;
    }
    else
    {
        LOG4CPLUS_INFO((logger<T,I>()), "mChannelID == channel");
    }

    typename I::state_t st;
    if( st.deserialize(std::string((char*)buffer.getBuffer()) ) )
    {
        receive( st );
    }
    else
        LOG4CPLUS_ERROR( (logger<T,I>()), "Failed to deserialize state from buffer" );
}

template<typename T, typename I>
void ReceiverProfile<T,I>::onChannelDeleted(const UInt32 channel_id)
{
    LOG4CPLUS_TRACE_METHOD((logger<T,I>()), __PRETTY_FUNCTION__ );

    if (mChannelID == channel_id)
        mChannelID = 0;

    iviLink::Error err = iviLink::Channel::deallocateChannel(channel_id);
    if (!err.isNoError())
    {
        LOG4CPLUS_WARN((logger<T,I>()), "Unable to deallocate channel: " + static_cast<std::string>(err));
    }
}

template<typename T, typename I>
void ReceiverProfile<T,I>::onConnectionLost()
{
    LOG4CPLUS_ERROR((logger<T,I>()), "seat receiver profile: connection lost!");
}

#endif // RECEIVER_PROFILE_HPP


