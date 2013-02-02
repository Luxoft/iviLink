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


#ifndef ANDROID
 #else


#ifndef CANDROIDBLUETOOTHCARRIERADAPTER_HPP_
#define CANDROIDBLUETOOTHCARRIERADAPTER_HPP_

#include <jni.h>
#include "Logger.hpp"
#include "JniThreadHelper.hpp"
#include "CThread.hpp"
#include "CCondVar.hpp"
#include "CCarrierAdapter.hpp"
#include "Frame.hpp"
#include "ConnectionInfo.hpp"
 #include <deque>

namespace iviLink {
namespace ConnectivityAgent {
namespace HAL {

/// CTcpCarrierAdapter class
/**
 * Class is used to manage connection by TCP/IP socket.
 * It encapsulates connection establishing and data transfer.
 *
 * Connection is established when newly created object of class is not "broken" 
 * (isBroken()).
 *
 * CTcpCarrierAdapter creates separate thread and uses select() to accept new
 * incoming connections or wait for incoming data.
 *
 */
class CAndroidBluetoothAdapter   : public CCarrierAdapter
                           , public CThread
{
private:
   static Logger logger;

   /**
    * Friend is used because this class has private constructor
    */
   friend class CAndroidBluetoothConnectionFinder;

public:
   // Methods section

   virtual ~CAndroidBluetoothAdapter();

public:
   // from CCarrierAdapter
   virtual bool isBroken() const;
   virtual ConnectivityAgentError makeHandshake();
   virtual const char* getRemoteAddress() const;
   virtual const char* getLocalAddress() const;
   virtual const char* getTypeName() const;

protected:
   // Methods section
   // from CCarrierAdapter

   virtual ConnectivityAgentError sendPreparedArray(const UInt8* pArray, const UInt32 size);
   virtual ConnectivityAgentError receiveRawArray(UInt8* pArray, UInt32 &size);
   
private:
   // from CThread
   // Next methods are hidden

   virtual bool start();
   virtual bool stop();
   virtual void threadFunc();

private:
   // Methods section

   /**
    * Constructor
    * @param connectionInfo is used to establish connection
    */
   explicit CAndroidBluetoothAdapter(const CBluetoothInfo & connectionInfo);


private:
   // Members section
   JavaVM * mpJM;
   jobject jImplementation;
   jmethodID receive;
   jmethodID send;

   jmethodID midIsBroken;

   /// Information used to establish connection
   const CBluetoothInfo mConnectionInfo;
};

}  // namespace HAL
}  // namespace ConnectivityAgent
}  // namespace AXIS


#endif /* CTCPCARRIERADAPTER_HPP_ */
#endif //ANDROID
