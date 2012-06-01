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

#include <queue>
#include <list>
#include <iostream>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <QVariant>
#include "string.h"
#include "utils/json/include/json/json.h"
#include "common.h"
#include "CStateUpdater.hpp"
#include <cassert>

#include "framework/components/ProfileManager/include/CProfile.hpp"

#include "utils/misc/include/UID.hpp"
#include "framework/components/ProfileManager/PMP/componentManager/include/CPmpComponentManager.hpp"

#include "framework/libraries/AppMan/App/include/CAppManConnectController.hpp"

using AXIS::Profile::CProfile;
using AXIS::Profile::ApiUid;
using AXIS::AppMan::CAppManConnectController;
using AXIS::AppMan::IAppMan;

#include "framework/components/ChannelSupervisor/Tube/include/ChannelSupervisorTube.hpp"

Logger CStateUpdater::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("samples.app.CStateUpdater"));

void LINK_GUARD()
{
   CError err = AXIS::ChannelSupervisor::deallocateChannel(1, std::string());
}

CStateUpdater::CStateUpdater(CBtnProcessor* classPtr, QObject* qobjPtr):
   mpBtnProcessor(classPtr),
   mpRootPtr(qobjPtr),
   mpClimateService(NULL),
   mClimateCallbacks(this),
   mpVehicleSystemService(NULL),
   mSeatCallbacks(this),
   mActiveTab(CLIMATE)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

   qRegisterMetaType<Phonon::MediaSource>( "MediaSource" );

   mProfileCbMap.insert (std::pair<AXIS::Profile::ApiUid, AXIS::Profile::IProfileCallbackProxy* >(std::string("ClimateProfile_PAPI_UID"), &mClimateCallbacks));
   mProfileCbMap.insert (std::pair<AXIS::Profile::ApiUid, AXIS::Profile::IProfileCallbackProxy* >(std::string("SeatProfile_PAPI_UID"), &mSeatCallbacks));

   CServiceManager::getInstance()->registerClient(this);

   std::list<AXIS::Service::Uid> supported;
   supported.push_back(AXIS::Service::Uid("ClimateServiceHUI"));
   supported.push_back(AXIS::Service::Uid("VehicleSystemHUI"));

   CAppManConnectController* pamcc = CAppManConnectController::instance();
   assert(pamcc);
   IAppMan* pam = pamcc->applicationManager();
   assert(pam);
   AXIS::AppMan::EInitResult initRes = pam->initApplication(supported);

   LOG4CPLUS_INFO(msLogger, "CStateUpdater::CStateUpdater initApplication result = " + convertIntegerToString((int)initRes));

   if (AXIS::AppMan::STARTED_BY_USER == initRes)
   {
      CGenericService* pService = CServiceManager::getInstance()->loadService("ClimateServiceHUI",mProfileCbMap);
      assert(pService);
      pService->instantiate();
      mServicesMap.insert (std::pair<AXIS::Service::Uid, CGenericService* >(std::string("ClimateServiceHUI"), pService)); 
     
      pService = CServiceManager::getInstance()->loadService("VehicleSystemHUI", mProfileCbMap);
      assert(pService);
      pService->instantiate();
      mServicesMap.insert (std::pair<AXIS::Service::Uid, CGenericService* >(std::string("VehicleSystemHUI"), pService)); 
      
      CVehicleSystemService* pVehicleService = static_cast<CVehicleSystemService* >(mServicesMap[std::string("VehicleSystemHUI")]);
      CClimateService* pClimateService = static_cast<CClimateService* >(mServicesMap[std::string("ClimateServiceHUI")]);
      
      mpBtnProcessor->setServices(pVehicleService,pClimateService);
   }

}

CStateUpdater::~CStateUpdater()
{
   mpBtnProcessor = NULL;
   mpRootPtr = NULL;

   if(mpClimateService) delete mpClimateService;
   if(mpVehicleSystemService) delete mpVehicleSystemService;
}
void CStateUpdater::serviceRequestCallback(AXIS::Service::Uid const& serviceUID)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
   std::string uid = serviceUID.value();
   CGenericService* pService = CServiceManager::getInstance()->loadService(uid.c_str(),mProfileCbMap); 
   mServicesMap.insert (std::pair<AXIS::Service::Uid, CGenericService* >(serviceUID, pService)); 
}

void CStateUpdater::incomingProfileRequest(AXIS::Service::Uid const& serviceUID, AXIS::Profile::Uid const& profileUid, AXIS::Profile::ApiUid const& papiUid)
{
   mServicesMap[serviceUID]->incomingProfileRequest(profileUid, papiUid);
   CVehicleSystemService* pVehicleService = NULL;
   CClimateService* pClimateService = NULL;
   std::map<AXIS::Service::Uid, CGenericService* >::iterator iter = mServicesMap.find(std::string("VehicleSystemClient"));
   if(iter != mServicesMap.end())
   {
      pVehicleService = static_cast<CVehicleSystemService* >(iter->second);
   }
   iter = mServicesMap.find(std::string("ClimateServiceClient"));
   if(iter != mServicesMap.end())
   {
      pClimateService = static_cast<CClimateService* >(iter->second);
   }
   mpBtnProcessor->setServices(pVehicleService,pClimateService);  
}

