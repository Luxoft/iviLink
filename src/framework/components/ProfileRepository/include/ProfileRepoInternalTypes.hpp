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





#ifndef PROFILEREPOINTERNALTYPES_HPP_
#define PROFILEREPOINTERNALTYPES_HPP_

namespace AXIS
{
   namespace ProfileRepository
   {
      const UInt16 REQ_ADD_PROFILE_API = 0;
      const UInt16 REQ_REMOVE_PROFILE_API = 1;
      const UInt16 REQ_ADD_PROFILE = 2;
      const UInt16 REQ_REMOVE_PROFILE = 3;
      const UInt16 REQ_ADD_PROFILE_IMPLEMENTATION = 4;
      const UInt16 REQ_REMOVE_PROFILE_IMPLEMENTATION_PL = 5;
      const UInt16 REQ_FIND_PROFILES = 6;
      const UInt16 REQ_GET_MANIFEST = 7;
      const UInt16 REQ_GET_PROFILE_LIST = 8;

      /**
       * Data type for IPC requests to Profile Repository
       */
      struct RepoRequest
      {
         UInt16 type;   ///< Requst type (one type for one nuction of Profile Repository
         UInt32 size;   ///< Request size
         UInt8 data[];  ///< Request data
      } __attribute__((__packed__));
   }
}
#endif /* PROFILEREPOINTERNALTYPES_HPP_ */
