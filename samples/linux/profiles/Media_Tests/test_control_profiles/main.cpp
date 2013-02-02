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
 * Created on August 16, 2012, 12:19 PM
 */

#include <cstdlib>
#include "CControlTest.h"

int main(int argc, char** argv) {
    
    CMediaControlClientProfile*   mProfile = new CMediaControlClientProfile();
  
    std::string str = "video";
    std::string trackName = "ldfkjh.mp3";
    std::string avform = "video";
    
    mProfile->stop();
    mProfile->play(trackName, avform); //send the msg to another side
    mProfile->keepPlay();
    mProfile->pause();
    mProfile->toggle();
    mProfile->sync();
    mProfile->unsync();
    mProfile->bufferReceived((UInt16)STOP, str);
    mProfile->bufferReceived((UInt16) PLAY, str);
    mProfile->bufferReceived((UInt16) KEEPPLAY, str);
    mProfile->bufferReceived((UInt16) PAUSE, str);
    mProfile->bufferReceived((UInt16) TOGGLE, str);
    mProfile->bufferReceived((UInt16) SYNC, str);
    mProfile->bufferReceived((UInt16) UNSYNC, str);
    mProfile->bufferReceived((UInt16)19,str);
    
    delete mProfile;
    
    CMediaControlServerProfile*   mnProfile = new CMediaControlServerProfile();
    
    mnProfile->stopStreaming();
    mnProfile->play(trackName, avform); //send the msg to another side
    mnProfile->keepPlay();
    mnProfile->pause();
    mnProfile->toggle();
    mnProfile->sync();
    mnProfile->unsync();
    mnProfile->bufferReceived((UInt16) STOP, str);
    mnProfile->bufferReceived((UInt16) PLAY, str);
    mnProfile->bufferReceived((UInt16) KEEPPLAY, str);
    mnProfile->bufferReceived((UInt16) PAUSE, str);
    mnProfile->bufferReceived((UInt16) TOGGLE, str);
    mnProfile->bufferReceived((UInt16) SYNC, str);
    mnProfile->bufferReceived((UInt16) UNSYNC, str);
    mnProfile->bufferReceived((UInt16)19,str);
    //sleep(10);
    return 0;
}

