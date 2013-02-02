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


#ifndef NEGOTIATORCLIENT_HPP
#define  NEGOTIATORCLIENT_HPP

#include <string>
#include <tr1/memory>

#include "CMutex.hpp"
#include "Types.hpp"
#include "Logger.hpp"
#include "CMutex.hpp"

namespace iviLink
{
namespace ChannelSupervisor
{

struct NegotiatorIPCClient;

class NegotiatorClientDeleter;

class NegotiatorClient
{
    friend class NegotiatorClientDeleter;

public:
    static NegotiatorClient* getInstance();

    BaseError NegotiateChannel(std::string const& tag, UInt32 & channelId);
    BaseError UpdateMapWithCID(std::string const& tag, UInt32 & channelId);
    BaseError FreeChannel(const UInt32 channelId);
    BaseError UpdateChannelInfo(std::string const& tag, const UInt32 & channelId);

private:
    NegotiatorClient();
    NegotiatorClient(NegotiatorClient const&);
    void operator =(NegotiatorClient const&);
    ~NegotiatorClient();

    static std::tr1::shared_ptr<NegotiatorClient> m_Instance;
    static CMutex m_SingletonMutex;

    NegotiatorIPCClient * m_ipcClient;
    static Logger msLogger;

};

}  // namespace ChannelSupervisor
}  // namespace AXIS

#endif //NEGOTIATORCLIENT_HPP
