/*
 *
 * iviLINK SDK, version 1.1.2
 * http://www.ivilink.net
 * Cross Platform Application Communication Stack for In-Vehicle Applications
 *
 * Copyright (C) 2012, Luxoft Professional Corp., member of IBS group
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


/**
 * @file                DeviceManager.cpp
 * @ingroup             Device Manager application
 * @author              Kramchaninov Pavel <pkramchaninov@luxoft.com>
 * @date                05.02.2013
 *
 * Device Manager
 */

#include <vector>
#include <string>

#include "DeviceManager.hpp"
#include "CIpc.hpp"
#include "ICallbackHandler.hpp"
#include "CDataAccessor.hpp"
#include "API.hpp"
#include "Serialize.hpp"

namespace iviLink
{
namespace DeviceManager
{

Logger DeviceManager::sLogger = Logger::getInstance(LOG4CPLUS_TEXT("applications.DeviceManager.Lib"));
Logger DeviceManagerIpc::sLogger = Logger::getInstance(LOG4CPLUS_TEXT("applications.DeviceManager.LibIpc"));

DeviceManager::DeviceManager(std::string trustListPath)
{
    LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);

    LOG4CPLUS_TRACE(sLogger, "TrustList path = " + trustListPath);

    mpTrustList = new CTrustList(trustListPath);

    mpHandler= new DeviceManagerIpc();

    mpConnectivityAgentIpc = new iviLink::Ipc::CIpc("/tmp/axisd_unix_sock", *mpHandler);

    mpConnectivityAgentIpc->connect();
}

void DeviceManager::setGUI(IDeviceManagerDialog* gui)
{
    mGUI = gui;
}

void DeviceManager::start()
{
    LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);

    mTrustedDeviceList.clear();

    mpTrustList->getKnownUids(mTrustedDeviceList);

    if(!mTrustedDeviceList.empty())
    {
        LOG4CPLUS_TRACE(sLogger, "UID = " + mTrustedDeviceList[0].first.value() + " Name = " + mTrustedDeviceList[0].second);
        mGUI->updateListContent(mTrustedDeviceList);
    }
    else
    {
        LOG4CPLUS_TRACE(sLogger, "DeviceList is empty");
        mGUI->updateListContent(mTrustedDeviceList);
    }
}

void DeviceManager::removeDeviceFromList(int index)
{
    LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
    BaseUid uid;
    time_t connectionTime;
    if(isConnected())
    {
        mpTrustList->getLastConnection(uid, connectionTime);
        if(uid == mTrustedDeviceList[index].first)
        {
            disconnectDevice(index);
        }
    }
    mpTrustList->removeUid(mTrustedDeviceList[index].first);
}

void DeviceManager::disconnectDevice(int index)
{
    LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);

    ConnectivityAgent::CDataAccessor data;

    data.setChannelID(0);
    data.setOpCode(E_DISCONNECT_DEVICE);
    const UInt8 * uidData = reinterpret_cast<const UInt8*>("Test");
    UInt32 uidDataLength = 5;
//    uidData = reinterpret_cast<const UInt8*>(mFoundDeviceList[index].mAddress.c_str());//FIXME: was temporary
//    uidDataLength = mFoundDeviceList[index].mAddress.length();    //FIXME: was temporary
    data.setData(uidData, uidDataLength);//Here goes Address
    UInt8* buf = new UInt8[data.getObjectSize()];
    data.copyToRawArray(buf);

    if(!mpHandler->isIpcConnected())
    {
        mpConnectivityAgentIpc->disconnect();
        delete mpConnectivityAgentIpc;
        mpConnectivityAgentIpc = new iviLink::Ipc::CIpc("/tmp/axisd_unix_sock", *mpHandler);
        mpConnectivityAgentIpc->connect();
    }
    LOG4CPLUS_TRACE(sLogger, "disconnectDevice DirId= " + convertIntegerToString(*(mpHandler->getDirId())));
    BaseError err = mpConnectivityAgentIpc->asyncRequest(0, buf, data.getObjectSize(), mpHandler->getDirId());
    LOG4CPLUS_TRACE(sLogger, "disconnectDevice DirId= " + convertIntegerToString(*(mpHandler->getDirId())));
    delete[] buf;

}

