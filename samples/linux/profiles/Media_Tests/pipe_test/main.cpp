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


/* 
 * File:   main.cpp
 * Author: oxana
 *
 * Created on August 16, 2012, 3:34 PM
 */

#include <cstdlib>
#include "CPipe.h"

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {
    std::string comm = "stop";
    CServerControl*  control = new CServerControl();
    control->sendCommand(comm);
    comm = "pause";
    control->sendCommand(comm);
    comm = "keepplay";
    control->sendCommand(comm);
    std::string port = "5544";
    std::string path = "/home/media/test.mp4";
    std::string streamName = "stream_test.mp4";
    std::string avform = "video";
    control->sendPlay( port, path,  streamName,  avform);
    avform = "audio";
    control->sendPlay( port, path,  streamName,  avform);
    sleep(2);

    return 0;
}

