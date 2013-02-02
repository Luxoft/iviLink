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
 * File:   CStreamingTest.h
 * Author: oxana
 *
 * Created on August 20, 2012, 10:11 AM
 */

#ifndef CSTREAMINGTEST_H
#define	CSTREAMINGTEST_H

class CStreamingTest {
public:
    CStreamingTest();
    CStreamingTest(const CStreamingTest& orig);
    virtual ~CStreamingTest();
private:

};


typedef unsigned short UInt16;
typedef unsigned long UInt32;


#include <iostream>
#include <queue>
#include <vector>

class Callbacks {
public:

    void onStreamingClientAccepted() {
        std::cout << "onStreamingClientAccepted" << std::endl;
    }

    void onStreamingClientUnaccepted() {
        std::cout << "onStreamingClientUnaccepted" << std::endl;
    }

    void onStreamingServerAccepted() {
        std::cout << "onStreamingServerAccepted" << std::endl;
    }

    void onStreamingServerUnaccepted() {
        std::cout << "onStreamingServerUnaccepted" << std::endl;
    }
    
    
    void onStop() {
        std::cout << "onStop" << std::endl;
    }
};

    enum PROCEDURES_IDS {
        SENDSTREAMINGINFO = 1,
        STREAMINGACCEPTED,
        STREAMINGUNACCEPTED,
    };

    
       struct info
   {
       //std::vector<std::string> audioForm;
       //std::vector<std::string> videoForm;
       std::string auForm;
       std::string viForm;
   };
   
class CMediaStreamingClientProfile  
{

public:    


   // from IMediaStreamingClientProfile_API
   virtual void prepareRequest(PROCEDURES_IDS proc);

   virtual void streamingAccepted();
   virtual void streamingUnaccepted();
   virtual void procInfo(std::string const& info);


   //from CChannelHandler
   virtual void bufferReceived(UInt16 tmp, std::string const& buffer);


   CMediaStreamingClientProfile();
   virtual ~CMediaStreamingClientProfile();
   Callbacks * mpAppCallbacks;

   bool                                         mBe;

   std::string                                  mTag;

   info                                         mInfo;
};


#include <iostream>
#include <queue>
#include <vector>

class CMediaStreamingServerProfile  
{

public:
    

   // from IMediaStreamingServerProfile_API
 
   virtual void sendStreamingInfo();


   //from CChannelHandler
   virtual void bufferReceived(UInt16 tmp, std::string const& buffer);


   CMediaStreamingServerProfile();
   virtual ~CMediaStreamingServerProfile();

   Callbacks * mpAppCallbacks;
   bool                                         mBe;
   std::string                                  mTag;
   info                                         mStreamingInfo;
};


#endif	/* CSTREAMINGTEST_H */