void CStateUpdater::serviceDropped(AXIS::Service::Uid const& serviceUID)
{

}

void CStateUpdater::profileDied(AXIS::Service::Uid const& serviceUID, AXIS::Profile::IUid const& piuid)
{
   mServicesMap[serviceUID]->unloadProfile(piuid);
}

void CStateUpdater::messageReceived(std::string profile_id, std::string message)
{
    Json::Value root; // will contain the root value after parsing.
    Json::Reader reader;

    bool parsingSuccessful = reader.parse(message.c_str(), root);
    if (!parsingSuccessful)
    {
        // report to the user the failure and their locations in the document.
        LOG4CPLUS_INFO(msLogger, "Failed to parse configuration\n" + reader.getFormatedErrorMessages());
    }
    else
    {
       bool bool_value;
       int int_value;
       if(profile_id == "ClimateProfileImpl")
       {
           if(mActiveTab != CLIMATE)
           {
                mpBtnProcessor->activityClimateOn();
                mActiveTab = CLIMATE;
           }
           bool_value = root.get(off, false).asBool();
           mpBtnProcessor->emitOff(bool_value);

           bool_value  =  root.get(recStatus, false).asBool();
           mpBtnProcessor->emitRecirc(bool_value);

           bool_value  =  root.get(auto_mode, false).asBool();
           mpBtnProcessor->emitAuto(bool_value);

           bool_value  =  root.get(dual, false).asBool();
           mpBtnProcessor->emitDual(bool_value);

           int int_value = root.get(fanSpd, 0).asInt();
           mpBtnProcessor->emitFanSpeed(int_value);

           int_value = root.get(tempD, 0).asInt();
           mpBtnProcessor->emitDrvTemp(int_value);

           int_value = root.get(tempP, 0).asInt();
           mpBtnProcessor->emitPassTemp(int_value);

           bool_value  =  root.get(fanlow, false).asBool();
           mpBtnProcessor->emitFanLow(bool_value);

           bool_value  =  root.get(fanmiddle, false).asBool();
           mpBtnProcessor->emitFanMiddle(bool_value);

           bool_value  =  root.get(fantop, false).asBool();
           mpBtnProcessor->emitFanTop(bool_value);

           bool_value  =  root.get(acStatus, false).asBool();
           mpBtnProcessor->emitAc(bool_value);

           bool_value  =  root.get(head, false).asBool();
           mpBtnProcessor->emitHead(bool_value);

           bool_value  =  root.get(rear, false).asBool();
           mpBtnProcessor->emitRear(bool_value);

       }
       else if(profile_id == "SeatProfileImpl")
       {
           if(mActiveTab != SEAT)
           {
                mpBtnProcessor->activitySeatOn();
                mActiveTab = SEAT;
           }
           int_value = root.get(heater, 0).asInt();
           mpBtnProcessor->emitHeater(int_value);

           int_value = root.get(heater_pass, 0).asInt();
           mpBtnProcessor->emitHeaterPass(int_value);

           int_value = root.get(activeSeat, 0).asInt();

           if(int_value == 0) //driver
           {
               int seatBackVal = root.get(seatBackDriver, 0).asInt();
               int seatBottomVal = root.get(seatBottomDriver, 0).asInt();
               int seatLiftVal = root.get(seatLiftDriver, 0).asInt();
               LOG4CPLUS_INFO(msLogger, "DRIVER seat");
               mpRootPtr->setProperty("driver_bottom_x", seatBottomVal);
               mpRootPtr->setProperty("driver_bottom_y", seatLiftVal);
               mpRootPtr->setProperty("driver_back_angle", seatBackVal);
               mpRootPtr->setProperty("current_seat_view", int_value);
           }
           else if (int_value == 1) //passen
           {
               int seatBackVal = root.get(seatBackPass, 0).asInt();
               int seatBottomVal = root.get(seatBottomPass, 0).asInt();
               int seatLiftVal = root.get(seatLiftPass, 0).asInt();
               LOG4CPLUS_INFO(msLogger, "PASSEN seat");
               mpRootPtr->setProperty("passen_bottom_x", seatBottomVal);
               mpRootPtr->setProperty("passen_bottom_y", seatLiftVal);
               mpRootPtr->setProperty("passen_back_angle", seatBackVal);
               mpRootPtr->setProperty("current_seat_view", int_value);

           }
       }
   }
}