void DeviceManager::connectDevice(int index)
{
    LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);

    ConnectivityAgent::CDataAccessor data;

    data.setChannelID(0);
    data.setOpCode(E_CONNECT_DEVICE);
    const UInt8 * uidData = reinterpret_cast<const UInt8*>(mFoundDeviceList[index].mAddress.c_str());
    UInt32 uidDataLength = mFoundDeviceList[index].mAddress.length();
    data.setData(uidData, uidDataLength);//Here goes Address
    UInt8* buf = new UInt8[data.getObjectSize()];
    data.copyToRawArray(buf);

    if(!mpHandler->isIpcConnected())
    {
        mpConnectivityAgentIpc->disconnect();
        delete mpConnectivityAgentIpc;
        mpConnectivityAgentIpc = new iviLink::Ipc::CIpc("/tmp/axisd_unix_sock", *mpHandler);
        mpConnectivityAgentIpc->connect();
    }
    LOG4CPLUS_TRACE(sLogger, "connectDevice DirId= " + convertIntegerToString(*(mpHandler->getDirId())));
    BaseError err = mpConnectivityAgentIpc->asyncRequest(0, buf, data.getObjectSize(), mpHandler->getDirId());
    LOG4CPLUS_TRACE(sLogger, "connectDevice DirId= " + convertIntegerToString(*(mpHandler->getDirId())));
    delete[] buf;
}

void DeviceManager::updateFoundDevices()
{
    LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);

    bool exitFlag = false;

    ConnectivityAgent::CDataAccessor data;

    UInt32 respSize = 4084; // max size that can pass through ipc
    UInt8 resp[respSize];
    UInt32 counter = 0;

    data.setChannelID(0);
    data.setOpCode(E_GET_DEVICE_LIST);

    mFoundDeviceList.clear();

    while(!exitFlag)
    {
        data.setData(reinterpret_cast<UInt8*>(&counter),sizeof(counter));
        counter++;
        UInt8* buf = new UInt8[data.getObjectSize()];
        data.copyToRawArray(buf);

        if(!mpHandler->isIpcConnected())
        {
            mpConnectivityAgentIpc->disconnect();
            delete mpConnectivityAgentIpc;
            mpConnectivityAgentIpc = new iviLink::Ipc::CIpc("/tmp/axisd_unix_sock", *mpHandler);
            mpConnectivityAgentIpc->connect();
        };
        LOG4CPLUS_TRACE(sLogger, "updateFoundDevices DirId= " + convertIntegerToString(*(mpHandler->getDirId())));
        BaseError err = mpConnectivityAgentIpc->request(genID(), buf, data.getObjectSize(), resp, respSize, mpHandler->getDirId());
        LOG4CPLUS_TRACE(sLogger, "updateFoundDevices DirId= " + convertIntegerToString(*(mpHandler->getDirId())));

        delete[] buf;

        if (err.isNoError())
        {
            if (respSize)
            {
                ConnectivityAgent::CDataAccessor response(resp, respSize);
                size_t res_size = response.getDataSize();
                LOG4CPLUS_TRACE(sLogger,"Responce size not zero " + convertIntegerToString(res_size));
                if(response.getErrorCode() != 0)//FIXME: This is hardcode
                {
                    exitFlag = true;
                    break;
                }
                if (res_size > 0)
                {
                    //Got answer with device. We need to push it to mFoundDeviceList
                    FoundDevice device;
                    device.deserialize(response.getData());
                    mFoundDeviceList.push_back(device);
                }
                else
                {
                    //Zero response size. So, there is no devices left.
                    exitFlag = true;
                    break;
                }
           }
           else
           {
               //Zero sized answer from IPC, nothing to decode
               exitFlag = true;
               break;
           }
       }
       else
       {
           //Error while connecting to ConnectivityAgent
           exitFlag = true;
           break;
        }
    }
    mGUI->updateFoundListContent(mFoundDeviceList);
}

void DeviceManager::clearTrustList()
{
    LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
    mpTrustList->clearList();
    if(isConnected())
    {
        LOG4CPLUS_INFO(sLogger, "disconnect");
        disconnectDevice(0);//FIXME: This is temporary, untill we have a proper list of discovered devices
    }
}

