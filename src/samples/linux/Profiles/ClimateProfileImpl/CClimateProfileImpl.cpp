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


#include <stdlib.h>
#include <cstring>

#include "framework/components/ChannelSupervisor/Tube/include/ChannelSupervisorTube.hpp"
#include "utils/misc/include/CError.hpp"
#include "CClimateProfileImpl.hpp"

Logger CClimateProfileImpl::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("samples.Profiles.CClimateProfileImpl"));

CClimateProfileImpl::CClimateProfileImpl(IUid const&  profile_id, AXIS::Service::Uid const& service_id, IProfileCallbackProxy* pCbProxy):
   IClimateProfile_API( profile_id, service_id), 
   mChannelID(0), mPapiUid("ClimateProfile_PAPI_UID"),mName("ClimateProfileImpl"),
   mpAppCallbacks(static_cast<IClimateProfile_API::Callbacks*>(pCbProxy))
{ 
}

CClimateProfileImpl::~CClimateProfileImpl() 
{   
}

void CClimateProfileImpl::onEnabledChange()
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
   if (isEnabled())
   {
      CError ret = AXIS::ChannelSupervisor::allocateChannel(this, "CClimateProfileImpl",mChannelID);
      if(ret.isNoError())
      {
         LOG4CPLUS_INFO(msLogger, "CClimateProfileImpl::CClimateProfileImpl():  Channel allocated, starting the communication...");
         resetState();
         saveState();
      }
      else
      {
          LOG4CPLUS_INFO(msLogger, "CClimateProfileImpl::CClimateProfileImpl(): allocate Channel failed error " + convertIntegerToString(ret.getCode()));
      }
   }
   else
   {
      AXIS::ChannelSupervisor::deallocateChannel(mChannelID, "CClimateProfileImpl");
   }
}


void CClimateProfileImpl::dataReceivedCallback(const UInt32 channel_id, const UInt32 read_size, const UInt32 free_size)
{
   LOG4CPLUS_TRACE(msLogger, "CClimateProfileImpl::dataReceivedCallback() available "+
         convertIntegerToString(read_size) + " bytes for " + convertIntegerToString(channel_id) + " channel");
   char *buf = new char[read_size+1];

   UInt32 received_size = 0;

   CError ret = AXIS::ChannelSupervisor::receiveData(channel_id, "CClimateProfileImpl", (UInt8*)buf, received_size,read_size+1 );
   if(ret.isNoError())
   {
      mpAppCallbacks->messageReceived(mName, std::string(buf));
   }
   else
   {
      LOG4CPLUS_INFO(msLogger, "CClimateProfileImpl::dataReceivedCallback()=> error obtaining data for "
            + convertIntegerToString(channel_id) + " channel ");
   }
   delete [] buf;
}

void CClimateProfileImpl::resetState()
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
   mAirRecirc   = false;
   mAutoClimate = true;
   mDriverTemp  = 20; // 16 -- 27
   mDualClimate = false;
   mFanSpeed    = 5;  // 0 -- 10
   mFrontWindow = false;
   mIsAC        = true;
   mIsOFF       = false;
   mPassTemp    = 20; // 16 -- 27
   mRearWindow  = false; 
   mFanModeLow  = false;
   mFanModeMiddle= false;
   mFanModeTop  = false;
}

