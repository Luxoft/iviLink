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


#include <pthread.h>
#include <stdlib.h>
#include <cstring>
#include <string>

#include "framework/components/ChannelSupervisor/Tube/include/ChannelSupervisorTube.hpp"
#include "utils/misc/include/CError.hpp"
#include "utils/misc/include/UID.hpp"
#include "CSeatProfileImpl.hpp"

Logger CSeatProfileImpl::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("samples.Profiles.CSeatProfileImpl"));

CSeatProfileImpl::CSeatProfileImpl(IUid const&  profile_id, AXIS::Service::Uid const& service_id, IProfileCallbackProxy* pCbProxy):
   ISeatProfile_API(profile_id, service_id), 
    mHeater(0),mHeaterPass(0),mCurrentSeatView(0), mDriverBottomX(0), mDriverBottomY(0), mDriverBackAngle(20),
    mPassenBottomX(0),mPassenBottomY(0), mPassenBackAngle(20),mChannelID(0),
    mPapiUid("SeatProfile_PAPI_UID"),mName("SeatProfileImpl"), 
    mpAppCallbacks(static_cast<ISeatProfile_API::Callbacks*>(pCbProxy))
{

}

CSeatProfileImpl::~CSeatProfileImpl() 
{
   
}

void CSeatProfileImpl::onEnabledChange()
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
   if (isEnabled())
   {
      CError ret = AXIS::ChannelSupervisor::allocateChannel(this,"CSeatProfileImpl", mChannelID);
      if(ret.isNoError())
      {
         LOG4CPLUS_INFO(msLogger, "CSeatProfileImpl::CSeatProfileImpl():  Channel allocated, starting the communication...");
      }
      else
      {
          LOG4CPLUS_INFO(msLogger, "CSeatProfileImpl::CSeatProfileImpl(): allocate Channel failed error "
                + convertIntegerToString(ret.getCode()));
      }
   }
   else
   {
      AXIS::ChannelSupervisor::deallocateChannel(mChannelID,"CSeatProfileImpl");
   }
}

void CSeatProfileImpl::dataReceivedCallback(const UInt32 channel_id, const UInt32 read_size, const UInt32 free_size)
{
   LOG4CPLUS_TRACE(msLogger, "CSeatProfileImpl::dataReceivedCallback() available " +
         convertIntegerToString(read_size) + " bytes for " +
         convertIntegerToString(channel_id) + " channel ");
   char *buf = new char[read_size+1];

   UInt32 received_size = 0;

   CError ret = AXIS::ChannelSupervisor::receiveData(channel_id, "CSeatProfileImpl",(UInt8*)buf, received_size,read_size+1 );
   if(ret.isNoError())
   {
      mpAppCallbacks->messageReceived(mName, std::string(buf));
   }
   else
   {
        LOG4CPLUS_INFO(msLogger, "CSeatProfileImpl::dataReceivedCallback()=> error obtaining data for "+
              convertIntegerToString(channel_id) + " channel ");
   }
   delete [] buf;
}

void CSeatProfileImpl::bufferOverflowCallback(const UInt32 channel_id)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, std::string(__PRETTY_FUNCTION__)
         + "for " + convertIntegerToString(channel_id) + "channel");
}

void CSeatProfileImpl::channelDeletedCallback(const UInt32 channel_id)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, std::string(__PRETTY_FUNCTION__) +
         " - " + convertIntegerToString(channel_id) + "channel deleted!");
   AXIS::ChannelSupervisor::deallocateChannel(mChannelID, "CSeatProfileImpl");
}

void CSeatProfileImpl::connectionLostCallback()
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
}

