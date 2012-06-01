/* 
 *  iviLINK SDK, version 0.9 (for preview only)                                      
 *    http://www.ivilink.net                                                         
 *  Cross Platform Application Communication Stack for In-Vehicle Applications       
 *                                                                                   
 *  Copyright (C) 2012, Luxoft Professional Corp., member of IBS group               
 *                                                                                   
 *  This library is free software; you can redistribute it and/or                    
 *  modify it under the terms of the GNU Lesser General Public                       
 *  License as published by the Free Software Foundation; version 2.1.               
 *                                                                                   
 *  This library is distributed in the hope that it will be useful,                  
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of                   
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU                
 *  Lesser General Public License for more details.                                  
 *                                                                                   
 *  You should have received a copy of the GNU Lesser General Public                 
 *  License along with this library; if not, write to the Free Software              
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA   
 * 
 * 
 * 
 * 
 */

#ifndef PROFILEMANAGERFORPROFILE_HPP_
#define PROFILEMANAGERFORPROFILE_HPP_

#include <string>

#include "utils/misc/include/CError.hpp"

#include "framework/components/ChannelSupervisor/Tube/include/ChannelSupervisorTube.hpp"

namespace AXIS {
namespace PMAL {
namespace Tube {

class ITubeObserver : public AXIS::ChannelSupervisor::IChannelSupervisorTubeObserver
{

};

CError allocateChannel( ITubeObserver* observer, const char * tag, const UInt32 tagLength, UInt32 & channelId );

CError deallocateChannel(const UInt32 channelId, const char * tag, const UInt32 tagLength);

CError sendData(const UInt32 channelId, const char * tag, const UInt32 tagLength, UInt8* data, UInt32 size);

CError receiveData(const UInt32 channelId, const char * tag, const UInt32 tagLength, UInt8* data, UInt32 & receivedSize, UInt32 maxSize);

}  // namespace Tube
}  // namespace ProfileManager
}  // namespace AXIS

#endif // PROFILEMANAGERFORPROFILE_HPP_
