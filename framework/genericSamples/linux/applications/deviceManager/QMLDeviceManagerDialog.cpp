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
 * @file                QMLDeviceManagerDialog.cpp
 * @ingroup             Device Manager application
 * @author              Kramchaninov Pavel <pkramchaninov@luxoft.com>
 * @date                06.02.2013
 *
 * QT implementation of DeviceManager GUI
 */

#ifndef ANDROID

#include <QAbstractListModel>
#include <QMessageBox>
#include <QFileSystemWatcher>
#include "DeviceManager.hpp"
#include "QMLDeviceManagerDialog.hpp"

using namespace iviLink::DeviceManager;

Logger QMLDeviceManagerDialog::mLogger =  Logger::getInstance(LOG4CPLUS_TEXT("applications.DeviceManager"));

QMLDeviceManagerDialog::QMLDeviceManagerDialog()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);

    mWatcher.addPath( PREFIX "/database/trust.db" );
    connect(&mWatcher, SIGNAL(fileChanged(const QString &)), this, SLOT(onDatabaseChanged(const QString &)));
}

void QMLDeviceManagerDialog::init()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);

    emit trustedListCleanup();
    emit showTrustedList();
    if (!isTrustListEmpty())
    {
        emit enableRemoveAllButton();
    }
}

void QMLDeviceManagerDialog::setDeviceManager(DeviceManager* deviceManager)
{
    mDeviceManager = deviceManager;
}

QMLDeviceManagerDialog::~QMLDeviceManagerDialog()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
}

void QMLDeviceManagerDialog::updateListContent(CTrustList::tNameUidVector deviceList)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);

    emit trustedListCleanup();

    for(int i = 0; i < deviceList.size(); i++ )
    {
        QString devName;
        if(deviceList[i].second != "")
        {
            devName = QString::fromStdString(deviceList[i].second);
            std::cout << " devName " << deviceList[i].second << std::endl;
        }
        else
        {
            devName = QString::fromStdString(deviceList[i].first.value());
            std::cout << " devName " << deviceList[i].first.value() << std::endl;
        }
        emit trustedListPopulate(devName); //strings are passed to qml to be displayed
    }
    if (!mDeviceManager->isTrustListEmpty())
    {
        emit enableRemoveAllButton();
    }
    else
    {
        emit disableRemoveAllButton();
    }
    emit showTrustedList();
}

void QMLDeviceManagerDialog::updateFoundListContent(std::vector<iviLink::FoundDevice> deviceList)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);

    emit foundListCleanup();

    for(int i = 0; i < deviceList.size(); i++ )
    {
        QString devName;
        if(deviceList[i].mName != "")
        {
            devName = QString::fromStdString(deviceList[i].mName);
            std::cout << " devName " << deviceList[i].mName << std::endl;
        }
        else
        {
            devName = QString::fromStdString(deviceList[i].mUid.value());
            std::cout << " devName " << deviceList[i].mUid.value() << std::endl;
        }
        emit foundListPopulate(devName); //strings are passed to qml to be displayed
    }
    if (!mDeviceManager->isTrustListEmpty())
    {
        emit enableRemoveAllButton();
    }
    else
    {
        emit disableRemoveAllButton();
    }
    emit showFoundList();
}
void QMLDeviceManagerDialog::onTrustedListRemoveDevice(int index)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
    LOG4CPLUS_TRACE(mLogger, "index = " + convertIntegerToString(index));

    mDeviceManager->removeDeviceFromList(index);
    if (mDeviceManager->isTrustListEmpty())
    {
        emit disableRemoveAllButton();
    }
}

void QMLDeviceManagerDialog::onDatabaseChanged(const QString &pathname)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
    
    mDeviceManager->start();
}

void QMLDeviceManagerDialog::onFoundListConnectAndPairDevice(int index)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
    LOG4CPLUS_TRACE(mLogger, "index = " + convertIntegerToString(index));

    mDeviceManager->connectDevice(index);
}

void QMLDeviceManagerDialog::onFoundListDisconnectDevice(int index)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
    LOG4CPLUS_TRACE(mLogger, "index = " + convertIntegerToString(index));
    
    mDeviceManager->disconnectDevice(index);
    if (!mDeviceManager->isTrustListEmpty())
    {
        emit enableRemoveAllButton();
    }
    else
    {
        emit disableRemoveAllButton();
    }
}

bool QMLDeviceManagerDialog::isConnected()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
    return mDeviceManager->isConnected();
}

bool QMLDeviceManagerDialog::isTrustListEmpty()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
    return mDeviceManager->isTrustListEmpty();
}

void QMLDeviceManagerDialog::searchForDevices()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );

    mDeviceManager->updateFoundDevices();
}

void QMLDeviceManagerDialog::onRemoveAll()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
    if (mDeviceManager)
    {
        mDeviceManager->clearTrustList();
    }
    emit clearList();
}

void QMLDeviceManagerDialog::onSearchClicked()
{
    searchForDevices();
}

void QMLDeviceManagerDialog::onExit()
{
    exit(0);
}

#endif // ANDROID
