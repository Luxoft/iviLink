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

#include "BluetoothConnectionFinder.hpp"
#include "BluetoothAdapter.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/rfcomm.h>

#define RFCOMM_CHANNEL 11
#define BDADDR_SIZE 32


using namespace iviLink::ConnectivityAgent::HAL;


Logger BluetoothConnectionFinder::logger = Logger::getInstance(LOG4CPLUS_TEXT("ConnectivityAgent.HAL.BluetoothConnectionFinder"));


BluetoothConnectionFinder::BluetoothConnectionFinder(IFoundConnectionHandler& handler, EGenderType gender)
:
    CConnectionFinder(handler, gender),
    mListOfDevices(NULL),
    mDevDescr(-1),
    mTimeoutDiscovery(8),
    mLocalDevAddr(NULL),
    mLocalDevName(NULL),
    mRemoteDevAddr(NULL),
    mRemoteDevName(NULL),
    mRegSvc(false),
    mLocalSdpSession(NULL),
    mDiscoveredIviLinkChannel(NULL)
{
    LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
    
    mFlags = NULL;   //set to IREQ_CACHE_FLUSH if you want to flush results of previous inquiry    

    mAnyAddress = {{0, 0, 0, 0, 0, 0}};
    mLocalAddress = {{0, 0, 0, 0xff, 0xff, 0xff}};    

    mAdapterId = hci_get_route(NULL); // get BT adapter ID
    
    if (0 > mAdapterId) 
    {
        LOG4CPLUS_ERROR(logger, "!!!!!!No bluetooth adapter or fail to get adapter id");
        mIsValid = false;
    }
    else
    {
        LOG4CPLUS_INFO(logger, "BT adapter Id = " + convertIntegerToString((int)mAdapterId));
        mIsValid = true;
    }        
    
}


BluetoothConnectionFinder::~BluetoothConnectionFinder()
{
    LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);    
    // close local sdp session
    sdp_close(mLocalSdpSession);
    hci_close_dev(mDevDescr);

    delete[] mRemoteDevName;
    mRemoteDevName = NULL;
    delete mRemoteDevAddr;
    mRemoteDevAddr = NULL;
    delete[] mListOfDevices;
    mListOfDevices = NULL;
    delete[] mLocalDevName;
    mLocalDevName = NULL;
    delete mLocalDevAddr;
    mLocalDevAddr = NULL;

    delete mDiscoveredIviLinkChannel;
    mDiscoveredIviLinkChannel = NULL;
}

