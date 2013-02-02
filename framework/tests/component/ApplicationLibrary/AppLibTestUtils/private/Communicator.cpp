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


#include <iostream>
#include <string>
#include <sstream>
#include <list>
#include <sys/socket.h>
#include <netinet/in.h>
#include <queue>
#include <pthread.h>
#include <cstring>
#include "stdlib.h"
#include "strings.h"

#include "CMutex.hpp"

#include "Logger.hpp"
#include "Communicator.hpp"

#define BUF_SIZE  1000   //big enough buffer size for incoming messages 
Logger Communicator::mLogger = Logger::getInstance(LOG4CPLUS_TEXT("AppLibTest.Utils.Communicator"));

/**
 * Communicator object which provides all the commectivity functionality and message exchane functionality
 * @param ipAddress - remote ip-address
 */
Communicator::Communicator(const char* ipAddress)
        : mIpAddress(ipAddress)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT("log4cplus.properties"));

    //setting up local socket for listerning and waiting for commection from the remote communicator
    setupReceiver();

    //setting up sender socket - connect to remote Communicator socket
    setupSender();

    //startingt listener and message processing threads
    pthread_create(&mThreadListernerId, NULL, &Communicator::Listener,
            reinterpret_cast<void *>(this));
    pthread_create(&mThreadQReaderId, NULL, &Communicator::QReader, reinterpret_cast<void *>(this));
}

Communicator::~Communicator()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    //nothing to free at the moment
    ;
}

/**
 * Sends message to the pair Communicator object
 * @param waiter - waiter to be removed
 * @return none
 */
void Communicator::sendMessage(InterAppMessage message)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);

    int sentLength;
    char buffer[BUF_SIZE];
    bzero(buffer, BUF_SIZE);

    char header[3];     //header contains id (1 byte) and data size (2 bytes)
    header[0] = 0x01;   //filling up the header

    //put message into string
    std::stringstream ss;
    ss << (int) message;

    short size = ss.str().length();     //data size

    LOG4CPLUS_INFO(mLogger, "Size: " + convertIntegerToString(size));

    //put size into header
    header[1] = ((size & 0xFF00) >> 8);
    header[2] = (size & 0x00FF);

    //put header into buffer to be sent
    buffer[0] = header[0];
    buffer[1] = header[1];
    buffer[2] = header[2];

    //put message into buffer to be sent
    strncpy((buffer + 3), ss.str().c_str(), ss.str().length());

    LOG4CPLUS_INFO(mLogger, "Buffer: " + string(buffer));

    //write to the socket
    sentLength = write(mRemoteCommunicatorSocket, buffer, ss.str().length() + 3);

    LOG4CPLUS_INFO(mLogger, "Sent: " + convertIntegerToString(sentLength));

    if (sentLength < 0)
    {
        LOG4CPLUS_ERROR(mLogger, "ERROR writing to socket");
    }
    return;
}

/**
 * Endless socket listerning - performed in separate thread
 * @param none
 * @return none
 */

void Communicator::listen()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);

    int readLength = -1;

    char buffer[BUF_SIZE];  //receiver buffer
    char header[3];         //header

    bzero(header, 3);       //zeroing header

    //some socket-related magic
    mClientAddressSize = sizeof(mClinetAddress);

    while (1)
    {
        //disconnect if flag is true
        bool disconnectionFlag = false;

        //accepting socket
        LOG4CPLUS_INFO(mLogger, "Accept socket");
        mNewSocket = accept(mSocket, reinterpret_cast<sockaddr *>(&mClinetAddress),
                &mClientAddressSize);

        if (mNewSocket < 0)
        {
            //again
            LOG4CPLUS_ERROR(mLogger, "Error on accept");
            continue;
        }
        else
        {
            LOG4CPLUS_INFO(mLogger, "Sock accept");
        }

        //read all the incoming data
        while (1)
        {
            usleep(100);

            bzero(buffer, BUF_SIZE);
            bzero(header, 3);

            //read the header
            readLength = read(mNewSocket, header, 3);

            if (readLength <= 0)
            {
                LOG4CPLUS_WARN(mLogger,
                        "sock possibly disconnected. Read Length = "
                                + convertIntegerToString(readLength));
                close (mNewSocket);
                break;
            }

            unsigned short dataSize = (header[1] << 8 & 0xFF00) | (header[2] & 0x00FF);
            LOG4CPLUS_INFO(mLogger, "Data size = " + convertIntegerToString(dataSize));

            //read the data left
            readLength = read(mNewSocket, buffer, dataSize);
            if (readLength <= 0)  //if pipe has closed
            {
                LOG4CPLUS_WARN(mLogger,
                        "sock possibly disconnected. Read Length = "
                                + convertIntegerToString(readLength));
                close (mNewSocket);
                break;
            }
            else if (readLength < dataSize)  //if read less than expected
            {
                LOG4CPLUS_INFO(mLogger, "Read: " + convertIntegerToString(readLength));
                disconnectionFlag = false;

                while (readLength < dataSize)  //reading until data ends
                {
                    //reading data from socket
                    int i = read(mNewSocket, buffer + readLength, dataSize - readLength);
                    readLength += i;

                    LOG4CPLUS_INFO(mLogger, "Read: " + convertIntegerToString(readLength));

                    if (i <= 0)  //smth wrong during reading
                    {
                        disconnectionFlag = true;
                        LOG4CPLUS_WARN(mLogger,
                                "Exited because of disc flag. read Length ="
                                        + convertIntegerToString(readLength));
                        break;
                    }
                }

                //checking disconnection flag
                if (disconnectionFlag)
                {
                    LOG4CPLUS_WARN(mLogger,
                            "sock possibly disconnected, read Length ="
                                    + convertIntegerToString(readLength));
                    close (mNewSocket);
                    break;
                }
            }
            LOG4CPLUS_INFO(mLogger, "Here is the message: " + string(buffer));

            //extract message

            int msgValue;
            istringstream(buffer) >> msgValue;

            LOG4CPLUS_INFO(mLogger, "Here is the message: " + convertIntegerToString(msgValue));

            InterAppMessage receivedMessage = static_cast<InterAppMessage>(msgValue);

            //send message into the q
            mMessageQ.enqueue(receivedMessage);
        }
    }

    // close socket
    close (mNewSocket);
    close (mSocket);

    return;
}

