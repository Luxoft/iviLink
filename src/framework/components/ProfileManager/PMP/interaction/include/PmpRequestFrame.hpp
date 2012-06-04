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





#ifndef PMPREQUESTFRAME_HPP_
#define PMPREQUESTFRAME_HPP_

#include "utils/misc/include/Types.hpp"

namespace AXIS
{
   namespace PMP
   {
      typedef UInt8 tPmpFrameType;
      typedef UInt32 tMessageId;
      typedef UInt8 tClientId;
      typedef UInt8 tRequestType;

      const tClientId PMP_CLIENT_CORE = 0;
      const tClientId PMP_CLIENT_PI_SRV = 1;
      const tClientId PMP_CLIENT_CMP_MGR = 2;

      const tPmpFrameType PMP_FRAME_TYPE_REQUEST = 0;
      const tPmpFrameType PMP_FRAME_TYPE_RESPONSE = 1;
//      const tPmpFrameType PMP_FRAME_TYPE_MESSAGE = 2;

      const tClientId PMP_FRAME_CLIENT_ID_CORE = 0;
      const tClientId PMP_FRAME_CLIENT_ID_PIM = 1;
      const tClientId PMP_FRAME_CLIENT_ID_CMP_MGR = 2;

      struct PMPFrame
      {
         tPmpFrameType frameType;   //> Type of frame request, responce
         tMessageId id;             //> is used in response map
         tClientId client;          //> PMP Core, PIM etc
         tRequestType reqType;      //> function number in client
         UInt32 size;               //> size of frame (includes all PMPFrame parts)
         UInt8 data[];              //> request/response data (function arguments, return value etc)
      } __attribute__((__packed__));

      const tRequestType PMP_REQ_CORE_GET_COMPLEMENTS = 0;
      const tRequestType PMP_REQ_CORE_RELOAD_PROFS_FROM_REPO = 1;
      const tRequestType PMP_REQ_CORE_RESET_PROF_STATE = 2;
      const tRequestType PMP_REQ_CORE_LOCK = 3;
      const tRequestType PMP_REQ_CORE_UNLOCK = 4;
      const tRequestType PMP_REQ_CORE_UNLOCK_ALL = 5;
      const tRequestType PMP_REQ_CORE_DIS_BY_CLIENT = 6;
      const tRequestType PMP_REQ_CORE_ENABLE_BY_CLNT = 7;
      const tRequestType PMP_REQ_CORE_ENABLE_BY_CLNT_ALL = 8;

      const tRequestType PMP_REQ_PIM_CREATE_PROFILE = 0;
      const tRequestType PMP_REQ_PIM_PROFILE_DIED = 1;

      const tRequestType PMP_REQ_CMP_MGR_READY = 0;
   }
}

#endif /* PMPREQUESTFRAME_HPP_ */
