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


#ifndef CDATA_ACCESSOR_HPP
#define CDATA_ACCESSOR_HPP
/********************************************************************
 *
 * System includes
 *
 ********************************************************************/

/********************************************************************
 *
 * Other includes
 *
 ********************************************************************/
#include "Types.hpp"
#include "Logger.hpp"
namespace iviLink
{
   namespace ConnectivityAgent
   {

         ///CDataAccessor class
         /**
          * CDataAccessor is used to pack data and unify the protocol of data exchange between Client App
          * and Connectivity agent. It also used for service requests/responces exchange of Connectivity Agents on both sides.
          */

         class CDataAccessor
         {
         private:
            static Logger logger;

         public:

            // Methods section

            /**
             * Default constructor. All fields instantiated by zero
             */
            CDataAccessor();

            /**
             * Parameters constructor to make the Accessor from given byte stream
             * @param pData pointer to bytestream
             * @param size bytestream size
             */

            CDataAccessor(const UInt8* pData, const UInt32 size);

            /**
             * Destructor
             */
            ~CDataAccessor();

            /**
             * Copying constructor
             * @param classRef The reference to the object the class is constructing on
             */
            CDataAccessor(const CDataAccessor& accessor);

            /**
             * Assigning operator
             * @param classRef The reference to the object the class is assiging from
             */
            CDataAccessor& operator=(const CDataAccessor& accessor);

            /**
             * Resets to zero all data inside the Data Accessor
             */
            inline void resetAll()
            {
               mChannelID = 0;
               mOperation = 0;
               mDataSize  = 0;
               mErrorCode = 0;
               if(mpData) delete[] mpData;
               mpData = NULL;
            }

            /**
             * Sets given channel number
             * @param  id the number to set
             */
            inline void setChannelID(const UInt32 id)
            {
               mChannelID = id;
            }
            /**
             * Sets given operation code
             * @param  id the operation code to set
             */
            inline void setOpCode(const UInt32 id)
            {
               mOperation = id;
            }

            inline void setErrorCode(const UInt32 err)
            {
                mErrorCode = err;
            }

            /**
             * gets current channel number
             * @return  current channel number
             */
            inline UInt32 getChannelID() const
            {
               return mChannelID;
            }

            /**
             * gets current operation code
             * @return  current operation code
             */
            inline UInt32 getOpCode() const
            {
               return mOperation;
            }

            inline UInt32 getErrorCode() const 
            {
                return mErrorCode;
            }

            /**
             * gets  internal data buffer size in bytes
             * @return  current data size
             */
            inline UInt32 getDataSize() const
            {
               return mDataSize;
            }

            /**
             * gets overall accessor size in bytes
             * @return  current data size
             */
            inline UInt32 getObjectSize() const
            {
               return mDataSize + 4*(sizeof(UInt32));
            }

            /**
             * gets internal data buffer
             * @return  pointer to internal data buffer
             */
            UInt8* getData();

            /**
             * Copy given bytestream to nternal data buffer
             * @param pData pointer bytestream to copy
             * @param size bytesize to copy
             */
            void setData(UInt8 const* pData, UInt32 size);

            void resetData();

            /**
             * Prints content of DataAccessor for debug purposes
             */
            void printContent();

            void copyToRawArray(UInt8* pBuf);

            private:

            // Members section

            UInt32                           mOperation;
            UInt32                           mChannelID;
            UInt32                           mDataSize;
            UInt32                           mErrorCode;
            UInt8*                           mpData;
         };

   }
}

#endif