void CClimateProfileImpl::saveState()
{
   mJSONstate[off]         = mIsOFF;
   mJSONstate[tempD]       = mDriverTemp;
   mJSONstate[tempP]       = mPassTemp;
   mJSONstate[fanSpd]      = mFanSpeed;
   mJSONstate[auto_mode]   = mAutoClimate;
   mJSONstate[dual]        = mDualClimate;
   mJSONstate[head]        = mFrontWindow;
   mJSONstate[rear]        = mRearWindow;
   mJSONstate[fanlow]      = mFanModeLow;
   mJSONstate[fanmiddle]   = mFanModeMiddle;
   mJSONstate[fantop]      = mFanModeTop;
   mJSONstate[acStatus]    = mIsAC;
   mJSONstate[recStatus]   = mAirRecirc;
   Json::StyledWriter writer;
   const char* buffer = (writer.write(mJSONstate)).c_str();
   mpAppCallbacks->messageReceived(mName, std::string(buffer));
}
void CClimateProfileImpl::savePrevState()
{
    mPrevState[off]         = mIsOFF;
    mPrevState[tempD]       = mDriverTemp;
    mPrevState[tempP]       = mPassTemp;
    mPrevState[fanSpd]      = mFanSpeed;
    mPrevState[auto_mode]   = mAutoClimate;
    mPrevState[dual]        = mDualClimate;
    mPrevState[head]        = mFrontWindow;
    mPrevState[rear]        = mRearWindow;
    mPrevState[fanlow]      = mFanModeLow;
    mPrevState[fanmiddle]   = mFanModeMiddle;
    mPrevState[fantop]      = mFanModeTop;
    mPrevState[acStatus]    = mIsAC;
    mPrevState[recStatus]   = mAirRecirc;
}
void CClimateProfileImpl::restorePrevState()
{
   mIsOFF         = mPrevState.get(off, false).asBool();
   mDriverTemp    = mPrevState.get(tempD, 0).asInt();
   mPassTemp      = mPrevState.get(tempP, 0).asInt();
   mFanSpeed      = mPrevState.get(fanSpd, 0).asInt();
   mAutoClimate   = mPrevState.get(auto_mode, false).asBool();
   mDualClimate   = mPrevState.get(dual, false).asBool();
   mFrontWindow   = mPrevState.get(head, false).asBool();
   mRearWindow    = mPrevState.get(rear, false).asBool();
   mFanModeLow    = mPrevState.get(fanlow, false).asBool();
   mFanModeMiddle = mPrevState.get(fanmiddle, false).asBool();
   mFanModeTop    = mPrevState.get(fantop, false).asBool();
   mIsAC          = mPrevState.get(acStatus, false).asBool();
   mAirRecirc     = mPrevState.get(recStatus, false).asBool();
}

void CClimateProfileImpl::restoreState()
{
   mIsOFF         = mJSONstate.get(off, false).asBool();
   mDriverTemp    = mJSONstate.get(tempD, 0).asInt();
   mPassTemp      = mJSONstate.get(tempP, 0).asInt();
   mFanSpeed      = mJSONstate.get(fanSpd, 0).asInt();
   mAutoClimate   = mJSONstate.get(auto_mode, false).asBool();
   mDualClimate   = mJSONstate.get(dual, false).asBool();
   mFrontWindow   = mJSONstate.get(head, false).asBool();
   mRearWindow    = mJSONstate.get(rear, false).asBool();
   mFanModeLow    = mJSONstate.get(fanlow, false).asBool();
   mFanModeMiddle = mJSONstate.get(fanmiddle, false).asBool();
   mFanModeTop    = mJSONstate.get(fantop, false).asBool();
   mIsAC          = mJSONstate.get(acStatus, false).asBool();
   mAirRecirc     = mJSONstate.get(recStatus, false).asBool();
}

void CClimateProfileImpl::sendUpdate()
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
   Json::StyledWriter writer;
   const char* buffer = (writer.write(mJSONstate)).c_str();

   CError ret = AXIS::ChannelSupervisor::sendData(mChannelID, "CClimateProfileImpl",(UInt8*)(buffer), strlen(buffer));
   if(!ret.isNoError())
   {
       LOG4CPLUS_INFO(msLogger, "CClimateProfileImpl::sendUpdate()=>  send failed, error " +
             convertIntegerToString(ret.getCode()));
   }
}

void CClimateProfileImpl::bufferOverflowCallback(const UInt32 channel_id)
{
   LOG4CPLUS_INFO(msLogger, "CClimateProfileImpl::bufferOverflowCallback() for " +
         convertIntegerToString(channel_id) + " channel ");
}

void CClimateProfileImpl::channelDeletedCallback(const UInt32 channel_id)
{
   LOG4CPLUS_INFO(msLogger, "CClimateProfileImpl::channelDeletedCallback() - " +
         convertIntegerToString(channel_id) + " channel deleted!");
   AXIS::ChannelSupervisor::deallocateChannel(mChannelID, "CClimateProfileImpl");
}

void CClimateProfileImpl::connectionLostCallback()
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
}

