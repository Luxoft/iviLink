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
 * File:   CControlTest.cpp
 * Author: oxana
 * 
 * Created on August 16, 2012, 12:22 PM
 */


static const char MediaServerName[]="MediaServer";


#include "CControlTest.h"

#include <cstring>
#include <cassert>
#include <iostream>

CMediaControlClientProfile::CMediaControlClientProfile()
: mpAppCallbacks(new Callbacks())
, mBe(true)
, mTag("CMediaControlClientProfile") {

}

CMediaControlClientProfile::~CMediaControlClientProfile() {

    mBe = false;
    delete mpAppCallbacks;
}

void CMediaControlClientProfile::play(std::string const& trackName, std::string const& avform) {
    prepareRequest(PLAY, true, trackName, avform);
}

void CMediaControlClientProfile::pause() {

    prepareRequest(PAUSE, false, mTag, mTag);
}

void CMediaControlClientProfile::stop() {

    prepareRequest(STOP, false, mTag, mTag);
}

void CMediaControlClientProfile::keepPlay() {

    prepareRequest(KEEPPLAY, false, mTag, mTag);
}

void CMediaControlClientProfile::sync() {

    prepareRequest(SYNC, false, mTag, mTag);
}

void CMediaControlClientProfile::unsync() {

    prepareRequest(UNSYNC, false, mTag, mTag);
}

void CMediaControlClientProfile::toggle() {

    prepareRequest(TOGGLE, false, mTag, mTag);
}

void CMediaControlClientProfile::prepareRequest(PROCEDURES_IDS proc, bool has_event, std::string const& event_f, std::string const& event_s) {
    std::cout << "prepareRequest  " << (UInt16) proc <<  std::endl;
  

    if (has_event) {
        std::cout << "has_event" << event_f << std::endl;
        std::cout << "has_event" << event_s << std::endl;
    }

}

void CMediaControlClientProfile::bufferReceived(UInt16 tmp, std::string const& buffer) {


    PROCEDURES_IDS proc = static_cast<PROCEDURES_IDS> (tmp);
    switch (proc) {
        case STOP:
        {

            mpAppCallbacks->onStop();
        }
            break;
        case PAUSE:
        {

            mpAppCallbacks->onPause();
        }
        case KEEPPLAY:
        {

            mpAppCallbacks->onKeepPlay();
        }
            break;
        case SYNC:
        {

            mpAppCallbacks->onSync();
        }
        case UNSYNC:
        {

            mpAppCallbacks->onUnsync();
        }
            break;
        case TOGGLE:
        {

            mpAppCallbacks->onToggle();
        }
            break;
        case PLAY:
        {
            std::string trackName = buffer;
            std::string source = buffer;
            std::string avform = buffer;
            mpAppCallbacks->onPlay(trackName, source, avform);
        }
            break;
        default:
        {
            std::cout << "ERROR" << std::endl;
            return;
        }
            break;
    }

}


#include <cstring>
#include <cassert>
#include <iostream>

CMediaControlServerProfile::CMediaControlServerProfile()
: mpAppCallbacks(new Callbacks())

, mBe(true)
, mTag("CMediaControlServerProfile")
, mPath("/home/")
, mIp("127.0.0.1")
, mPort("5544")
, mRtsp("rtsp://")
, mStreamName("stream_")
, mpServControl(new CServerControl()) {

    // get IP from ChannelSupervisorTube.hpp"
    std::string type;
    std::string remAdd;
    if (1) {
        mIp = "127.0.0.1"; // we've obtained our IP address
    }

}

CMediaControlServerProfile::~CMediaControlServerProfile() {

    mBe = false;

    delete mpAppCallbacks;
    delete mpServControl;


}

void CMediaControlServerProfile::play(std::string const trackName, std::string const avform) {

    std::string streamName = mStreamName + trackName;
    std::string path = mPath + trackName;
    std::string source = mRtsp + mIp + ":" + mPort + "/" + streamName;
    if (mpServControl->sendPlay(mPort, path, streamName, avform))
        prepareRequest(PLAY, true, trackName, source, avform);
}

