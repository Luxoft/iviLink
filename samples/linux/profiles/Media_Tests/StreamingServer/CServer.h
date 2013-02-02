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
 * File:   CServer.h
 * Author: oxana
 *
 * Created on August 14, 2012, 5:27 PM
 */

#include <vlc/libvlc.h>
#include <vlc/vlc.h>

#include <string>
#include "stdio.h"
#include "iostream"

#ifndef CSERVER_H
#define	CSERVER_H

class CServer {
public:
    CServer();
    virtual ~CServer();

void  onKeepPlay();
void  onPause();
void  onStop();
void  onPlay(std::string const& sout, std::string const& url);

private:
    libvlc_instance_t *vlc;
    const char*       media_name; 
};

#endif	/* CSERVER_H */