bool DeviceManager::isConnected()
{
    LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);

    ConnectivityAgent::CDataAccessor data;

    UInt32 respSize = 4084; // max size that can pass through ipc
    UInt8 resp[respSize];
    UInt32 counter = 0;

    data.setChannelID(0);
    data.setOpCode(E_GET_DEVICE_LIST);

    data.setData(reinterpret_cast<UInt8*>(&counter),sizeof(counter));
    UInt8* buf = new UInt8[data.getObjectSize()];
    data.copyToRawArray(buf);

    if(!mpHandler->isIpcConnected())
    {
        mpConnectivityAgentIpc->disconnect();
        delete mpConnectivityAgentIpc;
        mpConnectivityAgentIpc = new iviLink::Ipc::CIpc("/tmp/axisd_unix_sock", *mpHandler);
        mpConnectivityAgentIpc->connect();
    };
    LOG4CPLUS_TRACE(sLogger, "isConnected DirId= " + convertIntegerToString(*(mpHandler->getDirId())));
    BaseError err = mpConnectivityAgentIpc->request(genID(), buf, data.getObjectSize(), resp, respSize, mpHandler->getDirId());
    LOG4CPLUS_TRACE(sLogger, "isConnected DirId= " + convertIntegerToString(*(mpHandler->getDirId())));
    delete[] buf;

    if (err.isNoError())
    {
        if (respSize)
        {
            ConnectivityAgent::CDataAccessor response(resp, respSize);
            size_t res_size = response.getDataSize();
            LOG4CPLUS_TRACE(sLogger,"Responce size not zero " + convertIntegerToString(res_size));
            if (response.getErrorCode() != 0)
            {
                return false;
            }
            if (res_size > 0)
            {
                //Got answer with device. We need to push it to mFoundDeviceList
                FoundDevice device;
                device.deserialize(response.getData());
                return device.mConnection != CON_UNKNOWN;
            }
        }
    }
    return false;
}

Ipc::MsgID DeviceManager::genID()
{
    return ++mId;
}

bool DeviceManager::isTrustListEmpty()
{
    return mTrustedDeviceList.empty();
}

DeviceManager::~DeviceManager()
{
    delete mpTrustList;
    mpConnectivityAgentIpc->disconnect();
    delete mpConnectivityAgentIpc;
    delete mpHandler;
}


DeviceManagerIpc::DeviceManagerIpc()
{
    LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
    dirId = 0;
    connected = false;
}

void DeviceManagerIpc::OnConnection(iviLink::Ipc::DirectionID id)
{
    LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
    LOG4CPLUS_TRACE(sLogger, "IPC Connected " + convertIntegerToString(id));
    LOG4CPLUS_TRACE(sLogger, "IPC Connected " + convertIntegerToString(dirId));
    dirId = id;
    connected = true;
}

void DeviceManagerIpc::OnConnectionLost(iviLink::Ipc::DirectionID id)
{
    LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
    LOG4CPLUS_TRACE(sLogger, "IPC ConnectionLost " + convertIntegerToString(id));
    LOG4CPLUS_TRACE(sLogger, "IPC ConnectionLost " + convertIntegerToString(dirId));
    connected = false;
}

void DeviceManagerIpc::OnRequest(iviLink::Ipc::MsgID, const UInt8*, UInt32, UInt8*, UInt32&, iviLink::Ipc::DirectionID)
{
    LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
}

void DeviceManagerIpc::OnAsyncRequest(iviLink::Ipc::MsgID, const UInt8*, UInt32, iviLink::Ipc::DirectionID)
{
    LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
}

bool DeviceManagerIpc::isIpcConnected()
{
    LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
    LOG4CPLUS_TRACE(sLogger, "IPC isIpcConnected = " + convertIntegerToString(connected?1:0));
    return connected;
}

iviLink::Ipc::DirectionID* DeviceManagerIpc::getDirId()
{
    LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
    LOG4CPLUS_TRACE(sLogger, "IPC getDirId " + convertIntegerToString(dirId));
    return &dirId;
}

}
}