void BluetoothConnectionFinder::performSearch()
{
    if (!mIsValid) 
    {
        CThread::sleep(1000);
        LOG4CPLUS_ERROR(logger, "is not valid, sleeping");
        return;
    }
    LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

    char deviceName[256] = { 0 };
    int numOfDev;
    int maxNumOfDev = 256;

    mListOfDevices = new inquiry_info[maxNumOfDev];
    mDiscoveredIviLinkChannel = NULL;

    mDevDescr = hci_open_dev( mAdapterId );

    if (!mLocalDevAddr)
    {
        setLocalBdInfo();
    }
    if (!mRegSvc)
    {
        mLocalSdpSession = registerIviLinkService();        
    }

    if (mDevDescr >= 0 && mRegSvc)
    {    
        LOG4CPLUS_INFO(logger, "BT device descriptor = " + convertIntegerToString((int)mDevDescr));        
        LOG4CPLUS_INFO(logger, "Local BT device info:: " + std::string(mLocalDevName) 
                                + "\tBDADDR: " + convertBDaddrToStr(*mLocalDevAddr));

        numOfDev = hci_inquiry(mAdapterId, mTimeoutDiscovery, maxNumOfDev, NULL, &mListOfDevices, mFlags);

        if (numOfDev >= 0) 
        {    
            LOG4CPLUS_INFO(logger, "hci_inquiry: found " << numOfDev << " devices");

            for (int i = 0; i < numOfDev; i++)
            {
                memset(deviceName, 0, sizeof(deviceName));
                if (0 != hci_read_remote_name(mDevDescr, 
                                              &(mListOfDevices[i].bdaddr),
                                              sizeof(deviceName), 
                                              deviceName, 0)) 
                {
                    strcpy(deviceName, "[unknown]");
                }
                LOG4CPLUS_INFO(logger, convertBDaddrToStr(mListOfDevices[i].bdaddr) 
                                        + "\t" + std::string(deviceName));
            }
            bool isFound = false;
            for (int i = 0; i < numOfDev && (isFound != true); i++) 
            {
                memset(deviceName, 0, sizeof(deviceName));

                UInt8 *iviLinkCh = discoverIviLinkChannel(mListOfDevices[i].bdaddr);
                
                if (iviLinkCh)
                {
                    isFound = true;
                    if (0 != hci_read_remote_name(mDevDescr, 
                                                  &(mListOfDevices[i].bdaddr), 
                                                  sizeof(deviceName), 
                                                  deviceName, 0)) 
                    {
                        strcpy(deviceName, "[unknown]");
                        LOG4CPLUS_INFO(logger, "Can't get device name of "
                                                + convertBDaddrToStr(mListOfDevices[i].bdaddr));
                    }

                    mRemoteDevName = new char[sizeof(deviceName)];
                    mRemoteDevName = deviceName;

                    mRemoteDevAddr = new bdaddr_t;
                    memcpy(mRemoteDevAddr, &mListOfDevices[i].bdaddr, sizeof(bdaddr_t));
                    
                    mDiscoveredIviLinkChannel = iviLinkCh;                                      
                }
            }            

            if (mDiscoveredIviLinkChannel)
            {
                LOG4CPLUS_INFO(logger, "!!!!!!!FOUND addr =\t"
                                            + convertBDaddrToStr(*mRemoteDevAddr) + "\t" 
                                            + std::string(mRemoteDevName));
                CBluetoothInfo info;
                info.adapterId = mAdapterId;
                char strRemAddr[BDADDR_SIZE];                
                ba2str(mRemoteDevAddr, strRemAddr);
                info.remoteBDAddr = new char[BDADDR_SIZE];
                strcpy(info.remoteBDAddr, strRemAddr);

                if (1 == connectionEstablish(info, *mRemoteDevAddr)) 
                {                        
                    BluetoothAdapter * pAdapter = new BluetoothAdapter(info);
                    setReady(pAdapter);
                    sdp_close(mLocalSdpSession);
                    mRegSvc = false;
                }   
            }
            hci_close_dev(mDevDescr);
        }
        else
        {
            LOG4CPLUS_WARN(logger, "hci_inquiry FAILED");
            LOG4CPLUS_INFO(logger, "Restarting BT... ");
            restartHciDev(mAdapterId);
            setDiscoverable(mAdapterId);
            mRegSvc = false;
        }

        if (NULL != mListOfDevices)
        {
            free(mListOfDevices);
        }
    }
    else
    {
        LOG4CPLUS_ERROR(logger, "Open hci socket FAILED");
    }
//    

}

