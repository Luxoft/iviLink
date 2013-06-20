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
 * @file                DeviceManager.hpp
 * @ingroup             Device Manager application
 * @author              Kramchaninov Pavel <pkramchaninov@luxoft.com>
 * @date                05.02.2013
 *
 * Device Manager
 */


#ifndef DEVICE_MANAGER_HPP_
#define DEVICE_MANAGER_HPP_

#include "Logger.hpp"
#include "CTrustList.hpp"
#include "IDeviceManagerDialog.hpp"
#include "CIpc.hpp"
#include "ICallbackHandler.hpp"
#include "FoundDevice.hpp"
#include <string>

namespace iviLink
{
namespace DeviceManager
{

class  DeviceManagerIpc
    : public iviLink::Ipc::ICallbackHandler
{
public:
    DeviceManagerIpc();
    void OnConnection(iviLink::Ipc::DirectionID);
    void OnConnectionLost(iviLink::Ipc::DirectionID);
    void OnRequest(iviLink::Ipc::MsgID, const UInt8*, UInt32, UInt8*, UInt32&, iviLink::Ipc::DirectionID);
    void OnAsyncRequest(iviLink::Ipc::MsgID, const UInt8*, UInt32, iviLink::Ipc::DirectionID);

    iviLink::Ipc::DirectionID* getDirId();
    bool isIpcConnected();

private:
    static Logger sLogger;

    iviLink::Ipc::DirectionID   dirId;
    bool connected;
};

class DeviceManager
{
public:
    DeviceManager(std::string trustListPath);

    virtual ~DeviceManager();

    void start();
    void setGUI(IDeviceManagerDialog* gui);
    void removeDeviceFromList(int index);
    void clearTrustList();
    void disconnectDevice(int index);
    void connectDevice(int index);
    void updateFoundDevices();
    bool isTrustListEmpty();
    /**
     * Requests connection status 
     * @return true if device is connected
     */
    bool isConnected();

private:
    static Logger sLogger;

    Ipc::MsgID genID();

    IDeviceManagerDialog* mGUI;
    CTrustList * mpTrustList;
    CTrustList::tNameUidVector mTrustedDeviceList;
    std::vector<iviLink::FoundDevice> mFoundDeviceList;
    iviLink::Ipc::CIpc* mpConnectivityAgentIpc;
    DeviceManagerIpc * mpHandler;
    Ipc::MsgID mId;
};

}
}
#endif /* DEVICE_MANAGER_HPP_ */
