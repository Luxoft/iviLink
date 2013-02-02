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
 * File:   CStreamingTest.cpp
 * Author: oxana
 * 
 * Created on August 20, 2012, 10:11 AM
 */

#include "CStreamingTest.h"

CStreamingTest::CStreamingTest() {
}

CStreamingTest::CStreamingTest(const CStreamingTest& orig) {
}

CStreamingTest::~CStreamingTest() {
}

CMediaStreamingClientProfile::CMediaStreamingClientProfile()
: mpAppCallbacks(new Callbacks())
, mBe(true)
, mTag("CMediaStreamingClientProfile") {
    mInfo.auForm = "mp3";
    mInfo.viForm = "h264";
}

CMediaStreamingClientProfile::~CMediaStreamingClientProfile() {
    mBe = false;
}

void CMediaStreamingClientProfile::streamingAccepted() {
    prepareRequest(STREAMINGACCEPTED);
}

void CMediaStreamingClientProfile::streamingUnaccepted() {
    prepareRequest(STREAMINGUNACCEPTED);
}

void CMediaStreamingClientProfile::prepareRequest(PROCEDURES_IDS proc) {

    std::cout << "send this proc:" << proc << std::endl;
}

void CMediaStreamingClientProfile::procInfo(std::string const& info) {

    size_t pos = info.find(";");
    if (mInfo.auForm == info.substr(0, pos) && mInfo.viForm == info.substr(pos + 1)) {
        streamingAccepted();
        mpAppCallbacks->onStreamingClientAccepted();
    } else {
        streamingUnaccepted();
        mpAppCallbacks->onStreamingClientUnaccepted();
    }

}

//from CChannelHandler

void CMediaStreamingClientProfile::bufferReceived(UInt16 tmp, std::string const& buffer) {

    PROCEDURES_IDS proc = static_cast<PROCEDURES_IDS> (tmp);

    if (proc == SENDSTREAMINGINFO) {
        std::string info = buffer;
        procInfo(info);
    } else {
        std::cout << "ERROR" << std::endl;
    }

}

CMediaStreamingServerProfile::CMediaStreamingServerProfile()
: mpAppCallbacks(new Callbacks())
, mBe(true)
, mTag("CMediaStreamingServerProfile") {
    mStreamingInfo.auForm = "mp3";
    mStreamingInfo.viForm = "h264";
}

CMediaStreamingServerProfile::~CMediaStreamingServerProfile() {

    mBe = false;

}

void CMediaStreamingServerProfile::sendStreamingInfo() {

    std::string msg = mStreamingInfo.auForm + ";" + mStreamingInfo.viForm;

    std::cout << "send this msg: " << msg << std::endl;
}


//from CChannelHandler

void CMediaStreamingServerProfile::bufferReceived(UInt16 tmp, std::string const& buffer) {


    PROCEDURES_IDS proc = static_cast<PROCEDURES_IDS> (tmp);

    if (proc == STREAMINGACCEPTED) {

        mpAppCallbacks->onStreamingServerAccepted();
    }
    if (proc == STREAMINGUNACCEPTED) {
        mpAppCallbacks->onStreamingServerUnaccepted();
    } else {
        std::cout << "ERROR" << std::endl;
    }


}


