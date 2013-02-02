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
#ifndef CBUTTONPROCESSOR_H
#define CBUTTONPROCESSOR_H

#include <QObject>

#include <string>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "Logger.hpp"
#include "Exit.hpp"

class CButtonProcessor: public QObject
{
        Q_OBJECT
public:

    CButtonProcessor(bool isAuthEnabled);

public slots:

	//media button 
    void mediaPressed();

	//seat button 
    void seatPressed();

	// climate button 
    void climatePressed();

	// reset button 
	void resetPressed();

	// shutdown button
	void shutdownPressed();

	// unpair button 
	void unpairPressed();

private:

	// fork and execute the binary file "path"
    void launchApp(std::string path);

    bool mIsAuthEnabled;
	// logger
    static Logger    msLogger;
};

#endif // CBUTTONPROCESSOR_H