void CMediaControlServerProfile::pause() {

    prepareRequest(PAUSE, false, mTag, mTag, mTag);
    mpServControl->sendCommand(mpServControl->mPause);
}

void CMediaControlServerProfile::stopStreaming() {

    prepareRequest(STOP, false, mTag, mTag, mTag);
    mpServControl->sendCommand(mpServControl->mStop);
}

void CMediaControlServerProfile::keepPlay() {

    if (mpServControl->sendCommand(mpServControl->mKeepPlay))
        prepareRequest(KEEPPLAY, false, mTag, mTag, mTag);
}

void CMediaControlServerProfile::sync() {

    prepareRequest(SYNC, false, mTag, mTag, mTag);
}

void CMediaControlServerProfile::unsync() {

    prepareRequest(UNSYNC, false, mTag, mTag, mTag);
}

void CMediaControlServerProfile::toggle() {

    prepareRequest(TOGGLE, false, mTag, mTag, mTag);
}

void CMediaControlServerProfile::setPath(std::string const& path) {
    mPath = path;
}

void CMediaControlServerProfile::prepareRequest(PROCEDURES_IDS proc, bool has_events, std::string const& event_f, std::string const& event_s, std::string const& event_t) {
    std::cout << "prepareRequest "<< (UInt16) proc << std::endl;   
    if (has_events) {
        std::cout <<"has_event" <<  event_f << std::endl;
        std::cout << "has_event" << event_s << std::endl;
        std::cout << "has_event" << event_t << std::endl;
    }
}


//from CChannelHandler

void CMediaControlServerProfile::bufferReceived(UInt16 tmp, std::string const& buffer) {

    PROCEDURES_IDS proc = static_cast<PROCEDURES_IDS> (tmp);
    std::cout << "buffer received  " << tmp << std::endl;
    switch (proc) {
        case STOP:
        {

            mpAppCallbacks->onStop();
            mpServControl->sendCommand(mpServControl->mStop);
        }
            break;
        case PAUSE:
        {
            mpAppCallbacks->onPause();
           
            mpServControl->sendCommand(mpServControl->mPause);
            break;
           
        }
        case KEEPPLAY:
        {

            mpAppCallbacks->onKeepPlay();
            keepPlay();
        }
            break;
        case SYNC:
        {

            mpAppCallbacks->onSync();
            break;
        }
        case UNSYNC:
        {

            mpAppCallbacks->onUnsync();
        }
            break;
        case TOGGLE:
        {

            mpAppCallbacks->onToggle();
        }
            break;
        case PLAY:
        {

            std::string trackName = buffer;
            std::string avform = buffer;
            play(trackName, avform);
        }
            break;
        default:
        {
            std::cout << "ERROR" << std::endl;
            return;
        }
            break;
    }


}


CServerControl::CServerControl()

: mVTranscode("#rtp{sdp=rtsp://:")
, mATranscode("#transcode{vcodec=none,acodec=mp3,ab=128,channels=2,samplerate=44100}:rtp{sdp=rtsp://:")
, mServerPath(MediaServerName)
, mFd(0)
, mStop("stop;")
, mKeepPlay("keepplay;")
, mPause("pause;")
, mAudio("audio")
, mVideo("video")
, mPlay("play;") {

    pipe(fds);
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
            if (1) {//execle(mServerPath.c_str(), fds[0], NULL) == -1) {
                ; //error
                break;
            }
        }
        default:
        {
            std::cout << "forked!!!!" << std::endl;
            close(fds[0]);
            ; // ok
            break;
        }


            //file = fdopen(fds[1], "w");
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
    //int result = fprintf(file, "%s\n", message);
    /* Write the message to the stream, and send it off immediately.   */
    //fflush(file);
    /* Snooze a while.  */
    //return result;
    return 1;
} 