UInt8* BluetoothConnectionFinder::discoverIviLinkChannel(const bdaddr_t & DeviceAddress) 
{
    LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

    UInt8* iviLinkCh = NULL;

    sdp_session_t * sdpSession = sdp_connect(&mAnyAddress, &DeviceAddress, SDP_RETRY_IF_BUSY); // 

    if (0 != sdpSession)
    {
        LOG4CPLUS_INFO(logger, "SDP connect OK");

        sdp_list_t * searchList = sdp_list_append(0, &mIviLinkUUID);
        UInt32 range = 0x0000ffff;
        sdp_list_t * attrList = sdp_list_append(0, &range);
        sdp_list_t * responseList = NULL;

        if (0 == sdp_service_search_attr_req(sdpSession, 
                                             searchList, 
                                             SDP_ATTR_REQ_RANGE, 
                                             attrList, 
                                             &responseList))
        {
            LOG4CPLUS_INFO(logger, "SDP service searching...");
            if (0 == responseList)
            {
                LOG4CPLUS_INFO(logger, "No services found on device");
            }
            
            for (sdp_list_t * r = responseList; 0 != r; r = r->next)
            {
                sdp_record_t * sdpRecord = static_cast<sdp_record_t*>(r->data);
                sdp_list_t * protoList = 0;
                char serviceName[10];                
                int len = 10;
                sdp_get_service_name(sdpRecord, serviceName, len);
                LOG4CPLUS_INFO(logger,"Got service name = " + std::string(serviceName));

                if (0 == sdp_get_access_protos(sdpRecord, &protoList)) 
                {
                    LOG4CPLUS_INFO(logger, "SDP getting access protos...");
                    for (sdp_list_t * p = protoList; 0 != p; p = p->next)
                    {
                        sdp_list_t * pdsList = static_cast<sdp_list_t*>(p->data);

                        for (sdp_list_t * pds = pdsList; 0 != pds; pds = pds->next)
                        {
                            sdp_data_t * sdpData = static_cast<sdp_data_t*>(pds->data);
                            int proto = 0;

                            for (sdp_data_t * d = sdpData; 0 != d; d = d->next)
                            {
                                switch (d->dtd)
                                {
                                    case SDP_UUID16:
                                    case SDP_UUID32:
                                    case SDP_UUID128:
                                        proto = sdp_uuid_to_proto(&d->val.uuid);
                                        LOG4CPLUS_INFO(logger, "SDP uuid128 proto = " << proto);
                                        break;

                                    case SDP_UINT8:
                                        if (RFCOMM_UUID == proto)
                                        {
                                            iviLinkCh = &d->val.uint8;
                                            LOG4CPLUS_INFO(logger, "SDP uint8 iviLinkCh = " 
                                                            + convertIntegerToString((int)d->val.uint8));
                                        }
                                        break;
                                    default:
                                        LOG4CPLUS_WARN(logger,
                                                        "Switched to unknown proto");
                                }
                            }
                        }

                        sdp_list_free(pdsList, 0);
                    }

                    sdp_list_free(protoList, 0);
                }
            }
        }

        sdp_list_free(searchList, 0);
        sdp_list_free(attrList, 0);
        sdp_list_free(responseList, 0);
        sdp_close(sdpSession);
    }
    else
    {
        LOG4CPLUS_INFO(logger, "Service discovery failed for " + convertBDaddrToStr(DeviceAddress));
    }

    if (iviLinkCh)
    {
        LOG4CPLUS_INFO(logger, "iviLink service was discovered on device " + convertBDaddrToStr(DeviceAddress)
                                + " at channel: " << (int)(iviLinkCh));         
    }
    else
    {
        LOG4CPLUS_INFO(logger, "iviLink service was not discovered on device " + convertBDaddrToStr(DeviceAddress));
    }
    return iviLinkCh;
}

sdp_session_t* BluetoothConnectionFinder::registerIviLinkService() 
{
    LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

    UInt8 rfcommChannel = RFCOMM_CHANNEL;
    const char *serviceName = "iviLink";
    const char *serviceProv = "Luxoft";
    uuid_t rootUuid, l2capUuid, rfcommUuid, svcClassUuid;
    sdp_list_t *l2capList = 0, 
               *rfcommList = 0,
               *rootList = 0,
               *protoList = 0, 
               *accessProtoList = 0,
               *svcClassList = 0,
               *profileList = 0;
    sdp_data_t *channel = 0;
    sdp_profile_desc_t profile;
    sdp_record_t record = { 0 };
    sdp_session_t *session = 0;

    // set the general service ID
    UInt8 iviLinkUUIDdata[] = {0x93, 0x6D, 0xA0, 0x1F, 0x9A, 0xBD, 0x4D, 0x9D, 0x80, 0xC7, 0x02, 0xAF, 0x85, 0xC8, 0x22, 0xA8};
    // set the general service ID
    sdp_uuid128_create(&mIviLinkUUID, &iviLinkUUIDdata);
    sdp_set_service_id( &record, mIviLinkUUID );

    // set the service class
    sdp_uuid16_create(&svcClassUuid, SERIAL_PORT_SVCLASS_ID);
    svcClassList = sdp_list_append(0, &svcClassUuid);
    sdp_set_service_classes(&record, svcClassList);

    // set the Bluetooth profile information
    sdp_uuid16_create(&profile.uuid, SERIAL_PORT_PROFILE_ID);
    profile.version = 0x0100;
    profileList = sdp_list_append(0, &profile);
    sdp_set_profile_descs(&record, profileList);

    // make the service record publicly browsable
    sdp_uuid16_create(&rootUuid, PUBLIC_BROWSE_GROUP);
    rootList = sdp_list_append(0, &rootUuid);
    sdp_set_browse_groups( &record, rootList );

    // set l2cap information
    sdp_uuid16_create(&l2capUuid, L2CAP_UUID);
    l2capList = sdp_list_append( 0, &l2capUuid );
    protoList = sdp_list_append( 0, l2capList );

    // register the RFCOMM channel for RFCOMM sockets
    sdp_uuid16_create(&rfcommUuid, RFCOMM_UUID);
    channel = sdp_data_alloc(SDP_UINT8, &rfcommChannel);
    rfcommList = sdp_list_append( 0, &rfcommUuid );
    sdp_list_append( rfcommList, channel );
    sdp_list_append( protoList, rfcommList );

    accessProtoList = sdp_list_append( 0, protoList );
    sdp_set_access_protos( &record, accessProtoList );

    // set the name, provider, and description
    sdp_set_info_attr(&record, serviceName, serviceProv, NULL);

    // connect to the local SDP server, register the service record, 
    // and disconnect
    LOG4CPLUS_INFO(logger, "REGISTERING SERVICE:: for interface: " + std::string(mLocalDevName) 
                            + " bdaddr: " + convertBDaddrToStr(*mLocalDevAddr));
    
    session = sdp_connect(&mAnyAddress, &mLocalAddress, SDP_RETRY_IF_BUSY);
    if (0 < session)
    {
        LOG4CPLUS_INFO(logger, "Opening SDP session OK");
        LOG4CPLUS_INFO(logger, "sock = " + convertIntegerToString(session->sock));
        LOG4CPLUS_INFO(logger, "state = " + convertIntegerToString(session->state));
        LOG4CPLUS_INFO(logger, "local = " + convertIntegerToString(session->local));
        LOG4CPLUS_INFO(logger, "flags = " + convertIntegerToString(session->flags));
        int res = sdp_record_register(session, &record, 0);
        if (0 == res)
        {
            mRegSvc = true;
            LOG4CPLUS_INFO(logger, "iviLink service successful registered!!!");
        }
        else
        {
            mRegSvc = false;
            LOG4CPLUS_ERROR(logger, "iviLink service registering FAILED,\tres = " 
                                    + convertIntegerToString(res));
        }
    }
    else
    {
        mRegSvc = false;
        LOG4CPLUS_ERROR(logger,"Create SDP session failed!");
    }

    // cleanup
    sdp_data_free(channel);
    sdp_list_free(l2capList, 0);
    sdp_list_free(rfcommList, 0);
    sdp_list_free(rootList, 0);
    sdp_list_free(accessProtoList, 0);
    sdp_list_free(svcClassList, 0);
    sdp_list_free(profileList, 0);

    return session;
}

