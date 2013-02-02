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
 * File:   CTestClass.cpp
 * Author: oxana
 * 
 * Created on August 15, 2012, 4:30 PM
 */

#include "CTestClass.h"

CTestClass::CTestClass() {
}

CTestClass::CTestClass(const CTestClass& orig) {
}

CTestClass::~CTestClass() {
}

CMediaSourceClientProfile::CMediaSourceClientProfile()
: mBe(true)
, mTag("CMediaSourceClientProfile")
, mHasRequest(false) {
    mpAppCallbacks = new Callbacks();
    std::cout << mHasRequest << std::endl;
}

CMediaSourceClientProfile::~CMediaSourceClientProfile() {
    mBe = false;
    delete mpAppCallbacks;

}

void CMediaSourceClientProfile::requestTrackList() {
    mHasRequest = true;
}

//from CChannelHandler

void CMediaSourceClientProfile::bufferReceived(UInt16 tmp, std::string const& buffer) {

    bool res = true;


    PROCEDURES_IDS proc = static_cast<PROCEDURES_IDS> (tmp);

    switch (proc) {
        case ONRECALLTRACKLIST:
        {

            mpAppCallbacks->onRecallTrackList();
        }
            break;
        case ONTRACKLISTCHANGED:
        {

            mpAppCallbacks->onTrackListChanged();
        }
            break;
        case SENDTRACKLIST:
        {

            std::string trackList = buffer;

            if (res)
                mpAppCallbacks->trackListRecieved(trackList);
        }
            break;
        default:
        {

            return;
        }
            break;
    }

    if (!res) {

        return;
    }
}

void CMediaSourceClientProfile::senderLoop() {

    while (mBe) {
        if (hasRequests()) {
            handleRequest();

        }

    }
}

bool CMediaSourceClientProfile::hasRequests() {
    return mHasRequest;
}

void CMediaSourceClientProfile::handleRequest() {
    mHasRequest = false;
}

CMediaSourceServerProfile::CMediaSourceServerProfile()
: mpAppCallbacks(new Callbacks())
, mBe(true)
, mTag("CMediaSourceServerProfile") {

}

CMediaSourceServerProfile::~CMediaSourceServerProfile() {
   
    mBe = false;
  
}

void CMediaSourceServerProfile::sendTrackList(std::string const& trackList) {
     std::cout << "CMediaSourceServerProfile::sendTrackList" << std::endl;
     
    prepareRequest(SENDTRACKLIST, true, trackList);
}

void CMediaSourceServerProfile::onTrackListChanged() {
     std::cout << "CMediaSourceServerProfile::onTrackListChanged" << std::endl;
       std::string str = "lrdgj";
    prepareRequest(ONTRACKLISTCHANGED, false, str);
}

void CMediaSourceServerProfile::onRecallTrackList() {
     std::cout << "CMediaSourceServerProfile::onRecallTrackList" << std::endl;
     std::string str = "lrdgj";
    prepareRequest(ONRECALLTRACKLIST, false, str);
}

void CMediaSourceServerProfile::prepareRequest(PROCEDURES_IDS proc, bool has_event, std::string const& trackList) {
   std::cout << "prepareRequest" << std::endl;
    std::cout << (UInt16)proc << std::endl;

   if (has_event)
   {
        std::cout << trackList<< std::endl;
   }
 
}


//from CChannelHandler

void CMediaSourceServerProfile::bufferReceived(UInt16 tmp, std::string const& buffer) {


    PROCEDURES_IDS proc = static_cast<PROCEDURES_IDS> (tmp);

    if (proc == REQUESTTRACKLIST) {
        mpAppCallbacks->onRequestTrackList();
    } else {
        std::cout << "ERROR"<< std::endl;
    }
}


