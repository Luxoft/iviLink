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
 * @file                QMLDeviceManagerDialog.hpp
 * @ingroup             Device Manager application
 * @author              Kramchaninov Pavel <pkramchaninov@luxoft.com>
 * @date                06.02.2013
 *
 * QT implementation of DeviceManager GUI
 */

#ifndef QMLDEVICE_MANAGER_DIALOG_HPP
#define QMLDEVICE_MANAGER_DIALOG_HPP

#ifndef ANDROID
#include <QObject>
#include <QString>
#include <QFileSystemWatcher>

#include "IDeviceManagerDialog.hpp"
#include "Logger.hpp"

namespace iviLink
{
namespace DeviceManager
{

class QMLDeviceManagerDialog : public QObject,
                               public IDeviceManagerDialog
{
    Q_OBJECT
public:
    QMLDeviceManagerDialog();

    void init();

    ~QMLDeviceManagerDialog();

    virtual void setDeviceManager(DeviceManager* deviceManager);
    virtual void updateListContent(CTrustList::tNameUidVector);
    virtual void updateFoundListContent(std::vector<iviLink::FoundDevice>);
    virtual void searchForDevices();
    Q_INVOKABLE bool isTrustListEmpty();
    Q_INVOKABLE bool isConnected();

public: signals:
    void trustedListPopulate(QString devName);
    void trustedListCleanup();
    void foundListCleanup();
    void foundListPopulate(QString devName);
    void showTrustedList();
    void showFoundList();
    void removeItemfromList(int index);
    void clearList();
    void enableRemoveAllButton();
    void disableRemoveAllButton();

public slots:
    void onTrustedListRemoveDevice(int index);
    void onFoundListConnectAndPairDevice(int index);
    void onFoundListDisconnectDevice(int index);
    void onDatabaseChanged(const QString &pathName);
    void onExit();
    void onSearchClicked();
    void onRemoveAll();

private:

    static Logger mLogger;

    DeviceManager* mDeviceManager;
    QFileSystemWatcher mWatcher;
};

}
}

#endif //ANDROID
#endif // QMLDEVICE_MANAGER_DIALOG_HPP