bool CSeatProfileImpl::processEvent(Event event,  bool need_send )
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

   bool ret = true;

   switch(event.eventName)
   {
      case SEAT_HEATER_TOGGLE:
      {
         //m_airCondState.state = SEAT;
         if (mHeater > 0)
         {
            --mHeater;
         }
         else if (mHeater <= 0)
         {
            mHeater = 3; //max val for heater, another magic number ;)
         }
         break;
      }
      case SEAT_HEATER_PASS_TOGGLE:
      {
         if (mHeaterPass > 0)
         {
            --mHeaterPass;
         }
         else if (mHeaterPass <= 0)
         {
            mHeaterPass = 3;   //max val for heater, another magic number ;)
         }
         break;
      }
      case BOTTOM_UP_TOGGLE:
      {
         if (mCurrentSeatView == 0)
         {
            //driver
            if(++mDriverBottomY > LIFT_MAX)
               --mDriverBottomY;
         }
         else if (mCurrentSeatView == 1)
         {
            //passen
            if(++mPassenBottomY > LIFT_MAX)
               --mPassenBottomY;
         }
         else
         {
            //unkn. 
            if(++mDriverBottomY > LIFT_MAX)
               --mDriverBottomY;
         }
         break;
      }
      case BOTTOM_DOWN_TOGGLE:
      {
         if (mCurrentSeatView == 0)
         {
            //driver
            //m_airCondState.driver_bottom_y --;
            if(--mDriverBottomY < LIFT_MIN)
               ++mDriverBottomY;
         }
         else if (mCurrentSeatView == 1)
         {
            //passen
            if(--mPassenBottomY < LIFT_MIN)
               ++mPassenBottomY;
         }
         else
         {
            //unkn. 
            if(--mDriverBottomY < LIFT_MIN)
               ++mDriverBottomY;
         }
         break;
      }
      case BOTTOM_LEFT_TOGGLE:
      {

         if (mCurrentSeatView == 0)
         {
            //driver
            if(--mDriverBottomX < RIGHT_MAX)
               ++mDriverBottomX;
         }
         else if (mCurrentSeatView == 1)
         {
            //passen
            if(--mPassenBottomX < RIGHT_MAX)
               ++mPassenBottomX;
         }
         else
         {
            //unkn. 
            if(--mDriverBottomX < RIGHT_MAX)
               ++mDriverBottomX;
         }
         break;
      }      
      case BOTTOM_RIGHT_TOGGLE:
      {
         if (mCurrentSeatView == 0)
         {
            //driver
            if(++mDriverBottomX > LEFT_MAX)
               --mDriverBottomX;
         }
         else if (mCurrentSeatView== 1)
         {
            //passen
            if(++mPassenBottomX > LEFT_MAX)
               --mPassenBottomX;
         }
         else
         {
            //unkn. 
            if(++mDriverBottomX > LEFT_MAX)
               --mDriverBottomX;
         }
         break;
      }      
      case BACK_LEFT_TOGGLE:
      {
         if (mCurrentSeatView == 0)
         {
            std::cout << "CURRENT SEAT: DRIVER" << std::endl;
            //driver
            if(mDriverBackAngle - 5 > ANGLE_MIN)
               mDriverBackAngle -= 5;
         }
         else if (mCurrentSeatView == 1)
         {
            std::cout << "CURRENT SEAT: PASSEN" << std::endl;
            //passen
            if(mPassenBackAngle - 5 > ANGLE_MIN)
               mPassenBackAngle -= 5;
         }
         else
         {
            std::cout << "CURRENT SEAT: DRIVER - BY DEFAULT" << std::endl;
            //unkn. 
            if(mDriverBackAngle - 5 > ANGLE_MIN)
               mDriverBackAngle -= 5;
         }
         break;
      }   
      case BACK_RIGHT_TOGGLE:
      {
         if (mCurrentSeatView == 0)
         {
            //driver
            if(mDriverBackAngle + 5 < ANGLE_MAX)
               mDriverBackAngle += 5;
         }
         else if (mCurrentSeatView == 1)
         {
            //passen
            if(mPassenBackAngle + 5 < ANGLE_MAX)
               mPassenBackAngle += 5;
         }
         else
         {
            //unkn. 
            if(mDriverBackAngle + 5 < ANGLE_MAX)
               mDriverBackAngle += 5;
         }
         break;
      }      
      case DRIVER_SEAT_TOGGLE:
      {
         mCurrentSeatView = 0;
         break;
      }      
      case PASSEN_SEAT_TOGGLE:
      {
         mCurrentSeatView = 1;
         break;
      }
      case DRV_PROFILE_APPLIED:
      {
         mDriverBottomX   = event.seat_x;
         mDriverBottomY   = event.seat_y;
         mDriverBackAngle = event.seat_angle;
         break;
      }
      case PASSEN_PROFILE_APPLIED:
      {
         mPassenBottomX   = event.seat_x;
         mPassenBottomY   = event.seat_y;
         mPassenBackAngle = event.seat_angle;
         break;
      }
       case ACTIVITY_SEAT:
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

void CSeatProfileImpl::sendUpdate()
{
   
   LOG4CPLUS_TRACE(msLogger, "CSeatProfileImpl::sendUpdate()");
   Json::StyledWriter writer;
   const char* buffer = (writer.write(mJSONstate)).c_str();

   CError ret = AXIS::ChannelSupervisor ::sendData(mChannelID,"CSeatProfileImpl",(UInt8*)(buffer), strlen(buffer));
   if (!ret.isNoError())
   {
       LOG4CPLUS_INFO(msLogger, "CSeatProfileImpl::sendUpdate()=>  send failed, error " +
             convertIntegerToString(ret.getCode()));
   }
}

void CSeatProfileImpl::saveState()
{
   mJSONstate[seatBackDriver]   = mDriverBackAngle;
   mJSONstate[seatBottomDriver] = mDriverBottomX;
   mJSONstate[seatLiftDriver]   = mDriverBottomY;
   mJSONstate[seatBackPass]     = mPassenBackAngle;
   mJSONstate[seatBottomPass]   = mPassenBottomX;
   mJSONstate[seatLiftPass]     = mPassenBottomY;
   mJSONstate[activeSeat]       = mCurrentSeatView;
   mJSONstate[heater]           = mHeater;
   mJSONstate[heater_pass]      = mHeaterPass;
   Json::StyledWriter writer;
   const char* buffer = (writer.write(mJSONstate)).c_str();
   mpAppCallbacks->messageReceived(mName, std::string(buffer));
}
