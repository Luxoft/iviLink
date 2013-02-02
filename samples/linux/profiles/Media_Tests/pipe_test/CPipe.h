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
 * File:   CPipe.h
 * Author: oxana
 *
 * Created on August 16, 2012, 3:43 PM
 */

 
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include "sys/types.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "stdio.h"
#include "iostream"
#include "assert.h"

#ifndef CPIPE_H
#define	CPIPE_H

class CPipe {
public:
    CPipe();
    CPipe(const CPipe& orig);
    virtual ~CPipe();
private:

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


#endif	/* CPIPE_H */
