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


#ifndef COMMUNICATOR_HPP
#define	COMMUNICATOR_HPP

#include <sys/socket.h>
#include <netdb.h>
#include <cstring>
#include <list>
#include <pthread.h>
#include "stdlib.h"
#include "strings.h"

#include "CMutex.hpp"

#include "Logger.hpp"
#include "Waiter.hpp"
#include "InterAppMessage.hpp"
#include "private/Queue.hpp"

#define PORT_NUMBER     8000    //minimum port number
#define MAX_PORT_NUMBER 8010    //maximum port number

class Communicator
{
public:

    /**
     * Adds Waiter to the list of the waiters for message feeding
     * @param waiter - waiter to be added
     * @return none
     */
    void addWaiter(Waiter* waiter);

    /**
     * Removes Waiter from the list of the waiters for message feeding
     * @param waiter - waiter to be removed
     * @return none
     */
    void removeWaiter(Waiter* waiter);

    /**
     * Sends message to the pair Communicator object
     * @param waiter - waiter to be removed
     * @return none
     */
    void sendMessage(InterAppMessage message);

    /**
     * Lister - thread which does listerning of the incoming data from the pair Communicator object
     * @param pointer to the Communicatr class object
     * @return none
     */
    static void* Listener(void* thisPtr)
    {
        ((Communicator*) thisPtr)->listen();
    }

    /**
     * Queue reader - thread which does processing of the incoming messages received from the listener
     * @param pointer to the Communicatr class object
     * @return none
     */
    static void* QReader(void* thisPtr)
    {
        ((Communicator*) thisPtr)->processQ();
    }

    /**
     * Communicator object which provides all the commectivity functionality and message exchane functionality
     * @param ipAddress - remote ip-address
     */
    explicit Communicator(const char* ipAddress);

    virtual ~Communicator();

private:
    void processIncomingMessage(InterAppMessage msg);

private:
    const char * mIpAddress;

    std::list<Waiter*> mWaiterList;        //List of Waiters
    CMutex mWaiterListMutex;   //mutex to protect list of waiters
    MessageQueue mMessageQ;          //queue of the received messages

    pthread_t mThreadListernerId;
    pthread_t mThreadQReaderId;

    //socket section
    int mSocket;
    int mNewSocket;
    int mPortNumber;
    socklen_t mClientAddressSize;

    sockaddr_in mLocalServerAddress;
    sockaddr_in mClinetAddress;

    //sending socket data
    int mRemoteCommunicatorSocket;  //descriptor of the socket    
    int mRemoteCommunicatorPort;    //port of teh Communicator object which receives the message
    int nSend;
    sockaddr_in mServerAddress;
    hostent *mRemoteServer;             //remote server

    //logger
    static Logger mLogger;

    //internal methods
    void listen();
    void processQ();
    void setupSender();
    void setupReceiver();
};

#endif	/* COMMUNICATOR_HPP */

