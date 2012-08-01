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
#include "samples/linux/Profiles/ProfileProxy/CClimateClientProxy.hpp"

class CRequestProcessor : public QObject
{
    Q_OBJECT

    CClimateClientProxy mClimateClient;

public:

    CRequestProcessor();

public slots:

    /**
     * Set of functions which connected to qml
     * and send requests to the other side through
     * seat client profile API
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
    void initRequest();         // initialization state request
};

#endif // CREQUESTPROCESSOR_H
