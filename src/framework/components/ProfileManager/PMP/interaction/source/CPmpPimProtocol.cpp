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

/***************************************************************************
 * Project           AXIS
 * (c) copyright     2012
 * Company           LUXOFT
 * @file             CPmpPimProtocol.hpp
 * @brief            Source file for CPmpPimProtocol class
 * @author           VPlachkov
 ***************************************************************************/

#include <cassert>

#include "framework/components/ProfileManager/PMP/interaction/include/CPmpPimProtocol.hpp"
#include "framework/components/ProfileRepository/include/Serialize.hpp"
#include "framework/components/ProfileManager/PMP/include/CPmpError.hpp"
#include "utils/misc/include/byteOrder.hpp"

namespace AXIS
{

   namespace PMP
   {

      CPmpPimProtocol::CPmpPimProtocol(IPmpProtocol * pPmpProtocol)
         : mpPmpProtocol(pPmpProtocol)
      {
         assert(mpPmpProtocol);
      }

      CPmpPimProtocol::~CPmpPimProtocol()
      {
      }

      CError CPmpPimProtocol::createProfile(AXIS::Profile::Uid const& profileUid,
         AXIS::Profile::IUid const& piuid,
         AXIS::Service::Uid const& sid)
      {
         UInt32 size = sizeof(PMPFrame);
         size += stringInBufSize(profileUid.value());
         size += stringInBufSize(piuid.value());
         size += stringInBufSize(sid.value());

         UInt8 * pBuf = new UInt8[size];
         PMPFrame * pReq = reinterpret_cast<PMPFrame*>(pBuf);
         pReq->size = size;
         pReq->reqType = PMP_REQ_PIM_CREATE_PROFILE;

         stringToBuffer(profileUid.value(),pReq->data,true);
         unsigned int pos = stringInBufSize(profileUid.value());
         stringToBuffer(piuid.value(),pReq->data + pos,true);
         pos += stringInBufSize(piuid.value());
         stringToBuffer(sid.value(),pReq->data + pos,true);

         PMPFrame * pResp = mpPmpProtocol->makePimRequest(*pReq);
         delete [] pReq;

         if (!pResp)
         {
            return CPmpError(CPmpError::ERROR_NETWORK);
         }

         delete[] pResp;
         return CPmpError::NoPmpError();
      }

      CError CPmpPimProtocol::profileDied(AXIS::Profile::IUid const& piuid,
         AXIS::Service::Uid const& sid)
      {
         UInt32 size = sizeof(PMPFrame);
         size += stringInBufSize(piuid.value());
         size += stringInBufSize(sid.value());

         UInt8 * pBuf = new UInt8[size];
         PMPFrame * pReq = reinterpret_cast<PMPFrame*>(pBuf);
         pReq->size = size;
         pReq->reqType = PMP_REQ_PIM_PROFILE_DIED;

         stringToBuffer(piuid.value(),pReq->data,true);
         unsigned int pos = stringInBufSize(piuid.value());
         stringToBuffer(sid.value(),pReq->data + pos,true);

         PMPFrame * pResp = mpPmpProtocol->makePimRequest(*pReq);
         delete [] pReq;

         if (!pResp)
         {
            return CPmpError(CPmpError::ERROR_NETWORK);
         }

         delete[] pResp;
         return CPmpError::NoPmpError();
      }

   }

}
