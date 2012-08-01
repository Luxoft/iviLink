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

#include "cclimatehuprofilecallbacks.hpp"
#include "cclimatecond.h"

#include "framework/appLib/appLib/CApp.hpp"

#define SERVICE_NAME "ClimateHUService"
#define PROFILE_API_NAME "ClimateHUProfile_API_UID"

class CStateUpdater : public QObject
        ,public IProfileObserver
        ,public iviLink::CApp
{
    Q_OBJECT

    //Pointer to the holder of seat state
    CClimateCond * mpCClimateCond; 

    CClimateHUProfileCallbacks  mClimateHUCallbacks;

public:

    CStateUpdater(CClimateCond * mpCClimateCond);
    ~CStateUpdater();

    virtual void initDone(iviLink::ELaunchInfo launcher);
    
    /**
     * Callback for profile created on the other side; 
     * change temperature settings request
     * @param event specify the temperature event
     */
    virtual void onTempRequest(ETempEvent event);

    /**
     * Callback for profile created on the other side; 
     * change fan settings request
     * @param event specify the fan event
     */
    virtual void onFanRequest(EFanEvent event);

    /**
     * Callback for profile created on the other side; 
     * change heater settings request
     * @param event specify the heater event
     */
    virtual void onHeaterRequest(EHeaterEvent event);

    /**
     * Callback for profile created on the other side; 
     * change general settings request
     * @param event kind of settings
     */
    virtual void onGeneralRequest(EGeneralRequest event);

    /**
     * Callback for profile created on the other side;
     * initialization state request
     */
    virtual void onInitRequest();

public slots:

    /**
     * Set of functions which connected to qml
     * and send requests to the holder of seat state
     */
  
    void tempDecDrRequest();    // driver temperature decrease request
    void tempIncDrRequest();    // driver temperature increase request
    void tempDecPassRequest();  // passenger temperature decrease request
    void tempIncPassRequest();  // passenger temperature increase request
    void syncRequest();         // syncronizing temperature of driver and passenger
    void fanLowRequest();       // fan low request
    void fanMidRequest();       // fan middle request
    void fanHighRequest();      // fan high request
    void fanIncRequest();       // fan increase requst
    void fanDecRequest();       // fan decrease request
    void offRequest();          // off fan request
    void rearRequest();         // change rear mode request
    void frontRequest();        // change front mode request
    void acRequest();           // change ac mode request
    void recircRequest();       // change recirculation mode request
    void autoRequest();         // change auto mode request
    void initLocalRequest();    // initialization state request

};

#endif // CSTATEUPDATER_H
