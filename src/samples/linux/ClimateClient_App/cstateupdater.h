/* 
 * 
 * iviLINK SDK, version 1.0
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



#ifndef CSTATEUPDATER_H
#define CSTATEUPDATER_H

#include <QObject>

#include "crequestprocessor.h"
#include "cclimateclientprofilecallbacks.hpp"

#include "framework/appLib/appLib/CApp.hpp"

#define SERVICE_NAME "ClimateClientService"
#define PROFILE_API_NAME "ClimateClientProfile_API_UID"

class CStateUpdater : public QObject
                    , public IProfileObserver
                    , public iviLink::CApp
{
    Q_OBJECT

    //Pointer to the request processor
    CRequestProcessor * mpCRequestProcessor;
    CClimateClientProfileCallbacks  mClimateClientCallbacks;

public:

    CStateUpdater(CRequestProcessor * pCRequestProcessor);
    ~CStateUpdater();

    virtual void initDone(iviLink::ELaunchInfo launcher);

    virtual void incomingServiceAfterLoading(const iviLink::Service::Uid &service);


    //From IProfileObserver
    /**
     * Callback for profile created on the other side; notification 
     * from stateful side about conditiner state
     * @param condState current state of conditioner
     */
    virtual void incomingNotification(Json::Value condState);

signals:
 
    /**
     * set of functions which connected with qml 
     * and change UI state after changing conditioner state
     */
    void showClimate();
    void autoState(bool status);
    void recircState(bool status);
    void acState(bool status);
    void frontState(bool status);
    void rearState(bool status);
    void fanLowState(bool status);
    void fanMidState(bool status);
    void fanHighState(bool status);
    void passTemp(int temp);
    void driverTemp(int temp);
    void syncState(bool status);
    void fanSpeed(int speed);
    void fanOffState(bool status);
};

#endif // CSTATEUPDATER_H
