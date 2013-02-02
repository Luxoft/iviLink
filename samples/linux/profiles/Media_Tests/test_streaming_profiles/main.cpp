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
 * Created on August 20, 2012, 10:09 AM
 */

#include <cstdlib>
#include "CStreamingTest.h"

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {
    
    CMediaStreamingClientProfile*   mProfile = new CMediaStreamingClientProfile();
  
    std::string buffer = "mp3;h264";
    std::string buffer_new = "mp4;h264";
    std::string buffer1 = "mp3h264";
    mProfile->bufferReceived((UInt16)SENDSTREAMINGINFO, buffer);
    mProfile->bufferReceived((UInt16)SENDSTREAMINGINFO, buffer_new);
    mProfile->bufferReceived((UInt16)SENDSTREAMINGINFO, buffer1);
    
    
    CMediaStreamingServerProfile*   mSProfile = new CMediaStreamingServerProfile();
    
    mSProfile->sendStreamingInfo();

    return 0;
}

