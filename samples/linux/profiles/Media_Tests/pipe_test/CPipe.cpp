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
 * File:   CPipe.cpp
 * Author: oxana
 * 
 * Created on August 16, 2012, 3:43 PM
 */

#include "CPipe.h"

CPipe::CPipe() {
}

CPipe::CPipe(const CPipe& orig) {
}

CPipe::~CPipe() {
}


CServerControl::CServerControl()

: mVTranscode("#rtp{sdp=rtsp://:")
, mATranscode("#transcode{vcodec=none,acodec=mp3,ab=128,channels=2,samplerate=44100}:rtp{sdp=rtsp://:")
, mServerPath("/home/oxana/gitrepo/AXIS/src/samples/linux/Profiles/Media_Tests/StreamingServer/test")
, mFd(0)
, mStop("stop;")
, mKeepPlay("keepplay;")
, mPause("pause;")
, mAudio("audio")
, mVideo("video")
, mPlay("play;") {
    
    pipe (fds); 
    pid_t pid = fork();

    switch (pid) {
        case -1:
        {
            ; //error
            return;
            break;
        }
        case 0:
        {
            //child
            // starting application

            // Do not use logging or any other code with mutexes here!
            
            close(fds[1]);
            char fd_str[15];
            sprintf(fd_str, "%i", fds[0]);
            std::cout << fd_str <<std::endl;
            if (execl(mServerPath.c_str(), mServerPath.c_str(), fd_str,  NULL) == -1) {
                std::cout << "error" << std::endl;
                 exit(1);
            }
             exit(1);
        }
        default:
        {
            close(fds[0]);
            file = fdopen(fds[1], "w");
            ; // ok
            break;
        }        
    }
}

CServerControl::~CServerControl() {

    close(fds[1]);
}

bool CServerControl::sendCommand(std::string const& command) {
    std::string msg = command;
    bool res = false;
    //mpMutex->lock();
    res = writer(msg.c_str());
    //mpMutex->unlock();
    if (res)
        return 1;
    return 0;
}

bool CServerControl::sendPlay(std::string const& port, std::string const& path, std::string const& streamName, std::string const& avform) {
    std::string sout;
    if (avform == mAudio)
        sout = mPlay + mATranscode + port + "/" + streamName + "}";
    else if (avform == mVideo)
        sout = mPlay + mVTranscode + port + "/" + streamName + "}";
    std::string msg = sout + ";" + path;
    std::cout << "send play  " << msg <<std::endl;
    bool res = false;
    //mpMutex->lock();
    res = writer(msg.c_str());
    //mpMutex->unlock();
    if (res)
        return 1;
    return 0;
}

int CServerControl::writer(const char* message) {
    std::string msg(message);
    std::cout << "sending command to the server  " << msg <<std::endl;
    int result = fprintf(file, "%s\n", message);
    /* Write the message to the stream, and send it off immediately.   */
    fflush(file);
    /* Snooze a while.  */
    return result;
    //return 1;
} 
