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

#ifndef BLUETOOTH_FINDER_HPP
#define BLUETOOTH_FINDER_HPP

#include "CConnectionFinder.hpp"
#include "Logger.hpp"
#include "Types.hpp"
#include "ConnectionInfo.hpp"
#include "CRWMutex.hpp"
#include <unistd.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/sdp.h>
#include <bluetooth/sdp_lib.h>
#include <bluetooth/hci.h>

namespace iviLink
{
namespace ConnectivityAgent
{
namespace HAL
{
/// BluetoothConnectionFinder class
/**
*
*/
class BluetoothConnectionFinder : public CConnectionFinder
{
private:
    static Logger logger;
            
    bool mIsValid;

    IFoundConnectionHandler * handler;

    // List of discovered devices
    inquiry_info *mListOfDevices;

    // Local BT adapter ID
    int mAdapterId;

    // BT device descriptor
    int mDevDescr; 

    // Timeout of inquiry request
    int mTimeoutDiscovery; // The inquiry lasts for at most (1.28 * mTimeoutDiscovery) seconds

    // indicates whether or not to use previously discovered device information or to start afresh
    int mFlags; 

    // iviLink service UUID
    uuid_t mIviLinkUUID;

    // RFCOMM channel discovered on remote device with iviLink
    UInt8 *mDiscoveredIviLinkChannel;

    // Local SDP session, used to set iviLink service 
    sdp_session_t *mLocalSdpSession;

    // Local BT device MAC-address, visible to other devices
    bdaddr_t *mLocalDevAddr;

    // Local BT device name, visible to other devices
    char *mLocalDevName;

    // Remote BT device MAC-address with iviLink
    bdaddr_t *mRemoteDevAddr;

    // Remote BT device name with iviLink
    char *mRemoteDevName;

    // NULL BT address used to make SDP requests
    bdaddr_t mAnyAddress;    

    // Used to make requests for local device info
    bdaddr_t mLocalAddress; 

    // Flag that sets =true, if iviLink service registered 
    bool mRegSvc;   

    /// Mutex used to block access to sockets
    mutable CRWMutex mSocketMutex;

    /// Mutex used to block access to HCI
    CMutex mHciMutex;

public:
    // Methods section

    /**
    * Constructor
    * @param handler object which will process found connections
    * @param gender gender of connection
    */
    BluetoothConnectionFinder(IFoundConnectionHandler& handler, EGenderType gender);

    /**
    * Destructor
    */
    virtual ~BluetoothConnectionFinder();

protected:
    // Methods section
    // from CConnectionFinder

    virtual void performSearch();

private:
    /**
    * Searching iviLink service on the remote device and discover RFCOMM channel
    * @param DeviceAddress remote device address
    * @retval discovered RFCOMM channel
    */
    UInt8* discoverIviLinkChannel(const bdaddr_t &DeviceAddress);
    /**
    * Register iviLink service on local BT device
    * @retval pointer to local SDP session 
    */
    sdp_session_t* registerIviLinkService();
    /**
    * BT connection establishing
    * @param info struct of connection info, that used to create object of BluetoothAdapter
    * @param remoteAddr remote device address to connect 
    * @retval true if connection estableshed
    * @retval false otherwise 
    */
    bool connectionEstablish(CBluetoothInfo &info, const bdaddr_t &remoteAddr);
    /**
    * Server connection establishing
    * @param clientSock socket of accepted connection 
    * @retval true if connection estableshed
    * @retval false otherwise 
    */
    bool createSrvConnection(int &clientSock);
    /**
    * Client connection establishing
    * @param sock connection socket
    * @retval true if connection estableshed
    * @retval false otherwise 
    */
    bool createClientConnection(int &sock);
    /**
    * Converting from bdaddr_t to std::string
    * @param bdAddr address to convert
    * @retval string representation of address
    */
    std::string convertBDaddrToStr(const bdaddr_t &bdAddr);
    /**
    * Setting local device info: mLocalDevName, mLocalDevAddr
    * @retval true if setting info without errors
    * @retval false otherwise 
    */
    bool setLocalBdInfo();
    /**
    * Set local device discoverable mode
    * @param adapterId local BT device adapter ID
    * @retval true if sucessful set
    * @retval false otherwise 
    */
    bool setDiscoverable(const int &adapterId);
    /**
    * Restart local BT device
    * @param adapterId local BT device adapter ID
    */
    void restartHciDev(const int &adapterId);
            
};
} //HAL
} //ConnectivityAgent
} // iviLink

#endif // BluetoothConnectionFinder

#endif // ANDROID
