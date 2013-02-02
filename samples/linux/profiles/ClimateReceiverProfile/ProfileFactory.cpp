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


#include "IClimateProfiles.hpp"
#include "ReceiverProfile.hpp"

class ClimateReceiverProfile   : public ReceiverProfile<ClimateReceiverProfile,IClimateReceiverProfile>
{
public:
    static std::string pname() { return "ClimateReceiverProfile"; }
    static iviLink::Profile::Uid puid() { return iviLink::Profile::Uid("ClimateReceiverProfile_UID"); }
    static UInt32 version() { return 1; }

    explicit ClimateReceiverProfile(iviLink::Profile::IProfileCallbackProxy* proxy)
        : ReceiverProfile<ClimateReceiverProfile,IClimateReceiverProfile>( proxy ) {}
};


template<>
Logger& logger<ClimateReceiverProfile,IClimateReceiverProfile>()
{
    static Logger lg = Logger::getInstance(LOG4CPLUS_TEXT("profiles.receiver.climate"));
    return lg;
}


extern "C" iviLink::Profile::ProfileHolder*
createProfile(iviLink::Profile::IUid const& id, iviLink::Service::Uid const& sid,
              iviLink::Profile::IProfileCallbackProxy* pCbProxy)
{
    LOG4CPLUS_INFO( ( logger<ClimateReceiverProfile,IClimateReceiverProfile>()),
                    "ClimateReceiver: createProfile(\"" + id.value() + "\", \"" + sid.value() + "\")");

    return new iviLink::Profile::ProfileHolder(id, sid, new ClimateReceiverProfile(pCbProxy));
}