bool CClimateProfileImpl::processEvent(Event event, bool need_send)
{
   LOG4CPLUS_INFO(msLogger, "CClimateProfileImpl::processEvent():"
         + convertIntegerToString((int)event.eventName));

   bool ret = true;

   switch(event.eventName)
   {
      case FAN_INC_TOGGLE:
      {
         //auto goes off
         mAutoClimate = false;
         
         if (!mIsOFF) //if not off
         {
            if (++mFanSpeed > FAN_MAX_SPEED) --mFanSpeed;
         }
         else  //if off then turn on
         {
            restorePrevState();
            mFanSpeed = 1;  //set fan to 0 + 1 speed
            mIsOFF = false;
         }
         break;
      }
      case FAN_DECR_TOGGLE:
      {
         mAutoClimate = false;
         if (--mFanSpeed < FAN_MIN_SPEED) ++mFanSpeed;
         break;
      }
      case DRIV_TEMP_INC_TOGGLE:
      {
         //check if dual or sync. climate
         if (mDualClimate)  //dual.
         {
            if(++mDriverTemp > TEMP_MAX) --mDriverTemp;
         }
         else  //sync.
         {
            if(++mDriverTemp > TEMP_MAX) --mDriverTemp;
            else
               ++mPassTemp;
         }
         break;
      }
      case DRIV_TEMP_DECR_TOGGLE:
      {
         //check if dual or sync. climate
         if (mDualClimate)  //dual.
         {
            if(--mDriverTemp < TEMP_MIN) ++mDriverTemp;
         }
         else  //sync.
         {
            if(--mDriverTemp < TEMP_MIN) ++mDriverTemp;
            else
               --mPassTemp;
         }
         break;
      }
      case PASS_TEMP_INC_TOGGLE:
      {
         mDualClimate = true;
         mAutoClimate = false;
         if (++mPassTemp > TEMP_MAX) --mPassTemp;
         break;
      } 
      case PASS_TEMP_DECR_TOGGLE:
      {
         mDualClimate = true;
         mAutoClimate = false;
         if (--mPassTemp < TEMP_MIN) ++mPassTemp;
         break;
      }
      case AC_TOGGLE:
      {
         mIsAC = !(mIsAC);
         mAutoClimate = false;
         break;
      }
      case FRONT_W_TOGGLE:
      {
         mFrontWindow = !(mFrontWindow);
         break;
      }
      case REAR_W_TOGGLE:
      {
         mRearWindow = !(mRearWindow);
         break;
      }
      case OFF_TOGGLE:
      {
         if (!mIsOFF)
         {
            savePrevState();
            mAirRecirc = false;
            mAutoClimate = false;
            mDualClimate = false;
            mFanSpeed = 0; // 0 -- 10
            mFrontWindow = false;
            mIsAC = false;
            mIsOFF = true;
            mRearWindow = false;
            mFanModeLow  = false;
            mFanModeMiddle= false;
            mFanModeTop  = false;
            mState = CLIMATE;
            mStateChanged = false;
         }
         else
         {
            restorePrevState();
         }         
         break;
      }
      case DUAL_TOGGLE:
      {
         if(mDualClimate)
         {
            //turn dual off 
            mPassTemp = mDriverTemp;
            mDualClimate = false;
            //auto goes off
            mAutoClimate = false;
         }
         else
         {
            mDualClimate = true;
            //auto goes off
            mAutoClimate = false;
         }
         break;
      }
      case AUTO_TOGGLE:
      {
         if (!mAutoClimate) //if it was off
         {
            resetState();
            mDriverTemp    = mJSONstate.get(tempD, 0).asInt();
            mPassTemp      = mJSONstate.get(tempP, 0).asInt();
         }
         else
         {
            mAutoClimate = false;
         }
         break;
      }
      case RECIRCULATION_TOGGLE:
      {
         if (!mAutoClimate) //
         {
            //setDefaultState();
            mAirRecirc = !(mAirRecirc);
         }
         else
         {
            mAirRecirc = !(mAirRecirc);
            mAutoClimate = false;
         }
         break;
      }
      case FAN_MODE_TOP_TOGGLE:
      {
         if (!mAutoClimate) //
         {
            mFanModeTop = !(mFanModeTop);
         }
         else
         {
            mFanModeTop = !(mFanModeTop);
            mAutoClimate = false;
         }
         break;
      }
      case FAN_MODE_MIDDLE_TOGGLE:
      {
         if (!mAutoClimate) //
         {
            mFanModeMiddle = !(mFanModeMiddle);
         }
         else
         {
            mFanModeMiddle = !(mFanModeMiddle);
            mAutoClimate = false;
         }
         break;
      }
      case FAN_MODE_LOW_TOGGLE:
      {
         if (!mAutoClimate) //
         {
            mFanModeLow = !(mFanModeLow);
         }
         else
         {
            mFanModeLow = !(mFanModeLow);
            mAutoClimate = false;
         }
         break;
      }
      case ACTIVITY_AC:
      {
           break;
      }

      default:
      {
         ret = false;
         break;
      }
      
   }

   if(ret)
   {
      saveState();
      if(need_send)
      {
         sendUpdate();

      }
   }

   return ret;
}