bool BluetoothConnectionFinder::connectionEstablish(CBluetoothInfo &info, const bdaddr_t &remoteAddr) 
{    
    LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
    mSocketMutex.lockWrite();

    bool isConnect = false;

    char strRemoteAddr[BDADDR_SIZE];
    char strLocalAddr[BDADDR_SIZE];

    ba2str(&remoteAddr, strRemoteAddr);
    LOG4CPLUS_INFO(logger, "Trying to establish connection with\t" 
                            + std::string(info.remoteBDAddr));

    if (NULL == mLocalDevAddr)
    {
        setLocalBdInfo();
    }
    ba2str(mLocalDevAddr, strLocalAddr);
    if (strcmp(strRemoteAddr, strLocalAddr) >= 0)
    {// if greater than ours - try to connect
        info.isServer = false;
    }
    else
    {
        info.isServer = true;
    }
    
    if (mDiscoveredIviLinkChannel)
    {        
        if (info.isServer)
        {
            int clientSock = -1;
            if (true == createSrvConnection(clientSock))
            {
                isConnect = true;
                info.sock = clientSock;
            }
            else
            {
                isConnect = false;
                LOG4CPLUS_ERROR(logger, "Failed create server connection");
            }

        }
        else
        {           
            int sock = -1;
            if (true == createClientConnection(sock))
            {
                isConnect = true;
                info.sock = sock;
            }
            else
            {
                isConnect = false;
                LOG4CPLUS_ERROR(logger, "Failed create client connection");
            }
        }
        
    }
    else
    {
        LOG4CPLUS_ERROR(logger, "RFCOMM channel isn't exist");
    }
    mSocketMutex.unlockWrite();

    return isConnect;
}

