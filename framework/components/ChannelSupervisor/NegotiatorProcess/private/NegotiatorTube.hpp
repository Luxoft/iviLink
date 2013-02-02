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


#ifndef NEGOTIATORTUBE_HPP_
#define NEGOTIATORTUBE_HPP_

#include "ChannelSupervisorTube.hpp"
#include "IChannelSupervisorObserver.hpp"
#include "NegotiatorStates.hpp"
#include "CNegotiatorToSysCtrlProxy.hpp" 
#include "Logger.hpp"

namespace iviLink
{
namespace ChannelSupervisor
{

class NegotiatorTube: public IChannelSupervisorTubeObserver
{
public:

    NegotiatorTube(NegotiatorStates * states, CNegotiatorToSysCtrlProxy* pMsgProxy);
    ~NegotiatorTube();

    void onDataReceived(const UInt32 channelId, const UInt32 readSize);
    void onBufferOverflow(const UInt32 channelId);
    void onChannelDeleted(const UInt32 channelId);
    void onConnectionLost();

    void sendDataTube(const std::string& data);

private:

    NegotiatorStates* mNegotiatorStates;
    CNegotiatorToSysCtrlProxy* mMsgProxy;
    static Logger mLogger;

};

}  // namespace ChannelSupervisor
}  // namespace AXIS

#endif /* NEGOTIATORTUBE_HPP_ */
