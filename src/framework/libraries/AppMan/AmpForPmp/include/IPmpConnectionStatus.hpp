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
 * @file             IPmpConnectionStatus.hpp
 * @brief            Header file for IPmpConnectionStatus interface
 * @author           VPlachkov
 ***************************************************************************/

#ifndef IPMPCONNECTIONSTATUS_HPP_
#define IPMPCONNECTIONSTATUS_HPP_

namespace AXIS
{
   namespace AppMan
   {
      namespace Ipc
      {

         /**
          * Interface of IPC connection status with PMP
          */
         class IPmpConnectionStatus
         {
         public:
            /**
             * Callback is called in case of losing connection with PMP
             */
            virtual void onConnectionLost() = 0;

            /**
             * Virtual destructor
             */
            virtual ~IPmpConnectionStatus() {}
         };
      }
   }
}

#endif /* IPMPCONNECTIONSTATUS_HPP_ */