bool BluetoothConnectionFinder::createSrvConnection(int &clientSock)
{
    LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
    sockaddr_rc localSocketAddress = { 0 };
    sockaddr_rc remoteSocketAddress = { 0 };
    int rfcommSocket = -1;
    unsigned int opt = sizeof(remoteSocketAddress);
    fd_set rSet;
    int maxfd = -1;
    int sockFlags;

    LOG4CPLUS_INFO(logger, "I'm BT SERVER");   
    clientSock = -1;   

    rfcommSocket = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

    if (-1 != rfcommSocket)
    {
        if (!mDiscoveredIviLinkChannel)
        {
            LOG4CPLUS_WARN(logger, "Calling invalid pointer: mDiscoveredIviLinkChannel");
            return false;
        }              
        // bind socket to port mDiscoveredIviLinkChannel of the first available bluetooth adapter
        localSocketAddress.rc_family = AF_BLUETOOTH;
        localSocketAddress.rc_bdaddr = mAnyAddress;        
        localSocketAddress.rc_channel = *mDiscoveredIviLinkChannel;

        LOG4CPLUS_INFO(logger, "Bind socket");
        bind(rfcommSocket, (sockaddr *)&localSocketAddress, sizeof(localSocketAddress));

        // put socket into listening mode
        LOG4CPLUS_INFO(logger, "Listening...");
        listen(rfcommSocket, 1);

        // put server socket into nonblocking mode
        sockFlags = fcntl(rfcommSocket, F_GETFL, 0);
        fcntl(rfcommSocket, F_SETFL, sockFlags | O_NONBLOCK);

        // accept one connection 
        bool waitingConnect = true;   
        int timeout = 200;    
        while(waitingConnect) 
        {             
            FD_ZERO(&rSet);
            FD_SET(rfcommSocket, &rSet);
            maxfd = rfcommSocket;
            --timeout;

            int status = select(maxfd + 1, &rSet, NULL, NULL, NULL);
            if (status > 0 && FD_ISSET(rfcommSocket, &rSet)) 
            {
                // incoming connection            
                clientSock = accept(rfcommSocket, (sockaddr *)&remoteSocketAddress, &opt);
                if (clientSock >= 0) 
                {
                    break;
                }
                CThread::sleep(50);
            }
            if (0 == timeout)
            {
                LOG4CPLUS_INFO(logger, "Timeout waiting client connection");
                waitingConnect = false;
            }
        }
        close(rfcommSocket);     
                
        if (-1 != clientSock)
        {
            LOG4CPLUS_INFO(logger, "BT CONNECTION ESTABLISHED");
            LOG4CPLUS_INFO(logger, "clientSock = " + convertIntegerToString(clientSock));

            char addr[BDADDR_SIZE];
            ba2str(&remoteSocketAddress.rc_bdaddr, addr);
            LOG4CPLUS_INFO(logger, "Accepted connection from " + std::string(addr));

            return true;
        }
        else
        {
            int saved_errno = errno;
            LOG4CPLUS_ERROR(logger, "Accept connection FAILED:\t" 
                                    + convertIntegerToString(saved_errno) + strerror(saved_errno));
            return false;
        }
    }
    else
    {
        LOG4CPLUS_ERROR(logger, "allocate RFCOMM channel failed");
    }
    return false;
}

bool BluetoothConnectionFinder::createClientConnection(int &sock)
{
    LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

    sockaddr_rc remoteSocketAddress = { 0 };
    int status = -1;
    int rfcommSocket = -1;

    LOG4CPLUS_INFO(logger, "I'm client");

    rfcommSocket = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

    if (-1 != rfcommSocket)
    { 
        if (&mRemoteDevAddr)
        {
            remoteSocketAddress.rc_family = AF_BLUETOOTH;
            remoteSocketAddress.rc_bdaddr = *mRemoteDevAddr;
                       
            remoteSocketAddress.rc_channel = *mDiscoveredIviLinkChannel; // set channel to discovered on remote device
            LOG4CPLUS_INFO(logger, "Trying connect to " + std::string(mRemoteDevName) 
                                + "\t" + convertBDaddrToStr(*mRemoteDevAddr)
                                + "\tthrow " + convertIntegerToString((int)mDiscoveredIviLinkChannel));

            status = connect(rfcommSocket, (sockaddr *)&remoteSocketAddress, sizeof(remoteSocketAddress));
            if (0 == status)
            {
                LOG4CPLUS_INFO(logger, "BT CONNECTION ESTABLISHED");
                LOG4CPLUS_INFO(logger, "Connected to " + std::string(mRemoteDevName));
                sock = rfcommSocket;
                return true;
            }
            else
            {
                LOG4CPLUS_ERROR(logger, "Failed connect to remote device " + std::string(mRemoteDevName));
                LOG4CPLUS_ERROR(logger, "Connection status = " + convertIntegerToString(status) 
                                        + ":\t" +  strerror(errno));
                close(rfcommSocket);
                return false;
            }            
        }
        else
        {
            LOG4CPLUS_ERROR(logger, "Calling invalid pointer: mRemoteDevAddr");
        }
    }
    else
    {
        LOG4CPLUS_ERROR(logger, "allocate RFCOMM channel failed");
    }
    return false;
}

