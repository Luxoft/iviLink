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


#ifndef BLUETOOTHCARRIERADAPTER_HPP_
#define BLUETOOTHCARRIERADAPTER_HPP_

#include "Logger.hpp"
#include "CThread.hpp"
#include "CCondVar.hpp"
#include "CRWMutex.hpp"
#include "CMutex.hpp"
#include "CCarrierAdapter.hpp"
#include "Frame.hpp"
#include "ConnectionInfo.hpp"


namespace iviLink 
{
namespace ConnectivityAgent 
{
namespace HAL 
{

/// BluetoothAdapter class
/**
 * Class is used to manage connection by RFCOMM socket.
 * It encapsulates connection establishing and data transfer.
 *
 * BluetoothAdapter creates separate thread and uses select() to accept new
 * incoming connections or wait for incoming data.
 *
 */
class BluetoothAdapter   : public CCarrierAdapter
                         , public CThread
{
private:
    static Logger logger;

    /**
    * Friend is used because this class has private constructor
    */
    friend class BluetoothConnectionFinder;

public:
    // Methods section

    virtual ~BluetoothAdapter();

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
    explicit BluetoothAdapter(const CBluetoothInfo & connectionInfo);
    /**
    * Sets current local address
    */
   void setLocalAddress();


private:
    // Members section
    /// String representation of local MAC-address, known to remote side.
    /// Returned by getLocalAddress() method.
    char *mLocalAddr;

    /// String representation of remote MAC-address.
    /// Returned by getRemoteAddress() method.
    char *mRemoteAddr;

    /// File descriptor of socket used for data exchange
    int mSocket;

    /// Data structure used by ::select()
    fd_set mRFdSet;

    /// Mutex used to block access to sockets
    mutable CRWMutex mSocketMutex;

    /// Mutex used to block access to HCI
    mutable CMutex mHciMutex;

    /// Information used to establish connection
    const CBluetoothInfo mConnectionInfo;

	bool mBroken;

    /// Signals which can be send to working thread using selfpipe
    enum ESignals
    {
        eGameOver  ///< connection lost, thread must stop
    };
    /**
    * Helper class used to pass signals to working thread and to wake ::select()
    * from its blocked waiting.
    */
    class Selfpipe
    {
    public:
        typedef ESignals tSignal;
    public:
        Selfpipe();
        ~Selfpipe();

        ConnectivityAgentError setup();
        void destroy();

        bool isReady() const;

        int getReadingEnd() const;
        ConnectivityAgentError signal(tSignal signal);
        ConnectivityAgentError readSignal(tSignal& signal);

    private:
        bool mReady;
        int mSelfpipe[2];
    private:
        static Logger msLogger;
    };

    Selfpipe mSelfpipe;
};

}  // namespace HAL
}  // namespace ConnectivityAgent
}  // namespace AXIS


#endif /* BLUETOOTHCARRIERADAPTER_HPP_ */
#endif //ANDROID
