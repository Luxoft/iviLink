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

#ifndef CSEATPROFILE_IMPL_HPP
#define CSEATPROFILE_IMPL_HPP

#include <string>
#include "utils/misc/include/Types.hpp"
#include "utils/json/include/json/json.h"
#include "utils/misc/include/Logger.hpp"
#include "framework/components/ChannelSupervisor/Tube/include/IChannelSupervisorObserver.hpp"
#include "samples/linux/QTClient/common.h"
#include "samples/linux/Profiles/ProfileAPI/ISeatProfileAPI.hpp"

using AXIS::ChannelSupervisor::IChannelSupervisorTubeObserver;
using AXIS::Profile::IUid;
using AXIS::Profile::ApiUid;
using AXIS::Profile::CProfile;
using AXIS::Profile::CProfileInternal;
using AXIS::Profile::IProfileCallbackProxy;

extern "C" CProfile* createProfile(IUid const& id, AXIS::Service::Uid const& sid, IProfileCallbackProxy* pCbProxy);

class CSeatProfileImpl  : public IChannelSupervisorTubeObserver
                        , public ISeatProfile_API
{

   public:
      // from CProfile
      virtual std::string const & getName(void) const { return mName;}
      virtual UInt32 getVersion()               const { return 1; }
      virtual ApiUid const & getPAPI_UID()      const { return mPapiUid;}

      // from CProfileInternal
      virtual bool wasError() const
      {
         return false;
      }

      virtual CError getError() const
      {
         return CError::NoError("CSeatProfileImpl");
      }

      virtual void onEnabledChange();

      //from ISeatProfile_API
      virtual bool processEvent(Event event,  bool need_send = false);

      //from IChannelSupervisorTubeObserver
      virtual void dataReceivedCallback(const UInt32 channel_id, const UInt32 read_size, const UInt32 free_size);
      virtual void bufferOverflowCallback(const UInt32 channel_id);
      virtual void channelDeletedCallback(const UInt32 channel_id);
      virtual void connectionLostCallback();

   protected:
      friend CProfile* createProfile(IUid const& id, AXIS::Service::Uid const& sid, IProfileCallbackProxy*);
      CSeatProfileImpl(IUid const&  profile_id, AXIS::Service::Uid const& service_id, IProfileCallbackProxy* pCbProxy);
      virtual ~CSeatProfileImpl();

   private:

      void resetState();
      void sendUpdate();
      void saveState();
      void restoreState();

      int                              mHeater;
      int                              mHeaterPass;
      int                              mCurrentSeatView;
      int                              mAndroidCurrentSeatView;
      int                              mDriverBottomX;
      int                              mDriverBottomY;
      int                              mDriverBackAngle;
      int                              mPassenBottomX;
      int                              mPassenBottomY;
      int                              mPassenBackAngle;
      Json::Value                      mJSONstate;
      UInt32                           mChannelID;
      ApiUid                           mPapiUid;
      std::string                      mName;
      ISeatProfile_API::Callbacks*     mpAppCallbacks;
      static Logger                    msLogger;

};

extern "C" CProfile* createProfile(IUid const& id, AXIS::Service::Uid const& sid, IProfileCallbackProxy* pCbProxy)
{
   CSeatProfileImpl* p = new CSeatProfileImpl(id, sid, pCbProxy);
   return p;
}
#endif
