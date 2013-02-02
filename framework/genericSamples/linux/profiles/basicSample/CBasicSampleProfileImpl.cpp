/* 
 * iviLINK SDK, version 1.2
 * http://www.ivilink.net
 * Cross Platform Application Communication Stack for In-Vehicle Applications
 * 
 * Copyright (C) 2012-2013, Luxoft Professional Corp., member of IBS group
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; version 2.1.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 * 
 */ 


#include <stdlib.h>
#include <cstring>
#include <cassert>

#include "CBasicSampleProfileImpl.hpp"

//namespace required for working with receiving and sending data
using namespace iviLink::Channel;

Logger CBasicSampleProfileImpl::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("samples.Profiles.CBasicSampleProfileImpl"));

/**
 * Sending the summands
 */
void CBasicSampleProfileImpl::sendOperands(int a, int b)
{
   LOG4CPLUS_INFO(msLogger, "CBasicSampleProfileImpl::sendOperands(" + convertIntegerToString(a) + ", " + convertIntegerToString(b) + ")");

   UInt8 outgoingData[3];
   Buffer buf(outgoingData, 3);

   //sending the name of procedure and summands
   bool res = true;
   res = res && buf.write(static_cast<UInt8>(SEND_OPERANDS_PROCEDURE));
   res = res && buf.write(static_cast<UInt8>(a));
   res = res && buf.write(static_cast<UInt8>(b));

   if (res)
   {
      iviLink::Error err = iviLink::Channel::sendBuffer(mChannelID, buf);
      if (!err.isNoError())
         LOG4CPLUS_WARN(msLogger, "Unable to sendBuffer: " + static_cast<std::string>(err));
   }
   else
   {
      LOG4CPLUS_WARN(msLogger, "Unable to write data into buffer");
   }
}

/**
 * Sending the obtained result
 */
void CBasicSampleProfileImpl::sendResult(int a)
{
   LOG4CPLUS_INFO(msLogger, "CBasicSampleProfileImpl::sendResult(" + convertIntegerToString(a) + ")");

   UInt8 outgoingData[2];
   Buffer buf(outgoingData, 2);

   //sending the name of procedure and result of operation
   bool res = true;
   res = res && buf.write(static_cast<UInt8>(SEND_RESULT_PROCEDURE));
   res = res && buf.write(static_cast<UInt8>(a));

   if (res)
   {
      iviLink::Error err = iviLink::Channel::sendBuffer(mChannelID, buf);
      if (!err.isNoError())
         LOG4CPLUS_WARN(msLogger, "Unable to sendBuffer: " + static_cast<std::string>(err));
   }
   else
   {
      LOG4CPLUS_WARN(msLogger, "Unable to write data into buffer");
   }
}

CBasicSampleProfileImpl::CBasicSampleProfileImpl(iviLink::Profile::IProfileCallbackProxy* pCbProxy)
   : mChannelID(0)
   , mpAppCallbacks(static_cast<IBasicSampleProfile_API::Callbacks*>(pCbProxy))
{
   Logger logger = Logger::getInstance(LOG4CPLUS_TEXT("samples.BasicSample"));

   PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT("log4cplus.properties"));

   LOG4CPLUS_INFO(msLogger, "CBasicSampleProfileImpl::CBasicSampleProfileImpl");
}

CBasicSampleProfileImpl::~CBasicSampleProfileImpl()
{
   if (mChannelID != 0)
   {
      //deallocating channel after deleting instance 
      deallocateChannel(mChannelID);
      mChannelID = 0;
   }
}


/**
 * Allocating channel
 */
void CBasicSampleProfileImpl::onEnable()
{
   //Allocating channel
   iviLink::Error err =  allocateChannel("CBasicSampleProfileImpl", this, mChannelID, eRealTime);
   if(mChannelID != 0)
   {
      LOG4CPLUS_INFO(msLogger, "CBasicSampleProfileImpl::CBasicSampleProfileImpl(): Channel allocated, starting the communication...");
   }
   else
   {
      LOG4CPLUS_INFO(msLogger, "CBasicSampleProfileImpl::CBasicSampleProfileImpl(): allocate Channel failed");
   }
}

/**
 * Deallocating channel
 */
void CBasicSampleProfileImpl::onDisable()
{
   {
      //Deallocating channel
      deallocateChannel(mChannelID);
   }
}

/**
 * Callback that should be invoked when the data is received
 */
void CBasicSampleProfileImpl::onBufferReceived(const tChannelId channel, Buffer const& buffer)
{
   assert (channel == mChannelID);
   UInt8 procedure;

   bool res = buffer.read(procedure);
   if (!res)
   {
      LOG4CPLUS_WARN(msLogger, "Unable to read procedure from buffer");
      return;
   }

   switch (procedure)
   {
   case SEND_OPERANDS_PROCEDURE:
      {
         UInt8 a, b;
         res = res && buffer.read(a);
         res = res && buffer.read(b);

         if (res)
         {
            LOG4CPLUS_INFO(msLogger, "Operands for edition received");

            LOG4CPLUS_INFO(msLogger, "First operand = " + convertIntegerToString(a));
            LOG4CPLUS_INFO(msLogger, "Second operand = " + convertIntegerToString(b));

            sendResult(a + b);
            mpAppCallbacks->operandsReceived(a,b);
         }
         else
         {
            LOG4CPLUS_WARN(msLogger, "Unable to read operands from buffer");
            mpAppCallbacks->operandsReceived(a,b);
            return;
         }
      }
      break;
   case SEND_RESULT_PROCEDURE:
      {
         UInt8 result;
         res = res && buffer.read(result);

         if (res)
         {
            LOG4CPLUS_INFO(msLogger, "Result of edition received");
            LOG4CPLUS_INFO(msLogger, "Result = " + convertIntegerToString(result));
            mpAppCallbacks->resultReceived(result);
         }
         else
         {
            LOG4CPLUS_WARN(msLogger, "Unable to read operands from buffer");
            mpAppCallbacks->resultReceived(result);
            return;  
         }
      }
      break;
   default:
      LOG4CPLUS_WARN(msLogger, "Unknown procedure code");
   }

}

/**
 * Callback that should be invoked when the channel is deleted
 */
void CBasicSampleProfileImpl::onChannelDeleted(const UInt32 channel_id)
{
   LOG4CPLUS_INFO(msLogger, "CBasicSampleProfileImpl::channelDeletedCallback() - " +
         convertIntegerToString(channel_id) + " channel deleted!");
   //Deallocating channel
   deallocateChannel(mChannelID);
   mChannelID = 0;
}

/**
 * Callback that should be invoked when the connection is lost
 */
void CBasicSampleProfileImpl::onConnectionLost()
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
}

