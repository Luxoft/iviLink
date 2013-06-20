/* 
 * 
 * iviLINK SDK, version 1.2
 * http://www.ivilink.net
 * Cross Platform Application Communication Stack for In-Vehicle Applications
 * 
 * Copyright (C) 2013, Luxoft Professional Corp., member of IBS group
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
 * 
 */


#ifndef FOUND_DEVICE_HPP
#define FOUND_DEVICE_HPP

#include "BaseUid.hpp"
#include "Serialize.hpp"
#include "byteOrder.hpp"

#include <ctime>

namespace iviLink
{

enum ConnectionType
{
    CON_BLUETOOTH = 0,
    CON_IP,
    CON_UNKNOWN
};

enum DeviceOs
{
    OS_ANDROID = 0,
    OS_UBUNTU,
    OS_IOS,
    OS_OTHER
};

class FoundDevice
{
public:
    std::string mName;
    std::string mAddress;
    ConnectionType mConnection;
    BaseUid mUid;
    time_t mLastSeenTime;
    DeviceOs mOS;

    void deserialize(UInt8* data)
    {
   	    int pos = 0;
        mName = iviLink::bufferToString(data);
        pos = iviLink::stringInBufSize(mName);

        mAddress = iviLink::bufferToString(data + pos);
        pos += iviLink::stringInBufSize(mAddress);

        mConnection = static_cast<ConnectionType>(ByteOrder::ntoh32(*(reinterpret_cast<UInt32*>(data + pos))));
        pos += sizeof(UInt32);

        mUid = iviLink::bufferToString(data + pos);
        pos += iviLink::stringInBufSize(mUid.value());

        mLastSeenTime = static_cast<time_t>(ByteOrder::ntoh64(*(reinterpret_cast<UInt64*>(data + pos))));
        pos += sizeof(UInt64);

        mOS = static_cast<DeviceOs>(ByteOrder::ntoh32(*(reinterpret_cast<UInt32*>(data + pos))));
    }

    UInt8 *serialize()
    {
  	    UInt32 size = getSerializedSize();
        UInt8 * result = new UInt8[size];
        int pos = 0;
        iviLink::stringToBuffer(mName, result);
        pos += iviLink::stringInBufSize(mName);
        iviLink::stringToBuffer(mAddress, result + pos);
        pos += iviLink::stringInBufSize(mAddress);
        *(reinterpret_cast<UInt32*>(result+pos))=ByteOrder::hton32(mConnection);
        pos += sizeof(UInt32);
        iviLink::stringToBuffer(mUid.value(), result + pos);
        pos += iviLink::stringInBufSize(mUid.value());
        *(reinterpret_cast<UInt64*>(result+pos))=ByteOrder::hton64(static_cast<UInt64>(mLastSeenTime));
        pos += sizeof(UInt64);
        *(reinterpret_cast<UInt32*>(result+pos))=ByteOrder::hton32(mOS);
        pos += sizeof(UInt32);
        return result;
    }

    UInt32 getSerializedSize()
    {
        UInt32 size = iviLink::stringInBufSize(mName)
                    + iviLink::stringInBufSize(mAddress)
                    + sizeof(UInt32)
                    + iviLink::stringInBufSize(mUid.value())
                    + sizeof(UInt64)
                    + sizeof(UInt32);
        return size;
    }

    virtual ~FoundDevice() { }

protected:

};

}

#endif
