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
#include <QString>
#include <QDebug>
#include <QDir>
#include <QDeclarativeView>
#include <cassert>

#include "crequestprocessor.h"
#include "cseatclientprofilecallbacks.hpp"
#include "samples/linux/SeatClient_App/IProfileObserver.hpp"

#include "utils/json/include/json/json.h"
#include "framework/appLib/appLib/CApp.hpp"
#include "utils/misc/Logger.hpp"
#include "samples/linux/Profiles/ProfileAPI/ISeatClientProfile.hpp"

#define SERVICE_NAME "SeatClientService"
#define PROFILE_API_NAME "SeatClientProfile_API_UID"

class CStateUpdater : public QObject
        ,public IProfileObserver
        ,public iviLink::CApp
{
    Q_OBJECT
	

public:

    CStateUpdater(CRequestProcessor * pCRequestProcessor);
    ~CStateUpdater();
    
    // from CApp
    virtual void initDone(iviLink::ELaunchInfo launcher);


    //From IProfileObserver
    /**
     * Callback for profile created on the other side; notification 
     * from stateful side about seat state
     * @param seatState current state of seats
     */
    virtual void incomingNotification(Json::Value seatState);
    
private:
   //Pointer to the request processor 
   CRequestProcessor * mpRequestProcessor;
   CSeatClientProfileCallbacks  mSeatClientCallbacks;
   
   Logger mLogger;

   bool isInitDone;
   bool isQmlReady;

public slots:
   void onQmlVisible();

signals:
	
    /**
     * set of functions which connected with qml 
     * and change UI state after changing seat state
     */

    void showSeat();
    void heaterDriver(int heat);
    void heaterPass(int heat);
    void current_seat_viewPass();
    void current_seat_viewDriver();
    void bottom_x(int pos);
    void bottom_y(int pos);
    void back_angle(int angle);
    void back_x(int pos);
    void back_y(int pos);
};

#endif // CSTATEUPDATER_H
