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
 * File:   CServer.cpp
 * Author: oxana
 * 
 * Created on August 14, 2012, 5:27 PM
 */

#include "CServer.h"

CServer::CServer() {
    //vlc = libvlc_new(0, NULL);
    media_name = "media";
}

void CServer::onPlay(std::string const& sout, std::string const& url) {
    const char * mSout = sout.c_str();
    const char * mUrl = url.c_str();
    //libvlc_vlm_add_broadcast(vlc, media_name, mUrl, mSout, 0, NULL, true, false);
    //libvlc_vlm_play_media(vlc, media_name);
}

void CServer::onStop() {

    //libvlc_vlm_stop_media(vlc, media_name);
    //libvlc_vlm_release(vlc);
     std::cout << "onStop" << std::endl;
}

void CServer::onPause() {

    //libvlc_vlm_pause_media(vlc, media_name);
     std::cout << "onPause" << std::endl;
}

void CServer::onKeepPlay() {

    //libvlc_vlm_play_media(vlc, media_name);
     std::cout << "onKeepPlay" << std::endl;
}

CServer::~CServer() {

    //libvlc_vlm_release(vlc);
     std::cout << "CServer" << std::endl;
}
