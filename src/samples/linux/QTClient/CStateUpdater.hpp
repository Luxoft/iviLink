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

 /****************************************************************************/
 /*                                                                          */
 /* This file is part of iviLINK 0.9 Technology Preview Samples Set.         */
 /*                                                                          */
 /* iviLINK 0.9 Technology Preview Samples Set is free software:             */
 /* you can redistribute it and/or modify                                    */
 /* it under the terms of the GNU General Public License as published by     */
 /* the Free Software Foundation, version 3 of the License.                  */
 /*                                                                          */
 /* iviLINK 0.9 Technology Preview Samples Set is distributed in the         */
 /* hope that it will be useful, but WITHOUT ANY WARRANTY; without           */
 /* even the implied warranty of MERCHANTABILITY or FITNESS FOR A            */
 /* PARTICULAR PURPOSE.  See the GNU General Public License for more details.*/
 /*                                                                          */
 /* You should have received a copy of the GNU General Public License        */
 /* along with iviLINK 0.9 Technology Preview.                               */
 /* If not, see <http://www.gnu.org/licenses/>.                              */
 /*                                                                          */
 /****************************************************************************/

#ifndef CSTATE_UPDATER_HPP
#define CSTATE_UPDATER_HPP

#include <string>
#include <map>
#include <QObject>
#include "CBtnProcessor.hpp"
#include "IProfileObserver.hpp"

#include "utils/misc/include/Logger.hpp"
#include "samples/linux/Profiles/ProfileAPI/ISeatProfileAPI.hpp"
#include "samples/linux/Profiles/ProfileAPI/IClimateProfileAPI.hpp"
#include "framework/libraries/ServicesLib/src/include/CGenericService.hpp"
#include "framework/libraries/ServicesLib/src/include/CServiceManager.hpp"
#include "framework/libraries/ServicesLib/src/include/CVehicleSystemService.hpp"
#include "framework/libraries/ServicesLib/src/include/CClimateService.hpp"

class CStateUpdater  : public IProfileObserver
                     , public CServiceManager::IClientCallbacks
{
public:

   struct ClimateProfileCallbacks   : public IClimateProfile_API::Callbacks
   {
      public:

         explicit ClimateProfileCallbacks(IProfileObserver* pObserver):pOwner(pObserver){}
         virtual void handleError(CError const & error)
         {
            LOG4CPLUS_INFO(msLogger, static_cast<std::string>(error));
         }

         virtual void messageReceived(std::string profile_id, std::string message)
         {
            LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
            pOwner->messageReceived(profile_id, message);
         }

      private:
         IProfileObserver* pOwner;
   };

   struct SeatProfileCallbacks   : public ISeatProfile_API::Callbacks
   {
      public:
         explicit SeatProfileCallbacks(IProfileObserver* pObserver):pOwner(pObserver){}
         virtual void handleError(CError const & error)
         {
            LOG4CPLUS_INFO(msLogger,static_cast<std::string>(error));
         }

         virtual void messageReceived(std::string profile_id, std::string message)
         {
            LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
            pOwner->messageReceived(profile_id, message);
         }

      private:
         IProfileObserver* pOwner;

   };

   CStateUpdater(CBtnProcessor* classPtr, QObject * qobjPtr);

   ~CStateUpdater();

   //From IProfileObserver
   virtual void  messageReceived(std::string profile_id, std::string message);

    //From CServiceManager::IClientCallbacks
   /**
    * Callback for service created on the other side
    * @param UID service uid
    */
   virtual void serviceRequestCallback(AXIS::Service::Uid const& serviceUID);

   /**
    * Callback that should be invoked when paired profile requested from other side
    * @param serviceUID service that should own this profile
    * @param profileUID profile that should be created
    */
   virtual void incomingProfileRequest(AXIS::Service::Uid const& serviceUID, AXIS::Profile::Uid const& profileUid, AXIS::Profile::ApiUid const& papiUid);

   /**
    * Callback for service dropped on the other side
    * @param UID service uid
    */
   virtual void serviceDropped(AXIS::Service::Uid const& serviceUID);

  /**
    * Callback that should be invoked when paired profile from other side died
    * @param serviceUID service that should own this profile
    * @param profileUID profile that should be created
    */

   virtual void profileDied(AXIS::Service::Uid const& serviceUID, AXIS::Profile::IUid const& piuid);

private:

   CGenericService::tProfileCallbacksMap  mProfileCbMap;

   CBtnProcessor*                         mpBtnProcessor;
   QObject *                              mpRootPtr;
   
  
   IClimateProfile_API*                   mpClimateProfile;
   CClimateService*                       mpClimateService;
   ClimateProfileCallbacks                mClimateCallbacks;
   
   CVehicleSystemService*                 mpVehicleSystemService;
   ISeatProfile_API*                      mpSeatProfile;
   SeatProfileCallbacks                   mSeatCallbacks;
   
   std::map<AXIS::Service::Uid, CGenericService* > mServicesMap;
   CLIENT_STATE                           mActiveTab;
   static Logger                          msLogger;
};

#endif // STATEUPDATER_H
