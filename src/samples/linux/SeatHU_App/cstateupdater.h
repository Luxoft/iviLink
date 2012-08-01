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
#include <cassert>

#include "cseathuprofilecallbacks.hpp"
#include "cseatstate.h"
#include "samples/linux/SeatHU_App/IProfileObserver.hpp"
#include "crequestprocessor.h"

#include "utils/json/include/json/json.h"
#include "utils/misc/Logger.hpp"
#include "framework/appLib/appLib/CApp.hpp"
#include "samples/linux/Profiles/ProfileAPI/ISeatClientProfile.hpp"

#define SERVICE_NAME "SeatHUService"
#define PROFILE_API_NAME "SeatHUProfile_API_UID"

class CStateUpdater : public QObject
        ,public IProfileObserver
        ,public iviLink::CApp
{
    Q_OBJECT

    //Pointer to the holder of seat state
    CSeatState * mpCSeatState; 
    CSeatHUProfileCallbacks mSeatHUCallbacks;
    Logger mLogger;

public:

    CStateUpdater(CSeatState * pCSeatState);
    ~CStateUpdater();

    // from CApp
    virtual void initDone(iviLink::ELaunchInfo launcher);

    //From IProfileObserver
    /**
     * Callback for profile created on the other side; 
     * change the seat heater request
     * @param event  the holder of requested seat heater
     */
    virtual void onHeaterRequest(EPersonEvent event);

    /**
     * Callback for profile created on the other side;
     * change the current visible seat request
     * @param event the holder of requested seat
     */
    virtual void onTabRequest(EPersonEvent event);

    /**
     * Callback for profile created on the other side; 
     * change the settings of current visible seat request
     * @param event the direction of movement
     */
    virtual void onMoveRequest(EMoveEvent event);

    /**
     * Callback for profile created on the other side;
     * initialization state request
     */
    virtual void onInitRequest();

public slots:

    /**
     * Set of functions which are connected to qml
     * and send requests to the holder of seat state
     */

    void driverHeaterRequest();
    void passHeaterRequest();
    void driverSeatRequest();
    void passSeatRequest();
    void backLeftRequest();
    void backRightRequest();
    void bottomUpRequest();
    void bottomDownRequest();
    void bottomLeftRequest();
    void bottomRightRequest();
    void initLocalRequest();


};

#endif // CSTATEUPDATER_H
