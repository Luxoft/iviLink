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
 * File:   CControlTest.h
 * Author: oxana
 *
 * Created on August 16, 2012, 12:22 PM
 */

typedef unsigned short UInt16;
typedef unsigned long UInt32;

/*
enum PROCEDURES_IDS {
    STOP = 1,
    PLAY,
    KEEPPLAY,
    PAUSE,
    TOGGLE,
    SYNC,
    UNSYNC,
};
 */

#include <iostream>
#include <queue>
#include <cstring>
#include <cassert>
#include <iostream>

class Callbacks {
public:

    void onKeepPlay() {
        std::cout << "onKeepPlay" << std::endl;
    }

    void onStop() {
        std::cout << "onStop" << std::endl;
    }

    void onPause() {
        std::cout << "onPause   " << std::endl;
    }

    void onSync() {
        std::cout << "onSync   " << std::endl;
    }

    void onUnsync() {
        std::cout << "onUnsync   " << std::endl;
    }

    void onToggle() {
        std::cout << "onToggle   " << std::endl;
    }

    void onPlay(std::string const& str1,std::string const& str2,std::string const& str3) {
        std::cout << "onPlay   " << std::endl;
        std::cout << str1<< std::endl;
        std::cout << str2<< std::endl;
        std::cout << str3 << std::endl;
    }

};

 enum PROCEDURES_IDS {
    STOP = 1,
    PLAY,
    KEEPPLAY,
    PAUSE,
    TOGGLE,
    SYNC,
    UNSYNC,
    };

class CMediaControlClientProfile {
public:
    
    
    // from IMediaControlClientProfile_API
    virtual void prepareRequest(PROCEDURES_IDS proc, bool has_events, std::string const& event_f, std::string const& event_s);

    virtual void stop();
    virtual void play(std::string const& trackName, std::string const& avform); //send the msg to another side
    virtual void keepPlay();
    virtual void pause();
    virtual void toggle();
    virtual void sync();
    virtual void unsync();
    virtual void bufferReceived(UInt16 tmp, std::string const& buffer);

    CMediaControlClientProfile();
    virtual ~CMediaControlClientProfile();

    Callbacks * mpAppCallbacks;
    bool mBe;
    std::string mTag;
    
    
};


class CServerControl  {

public:


   CServerControl();
   virtual ~CServerControl();

   bool sendCommand(std::string const& command); 
   bool sendPlay(std::string const& port, std::string const& path, std::string const& streamName, std::string const& avform); 

private:
   
   int writer(const char* message);

   std::string                                  mVTranscode;
   std::string                                  mATranscode;
   std::string                                  mServerPath;
   int                                          mFd;
   int 						fds[2];
   FILE*                                        file; 
public:
   std::string mStop ;
   std::string mKeepPlay ;
   std::string mPause;  
   std::string mAudio;
   std::string mVideo;
   std::string mPlay;
};


#include <stdlib.h>
#include <unistd.h>
#include <string>
#include "sys/types.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "stdio.h"

#include <iostream>
#include <queue>
#include <string>


class CMediaControlServerProfile 
{


public:
   // from IMediaControlServerProfile_API
   virtual void prepareRequest(PROCEDURES_IDS proc, bool has_events, std::string const& event_f, std::string const& event_s, std::string const& event_t);

   virtual void stopStreaming();
   virtual void play(std::string const  trackName, std::string const avform); //send the msg to another side
   virtual void keepPlay();
   virtual void pause(); 
   virtual void toggle(); 
   virtual void sync(); 
   virtual void unsync(); 
   virtual void setPath(std::string const& str);

   //from CChannelHandler
   virtual void bufferReceived(UInt16 tmp, std::string const& buffer);


   CMediaControlServerProfile();
   virtual ~CMediaControlServerProfile();

 
   Callbacks *  mpAppCallbacks;

   bool                                         mBe;
  
   std::string                                  mTag;
 
   std::string                                  mPath;
   std::string                                  mIp;
   std::string                                  mPort;
   std::string                                  mRtsp;
   std::string                                  mStreamName;
   CServerControl *                             mpServControl;
};


