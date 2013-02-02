/* 
 * iviLINK SDK, version 1.2
 * http://www.ivilink.net
 * Cross Platform Application Communication Stack for In-Vehicle Applications
 * 
 * Copyright (C) 2012-2013, Luxoft Professional Corp., member of IBS group
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
 */ 


#ifndef CREQUESTPROCESSOR_H
#define CREQUESTPROCESSOR_H

#ifndef ANDROID
#include <QObject>
#endif //ANDROID

#include "seat-app.hpp"


class CStateUpdater;

#ifndef ANDROID
class CRequestProcessor : public QObject
{
    Q_OBJECT
#else
class CRequestProcessor
{
#endif //ANDROID


public:

    explicit CRequestProcessor(seatAppPtr app);

private:
    seatAppPtr app;

#ifndef ANDROID
public slots:
#else
public:
#endif //ANDROID

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
    void exitRequest();

};

#endif // CREQUESTPROCESSOR_H
