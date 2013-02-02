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


#ifndef CSERVERCONTROL_HPP_
#define CSERVERCONTROL_HPP_

#include "Logger.hpp"
#include "IMediaControlServerProfileAPI.hpp"

#include <stdlib.h>
#include <unistd.h>
#include <string>
#include "sys/types.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "stdio.h"

//#include "utils/misc/Logger.hpp"
//#include "framework/libraries/profileLib/ProfileLibCommon.hpp"
//#include "samples/linux/Profiles/ProfileAPI/IMediaControlServerProfileAPI.hpp"
//#include "samples/linux/Profiles/MediaControlServerProfile/ISender.hpp"

//using iviLink::CBuffer;
//using iviLink::Channel::tChannelId;

//class CMutex;
class CServerControl  {

public:

   std::string mStop ;
   std::string mResume;
   std::string mPause;  
   std::string mAudio;
   std::string mVideo;
   std::string mPlay;
  
   CServerControl(IMediaControlServerProfile_API::Callbacks* callbacks);
   virtual ~CServerControl();

   bool sendCommand(std::string const& command); 
   bool sendPlay(std::string const& port, std::string const& path, std::string const& streamName, std::string const& avform); 

private:
   
   int writer(const char* message);

   //CMutex *                                     mpMutex;
   std::string                                  mVTranscode;
   std::string                                  mATranscode;
   std::string                                  mServerPath;
   int                                          mFd;
   int 						fds[2];
   FILE*                                        file; 
   static Logger                                msLogger;


};

#endif /* CSERVERCONTROL_HPP_ */
