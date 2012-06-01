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
 * @file             CProfileRepoConnection.hpp
 * @brief            Header file for CProfileRepoConnection class
 * @author           VPlachkov
 ***************************************************************************/

#ifndef CPROFILEREPOCONNECTION_HPP_
#define CPROFILEREPOCONNECTION_HPP_


#include "utils/misc/include/Logger.hpp"
#include "utils/threads/include/CThread.hpp"
#include "framework/components/ProfileRepository/include/CProfileRepoClient.hpp"

namespace AXIS
{

   namespace PMP
   {
      /**
       * Class is used to handle connection with Profile Repository
       */
      class CProfileRepoConnection: public CThread
      {
      public:
         /**
          * Constructor
          * @param pRepoClient is pointer on object of Profile Repository client
          */
         explicit CProfileRepoConnection(ProfileRepository::CProfileRepoClient * pRepoClient);

         /**
          * Destructor
          */
         virtual ~CProfileRepoConnection();

         /**
          * Function is used to connect to Profile Repository
          */
         void connect();

         /**
          * Function is used to disconnect Profile Repository
          */
         void disconnect();

      private:
         /**
          * Main function of thread, used to handler connection status
          */
         virtual void threadFunc();

         ProfileRepository::CProfileRepoClient * mpRepoClient; ///< pointer on Profile Repository client

         static Logger msLogger; ///< object of logger
      };

   }

}

#endif /* CPROFILEREPOCONNECTION_HPP_ */