std::string BluetoothConnectionFinder::convertBDaddrToStr(const bdaddr_t &bdAddr)
{
    char addr[BDADDR_SIZE];
    ba2str(&bdAddr, addr);
    return std::string(addr);
}

bool BluetoothConnectionFinder::setLocalBdInfo() 
{
    LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
    mHciMutex.lock();
    hci_dev_info devInfo;
    if (0 <= mAdapterId)
    {
        devInfo.dev_id = mAdapterId;
        if (-1 == ioctl(mDevDescr, HCIGETDEVINFO, (void *) &devInfo))  // get local device info
        {
            LOG4CPLUS_ERROR(logger, "Can't read local device info");
            mHciMutex.unlock();
            return false;
        }
        else
        {
            mLocalDevAddr = new bdaddr_t;
            memcpy(mLocalDevAddr, &devInfo.bdaddr, sizeof(bdaddr_t));
            mLocalDevName = new char[sizeof(devInfo.name)];
            memcpy(mLocalDevName, devInfo.name, sizeof(devInfo.name));
            LOG4CPLUS_INFO(logger, " " + std::string(mLocalDevName) + "\t" 
                                    + convertBDaddrToStr(*mLocalDevAddr));
            mHciMutex.unlock();
            return true;
        }    
    }
    else
    {
        LOG4CPLUS_ERROR(logger, "Bad adapter ID = " + convertIntegerToString(mAdapterId));
    }
    mHciMutex.unlock();
    return false;
}

bool BluetoothConnectionFinder::setDiscoverable(const int &adapterId)
{
    LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

    mHciMutex.lock();
    int ctlDescr = hci_open_dev(mAdapterId);

    hci_dev_req devReq;
    devReq.dev_id  = mAdapterId;
    devReq.dev_opt = SCAN_PAGE;    
    
    if (ioctl(ctlDescr, HCISETSCAN, (unsigned long) &devReq) < 0) 
    {
        LOG4CPLUS_WARN(logger, "Can't set scan mode on hci" + convertIntegerToString(mAdapterId) + ":\t"
                                + strerror(errno)+ " (" + convertIntegerToString(errno) + ")");
        mHciMutex.unlock();
        return false;
    }

    devReq.dev_opt = SCAN_PAGE | SCAN_INQUIRY;
    if (ioctl(ctlDescr, HCISETSCAN, (unsigned long) &devReq) < 0) // set discoverable
    {
        LOG4CPLUS_WARN(logger, "Can't set piscan mode on hci" + convertIntegerToString(mAdapterId) + ":\t"
                                + strerror(errno)+ " (" + convertIntegerToString(errno) + ")");
        mHciMutex.unlock();
        return false;
    }    
    hci_close_dev(ctlDescr);
    mHciMutex.unlock();
    return true;
}
void BluetoothConnectionFinder::restartHciDev(const int &adapterId)
{
    LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

    mHciMutex.lock();
    int ctlDescr = hci_open_dev(mAdapterId);
    /* Stop HCI device */
    if (ioctl(ctlDescr, HCIDEVDOWN, mAdapterId) < 0) 
    {
        LOG4CPLUS_WARN(logger, "Can't down device hci" + convertIntegerToString(mAdapterId) + ":\t"
                                + strerror(errno) + " (" + convertIntegerToString(errno) + ")");
    }
    CThread::sleep(2000);

    /* Start HCI device */
    if (ioctl(ctlDescr, HCIDEVUP, mAdapterId) < 0) 
    {
        if (errno == EALREADY) LOG4CPLUS_INFO(logger, "Already started");
        LOG4CPLUS_WARN(logger, "Device already started hci" + convertIntegerToString(mAdapterId) + ":\t"
                                + strerror(errno) + " (" + convertIntegerToString(errno) + ")");
    }
    CThread::sleep(2000);
    hci_close_dev(ctlDescr);
    mHciMutex.unlock();
}

#endif // ANDROID
