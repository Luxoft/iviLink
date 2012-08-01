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
//Seat HU Profile API header
#include "../Profiles/ProfileAPI/ISeatHUProfile.hpp"
#include "../Profiles/ProfileProxy/CSeatHUProxy.hpp"
#include "utils/misc/Logger.hpp"

#define PROFILE_API_NAME "SeatHUProfile_API_UID"

using namespace iviLink::Profile;
using namespace iviLink::Service;

class CRequestProcessor : public QObject
{
    Q_OBJECT

    //Pointer to the seat HU profile API
    CSeatHUProxy * mProxy;

public:

    CRequestProcessor();

    /**
     * Get pointer to the Seat Client Profile API
     * @param pSeatClientService loaded Seat Client Service
     */
    void setProxy(CSeatHUProxy * proxy);

    /**
     * Send the state of seat to the stateless side
     * @param seatState current state of seat
     */
    void sendNotification(Json::Value seatState);

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

#endif // CREQUESTPROCESSOR_H
