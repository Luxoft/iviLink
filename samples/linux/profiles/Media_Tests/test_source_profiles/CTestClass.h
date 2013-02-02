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
 * File:   CTestClass.h
 * Author: oxana
 *
 * Created on August 15, 2012, 4:30 PM
 */

#ifndef CTESTCLASS_H
#define	CTESTCLASS_H

class CTestClass {
public:
    CTestClass();
    CTestClass(const CTestClass& orig);
    virtual ~CTestClass();
private:

};

#endif	/* CTESTCLASS_H */


#include <cstring>
#include <cassert>
#include <iostream>


typedef unsigned short UInt16;
typedef unsigned long UInt32;

class Callbacks;

enum PROCEDURES_IDS {
    ONRECALLTRACKLIST = 1,
    SENDTRACKLIST,
    ONTRACKLISTCHANGED,
    REQUESTTRACKLIST,
};

class CMediaSourceClientProfile {
public:

    // from IMediaSourceClientProfile_API
    void requestTrackList();

    //from CChannelHandler
    void bufferReceived(UInt16 tmp, std::string const& buffer);


    //from ISender
    void senderLoop();

    bool hasRequests();
    void handleRequest();

    CMediaSourceClientProfile();
    virtual ~CMediaSourceClientProfile();


    Callbacks * mpAppCallbacks;

    bool mBe;
    bool mHasRequest;
    std::string mTag;

};

class Callbacks {
public:

    void onRecallTrackList() {
        std::cout << "onRecallTrackList" << std::endl;
    }

    void onTrackListChanged() {
        std::cout << "onTrackListChanged" << std::endl;
    }

    void trackListRecieved(std::string const& str) {
        std::cout << "trackListRecieved   " << str << std::endl;
    }
    
     void onRequestTrackList() {
        std::cout << "onRequestTrackList   "<< std::endl;
    }
   
};

class CMediaSourceServerProfile {
public:
    virtual void prepareRequest(PROCEDURES_IDS proc, bool has_event, std::string const& event);

    virtual void onRecallTrackList();
    virtual void sendTrackList(std::string const& trackList);
    virtual void onTrackListChanged();

    virtual void bufferReceived(UInt16 tmp, std::string const& buffer);


    //from ISender


    CMediaSourceServerProfile();
    virtual ~CMediaSourceServerProfile();

    Callbacks * mpAppCallbacks;
    bool mBe;
    std::string mTag;

};
