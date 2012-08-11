/* 
 * 
 * iviLINK SDK, version 1.0.1
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





#ifndef CREQUESTPROCESSOR_H
#define CREQUESTPROCESSOR_H

#include <QObject>
#include <QString>
#include <QDebug>
#include <QDir>

#include <iostream>
#include <stdio.h>
#include <cassert>

#include "utils/json/include/json/json.h"
//Seat Client Profile API header
#include "samples/linux/Profiles/ProfileAPI/ISeatClientProfile.hpp"
#include "samples/linux/Profiles/ProfileProxy/CSeatClientProxy.hpp"
#include "utils/misc/Logger.hpp"

#define PROFILE_API_NAME "SeatClientProfile_API_UID"

class CRequestProcessor : public QObject
{
    Q_OBJECT

    //Pointer to the seat client profile API
    CSeatClientProxy * mProxy;
    Logger mLogger;

public:

    CRequestProcessor();

    /**
     * Get pointer to the Seat Client Profile API
     * @param pSeatClientService loaded Seat Client Service
     */
    void setProxy(CSeatClientProxy * proxy);

public slots:

   /**
     * Set of functions connected to qml
     * that send requests to the other side through
     * seat client profile API
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
    void initRequest();

};

#endif // CREQUESTPROCESSOR_H
