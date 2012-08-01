/* 
 * 
 * iviLINK SDK, version 1.0
 * http://www.ivilink.net
 * Cross Platform Application Communication Stack for In-Vehicle Applications
 * 
 * Copyright (C) 2012, Luxoft Professional Corp., member of IBS group
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
 * 
 */



#ifndef ISEATHUPROFILE_HPP_
#define ISEATHUPROFILE_HPP_

#include "framework/appLib/appLib/CProfileApiBase.hpp"
#include "utils/json/include/json/json.h"
#include "samples/linux/Profiles/ProfileAPI/ESeatRequests.hpp"

class ISeatHUProfile : public iviLink::CProfileApiBase
{
   IVILINK_PROFILE_API_UID(SeatHUProfile_API_UID)
public:
   class Callbacks : public iviLink::Profile::IProfileCallbackProxy
   {
      public:
      virtual void onMoveRequest(EMoveEvent event) = 0; 
      virtual void onTabRequest(EPersonEvent event) = 0; 
      virtual void onHeaterRequest(EPersonEvent event) = 0; 
      virtual void onInitRequest()= 0;
   };

   virtual void sendNotification(Json::Value state) = 0;
};


#endif /* ISEATHUPROFILE_HPP_ */
