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





#include <cassert>

#include "framework/components/ProfileManager/PMP/interaction/include/CPmpPimClbCtrl.hpp"
#include "framework/components/ProfileRepository/include/Serialize.hpp"
#include "utils/misc/include/byteOrder.hpp"

namespace AXIS
{

   namespace PMP
   {

      CPmpPimClbCtrl::CPmpPimClbCtrl(IPmpProtocol * pPtorocol, IPmpPimClb * pPimClb)
         : mpProtocol(pPtorocol)
         , mpPimClb(pPimClb)
      {
         assert(mpProtocol);
         assert(mpPimClb);
      }

      CPmpPimClbCtrl::~CPmpPimClbCtrl()
      {
      }

      void CPmpPimClbCtrl::onCreateProfile(PMPFrame * pFrame)
      {
         std::string str = bufferToString(pFrame->data,true);
         unsigned int pos = stringInBufSize(str);
         AXIS::Profile::Uid profileUid(str);
         str = bufferToString(pFrame->data + pos,true);
         pos += stringInBufSize(str);
         AXIS::Profile::IUid piuid(str);
         str = bufferToString(pFrame->data + pos,true);
         AXIS::Service::Uid sid(str);

         mpPimClb->onCreateProfile(profileUid,piuid,sid);

         PMPFrame pResp;
         pResp.size = sizeof(PMPFrame);
         pResp.frameType = PMP_FRAME_TYPE_RESPONSE;
         pResp.id = pFrame->id;
         pResp.client = pFrame->client;
         pResp.reqType = pFrame->reqType;

         mpProtocol->makeResponse(pResp);
      }

      void CPmpPimClbCtrl::onProfileDied(PMPFrame * pFrame)
      {
         std::string str = bufferToString(pFrame->data,true);
         unsigned int pos = stringInBufSize(str);
         AXIS::Profile::IUid piuid(str);
         str = bufferToString(pFrame->data + pos,true);
         AXIS::Service::Uid sid(str);

         mpPimClb->onProfileDied(piuid,sid);

         PMPFrame pResp;
         pResp.size = sizeof(PMPFrame);
         pResp.frameType = PMP_FRAME_TYPE_RESPONSE;
         pResp.id = pFrame->id;
         pResp.client = pFrame->client;
         pResp.reqType = pFrame->reqType;

         mpProtocol->makeResponse(pResp);
      }

   }

}
