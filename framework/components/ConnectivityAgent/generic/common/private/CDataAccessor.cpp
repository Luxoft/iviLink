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


/********************************************************************
 *
 * System includes
 *
 ********************************************************************/
#include <cassert>
#include <cstring>
#include <unistd.h>
 
/********************************************************************#include <cassert>
 *
 * Forward declaration includes
 *
 ********************************************************************/
 /********************************************************************
 *
 * The class includes
 *
 ********************************************************************/
#include "CDataAccessor.hpp"
/********************************************************************
 *
 * The other includes
 *
 ********************************************************************/
#include "Logger.hpp"

using namespace iviLink::ConnectivityAgent;
Logger CDataAccessor::logger = Logger::getInstance(LOG4CPLUS_TEXT("ConnectivityAgent.CDataAccessor"));

CDataAccessor::CDataAccessor():
mOperation(0),
mChannelID(0),
mDataSize(0),
mErrorCode(0),
mpData(NULL)
{
}

CDataAccessor::CDataAccessor(const UInt8* pData, const UInt32 size):
mOperation(0),
mChannelID(0),
mDataSize(0),
mErrorCode(0),
mpData(NULL)
{
   assert(size >= 12);
   if (NULL != pData)
   {
      memcpy(&mOperation,pData, sizeof(UInt32) );
      pData+= sizeof(UInt32);
      memcpy(&mChannelID,pData, sizeof(UInt32) );
      pData+= sizeof(UInt32);
      memcpy(&mDataSize,pData, sizeof(UInt32) );
      pData+= sizeof(UInt32);
      memcpy(&mErrorCode,pData, sizeof(UInt32) );
      pData+= sizeof(UInt32);
      LOG4CPLUS_INFO(logger, "CDataAccessor::CDataAccessor(): mDataSize = "
            + convertIntegerToString(mDataSize) + "  size = "
            + convertIntegerToString(size));
      if (mDataSize)
      {
         mpData = new UInt8[mDataSize];
         memset(mpData, 0,mDataSize );
         memcpy(mpData,pData,mDataSize );
      }
   }

}

CDataAccessor::~CDataAccessor()
{
   LOG4CPLUS_INFO(logger, "CDataAccessor::~CDataAccessor(): mpData " + convertIntegerToString((intptr_t)mpData));
   if (mpData!=NULL)
   {
      delete[] mpData;
      mpData = NULL;
   }
}

CDataAccessor::CDataAccessor(const CDataAccessor& accessor) :
   mOperation(accessor.mOperation),
   mChannelID(accessor.mChannelID),
   mDataSize(accessor.mDataSize),
   mErrorCode(accessor.mErrorCode),
   mpData(NULL)
{
   if (mDataSize)
   {
      mpData = new UInt8[mDataSize];
      memcpy(mpData,accessor.mpData,mDataSize );
   }
}

CDataAccessor& CDataAccessor::operator=(const CDataAccessor& accessor)
{
   if (this != &accessor)
   {
      mOperation = accessor.mOperation;
      mChannelID = accessor.mChannelID;
      mDataSize = accessor.mDataSize;
      mErrorCode = accessor.mErrorCode;
      if (mpData != NULL)
      {
         delete[] mpData;
         mpData = NULL;
      }
      if (mDataSize)
      {
         mpData = new UInt8[mDataSize];
         memcpy(mpData,accessor.mpData,mDataSize );
      }
   }
  return *this;
}

UInt8* CDataAccessor::getData()
{
   return mpData;
}

void CDataAccessor::setData(UInt8 const* pData, UInt32 size)
{
   if((NULL != pData)&&(size >0))
   {
      if (mpData != NULL)
      {
         delete[] mpData;
      }
      mpData = new UInt8[size];
      mDataSize = size;
      memset(mpData, 0,size );
      memcpy(mpData,pData, size);
   }
}

void CDataAccessor::resetData()
{
   mDataSize = 0;
   delete[] mpData;
   mpData = NULL;
}

void CDataAccessor::copyToRawArray(UInt8* pBuf)
{
      memcpy(pBuf, &mOperation, sizeof(UInt32) );
      pBuf+= sizeof(UInt32);
      memcpy(pBuf, &mChannelID, sizeof(UInt32) );
      pBuf+= sizeof(UInt32);
      memcpy(pBuf, &mDataSize, sizeof(UInt32) );
      pBuf+= sizeof(UInt32);
      memcpy(pBuf, &mErrorCode, sizeof(UInt32) );
      pBuf+= sizeof(UInt32);
      if (mDataSize > 0)
      {
         memcpy(pBuf,mpData,mDataSize );
      }
}
void CDataAccessor::printContent()
{
   LOG4CPLUS_INFO(logger, "========CDataAccessor::printContent()==============");
   LOG4CPLUS_INFO(logger, 
      "Operation  = " + convertIntegerToString(mOperation) + " \n "
      "Error code = " + convertIntegerToString(mErrorCode) + " \n "
      "Channel ID = " + convertIntegerToString(mChannelID) + " \n "
      "Data size  = " + convertIntegerToString(mDataSize));
   // if (mDataSize < 1024)
   // {
   //    LOG4CPLUS_INFO(logger, "Data:  ");
   // }

   LOG4CPLUS_INFO(logger, "=======================END=========================");
}