/**
 * Endless incoming queue processing - performed in separate thread
 * @param none
 * @return none
 */
void Communicator::processQ()
{

    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);

    InterAppMessage msg;
    while (1)
    {
        if (mMessageQ.queueSize() > 0)
        {
            LOG4CPLUS_INFO(mLogger, "Q size: " + convertIntegerToString(mMessageQ.queueSize()));
            msg = mMessageQ.dequeue();
            LOG4CPLUS_INFO(mLogger, "Q message: " + convertIntegerToString((int) msg));
            processIncomingMessage(msg);
            msg = (InterAppMessage) 0;
        }
        usleep(100);
    }
    return;  //never return
}

/**
 * Feeds incoming message to all the Waiters from the List
 * @param incoming message
 * @return none
 */
void Communicator::processIncomingMessage(InterAppMessage msg)
{

    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);

    //go through the list of waiters and offer msg until first of them accepted it
    std::list<Waiter*>::iterator iterator;

    LOG4CPLUS_INFO(mLogger, "process message: " + convertIntegerToString((int) msg));

    mWaiterListMutex.lock();
    for (iterator = mWaiterList.begin(); iterator != mWaiterList.end(); ++iterator)
    {
        (*iterator)->tryMessage(msg);
    }

    mWaiterListMutex.unlock();

    return;
}

/**
 * Adds Waiter to the list of the waiters for message feeding
 * @param waiter - waiter to be added
 * @return none
 */
void Communicator::addWaiter(Waiter* waiter)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);

    mWaiterListMutex.lock();
    mWaiterList.push_back(waiter);
    mWaiterListMutex.unlock();
    return;
}

/**
 * Removes Waiter from the list of the waiters for message feeding
 * @param waiter - waiter to be removed
 * @return none
 */
void Communicator::removeWaiter(Waiter* waiter)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);

    mWaiterListMutex.lock();
    mWaiterList.remove(waiter);
    mWaiterListMutex.unlock();
    return;
}

/**
 * Setting up socket client which connects to teh remote Communicator socket
 * @param none
 * @return none
 */
void Communicator::setupSender()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);

    //set initial port number
    mRemoteCommunicatorPort = PORT_NUMBER;

    //create socket
    mRemoteCommunicatorSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (mRemoteCommunicatorSocket < 0)
    {
        LOG4CPLUS_ERROR(mLogger, "ERROR opening socket ");
        exit(0);
    }

    //getting remote host name
    mRemoteServer = gethostbyname(mIpAddress);
    /*while (mRemoteServer == NULL)
     {
     std::cout << "ERROR no host" << std::endl;
     sleep(1);
     }*/

    bzero((char *) &mServerAddress, sizeof(mServerAddress));
    mServerAddress.sin_family = AF_INET;
    bcopy((char *) mRemoteServer->h_addr, (char *) &mServerAddress.sin_addr.s_addr,
            mRemoteServer->h_length);
    mServerAddress.sin_port = htons(mRemoteCommunicatorPort);

    //connecting to the remote socket, trying different port numbers
    while (connect(mRemoteCommunicatorSocket, reinterpret_cast<sockaddr *>(&mServerAddress),
            sizeof(mServerAddress)) < 0)
    {
        LOG4CPLUS_ERROR(mLogger, "ERROR on connecting ");

        sleep(1);
        if (mRemoteCommunicatorPort < MAX_PORT_NUMBER)
        {
            mRemoteCommunicatorPort++;
        }
        else
        {
            mRemoteCommunicatorPort = PORT_NUMBER;
        }
    }

    //connected
    LOG4CPLUS_INFO(mLogger, "Connected ");
    return;
}

void Communicator::setupReceiver()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);

    //setup server socket

    //initialize socket
    mPortNumber = PORT_NUMBER;

    //Create socket
    mSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (mSocket < 0)
    {
        LOG4CPLUS_ERROR(mLogger, "ERROR opening socket ");
        return;
    }

    bzero((char *) &mLocalServerAddress, sizeof(mLocalServerAddress));

    mLocalServerAddress.sin_family = AF_INET;
    mLocalServerAddress.sin_addr.s_addr = INADDR_ANY;
    mLocalServerAddress.sin_port = htons(mPortNumber);

    //bind to the socket
    while (bind(mSocket, reinterpret_cast<sockaddr *>(&mLocalServerAddress),
            sizeof(mLocalServerAddress)) < 0)
    {
        LOG4CPLUS_ERROR(mLogger, "ERROR on binding");
        sleep(1);

        if (mPortNumber < MAX_PORT_NUMBER)
        {
            mPortNumber++;
        }
        else
        {
            mPortNumber = PORT_NUMBER;
        }
    }

    LOG4CPLUS_INFO(mLogger, "Listen Sock ");
    //listen socket
    ::listen(mSocket, 5);
    return;
}
