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


#ifndef ANDROID

#include <cassert>
#include <cerrno>

#include "BluetoothAdapter.hpp"

#include <fcntl.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/sdp.h>
#include <bluetooth/sdp_lib.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/rfcomm.h>

#define BDADDR_SIZE 32

using namespace iviLink::ConnectivityAgent::HAL;
using namespace iviLink::ConnectivityAgent;
Logger BluetoothAdapter::logger = Logger::getInstance(LOG4CPLUS_TEXT("ConnectivityAgent.HAL.BluetoothCarrierAdapter"));

BluetoothAdapter::BluetoothAdapter(const CBluetoothInfo & connectionInfo) :
   CThread("BluetoothCarrierAdapter", true),
   mConnectionInfo(connectionInfo),
   mSocket(connectionInfo.sock),
   mLocalAddr(NULL),
   mBroken(false),
   mRemoteAddr(connectionInfo.remoteBDAddr)
{
    LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
    setLocalAddress();
    start();
}
BluetoothAdapter::~BluetoothAdapter()
{
    LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
    close(mSocket);
    delete[] mRemoteAddr;
    mRemoteAddr = NULL;
    delete[] mLocalAddr;
    mLocalAddr = NULL;
}

bool BluetoothAdapter::isBroken()  const
{
    LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

	if (mBroken)
	{
		return true;
	}

    hci_conn_info_req *conInfoReq;
    int devDescr;
    conInfoReq = (hci_conn_info_req*)malloc(sizeof(*conInfoReq) + sizeof(hci_conn_info));
    if (!conInfoReq) 
    {
        LOG4CPLUS_ERROR(logger,"Can't allocate memory for connection info");
        return true;
    }

    devDescr = hci_open_dev(mConnectionInfo.adapterId);
    if (devDescr < 0) 
    {
        LOG4CPLUS_ERROR(logger,"HCI device open failed");
        free(conInfoReq);
        return true;
    }

    str2ba(mConnectionInfo.remoteBDAddr, &conInfoReq->bdaddr);
    conInfoReq->type = ACL_LINK;
    mHciMutex.lock();
    int res = ioctl(devDescr, HCIGETCONNINFO, (unsigned long) conInfoReq);
    ::sleep(2);     // need because requests to HCI is very slow
    if (-1 == res) 
    {
        LOG4CPLUS_ERROR(logger,"Get connection info failed ");
        free(conInfoReq);
        mHciMutex.unlock();
        return true;
    }
    else
    {
        char addr[BDADDR_SIZE];
        ba2str(&conInfoReq->conn_info->bdaddr, addr);
        LOG4CPLUS_INFO(logger, "Connection with:\t" + std::string(addr));
        LOG4CPLUS_INFO(logger, "Connection handle:\t"
                                + convertIntegerToString(conInfoReq->conn_info->handle));
        LOG4CPLUS_INFO(logger, "Connection state:\t"
                                + convertIntegerToString(conInfoReq->conn_info->state));
    }
    mHciMutex.unlock();
    free(conInfoReq);
    return false;
}

ConnectivityAgentError  BluetoothAdapter::makeHandshake()
{
    LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
    if(mConnectionInfo.isServer)
    {
        mGenderType = eServerGender;
    } 
    else 
    {
        mGenderType = eClientGender;
    }
    return ConnectivityAgentError::NoError();
}
 
const char* BluetoothAdapter::getRemoteAddress()  const
{
    LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
    return mConnectionInfo.remoteBDAddr; 
}

const char* BluetoothAdapter::getLocalAddress()  const
{
    LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
    return mLocalAddr; 
}
void BluetoothAdapter::setLocalAddress()
{
    LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
    hci_dev_info devInfo;
    int devDescr;
    mLocalAddr = new char[BDADDR_SIZE];

    devInfo.dev_id = mConnectionInfo.adapterId;
    devDescr = hci_open_dev(mConnectionInfo.adapterId);

    if (0 <= devDescr)
    {
        if (-1 == ioctl(devDescr, HCIGETDEVINFO, (void *) &devInfo))  // get local device info
        {
            LOG4CPLUS_ERROR(logger, "Can't read local device info");
        }
        else
        {
            ba2str(&devInfo.bdaddr, mLocalAddr);
        }
        hci_close_dev(devDescr);
    }
    else
    {
        LOG4CPLUS_ERROR(logger, "Get device descriptor FAILED");
    }
}

const char* BluetoothAdapter::getTypeName()  const
{
    LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
    return "Bluetooth";
}

ConnectivityAgentError BluetoothAdapter::sendPreparedArray(const UInt8* pArray, const UInt32 size)
{
    LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
    if (!pArray || (size <= 0) )
    {
        LOG4CPLUS_ERROR(logger, "sendPreparedArray called with incorrect pArray or size");
        return ConnectivityAgentError(ConnectivityAgentError::ERROR_INCONSISTENT_DATA);
    }

    ssize_t sendRes = -1;
    size_t sent = 0;

    mSocketMutex.lockRead();
    {
        do 
        {
            sendRes = ::send(mSocket, pArray + sent, size - sent, 0);            
         
            if (-1 != sendRes)
            {
                sent += sendRes;
            }
        } 
        while ((sendRes != -1 && sent < size) || 
                (sendRes == -1 && errno == EINTR));
    }
    mSocketMutex.unlockRead();

    if (-1 == sendRes)
    {
        return ConnectivityAgentError(ConnectivityAgentError::ERROR_BROKEN_PIPE);
    }
    else
    {
        if (sent != size)
        {
            LOG4CPLUS_ERROR(logger, "Sent not all data!");
        }
    }

   return ConnectivityAgentError::NoError();   
}

ConnectivityAgentError BluetoothAdapter::receiveRawArray(UInt8* pArray, UInt32 &size)
{
    LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
    ssize_t recvRes = -1;
    int saved_errno = 0;

    ConnectivityAgentError receiveResult = ConnectivityAgentError::NoError();

    mSocketMutex.lockRead();
    {
        do 
        {
            recvRes = ::recv(mSocket, pArray, size, 0);
        } 
        while (recvRes == -1 && errno == EINTR);

        saved_errno = errno;
    }
    mSocketMutex.unlockRead();

    if (recvRes > 0)
    {
        size = recvRes;
    }
    else
    {
        LOG4CPLUS_ERROR(logger, "Recv error: " 
                                + convertIntegerToString(saved_errno) 
                                + strerror(saved_errno));
        receiveResult.setErrorCode(ConnectivityAgentError::ERROR_BROKEN_PIPE);
    }

   return receiveResult;
}

bool BluetoothAdapter::start()
{
    LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
    return CThread::start();
}

bool BluetoothAdapter::stop()
{
    LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
    return CThread::stop();
}

void BluetoothAdapter::threadFunc()
{
    LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
    fd_set* const rSet = &mRFdSet;

    bool needExit = false;

    while (true)
    {
        LOG4CPLUS_INFO(logger, "BluetoothCarrierAdapter::threadFunc() iteration");
        bool sockSet = false;
        bool pipeSet = false;

        mSocketMutex.lockRead();
        {
            FD_ZERO(rSet);

            int maxFD = -1;

            FD_SET(mSelfpipe.getReadingEnd(), rSet);
            maxFD = std::max(mSelfpipe.getReadingEnd(), maxFD);

            if (-1 != mSocket)
            {
                FD_SET(mSocket, rSet);
                maxFD = std::max(mSocket, maxFD);
            }

            // Must be max+1
            ++maxFD;

            int res = -1;
            do
            {
                LOG4CPLUS_INFO(logger, "Before select maxFD = " + convertIntegerToString(maxFD));
                res = ::select(maxFD, rSet, NULL, NULL, NULL);
            } 
            while (res == -1 && errno == EINTR);
            LOG4CPLUS_INFO(logger, "Select res = " + convertIntegerToString(res));

            if (-1 == res)
            {
                /// @todo error handling
                LOG4CPLUS_INFO(logger, "Select failed, needExit");
                needExit = true;
				mBroken = true;
            }
            else
            {
                pipeSet = FD_ISSET(mSelfpipe.getReadingEnd(), rSet);

                if (-1 != mSocket)
                {
                    sockSet = FD_ISSET(mSocket, rSet);
                }
            } // if res < -1
        }
        mSocketMutex.unlockRead();

        if (needExit)
            break;

        if (pipeSet)
        {
            Selfpipe::tSignal sig;

            if (!mSelfpipe.readSignal(sig).isNoError())
            {
                break;
            }

            switch (sig)
            {
                case eGameOver:
                {
                    needExit = true;
					mBroken = true;
                    break;
                }

                default:
                {
                    LOG4CPLUS_ERROR(logger, "Catch unknown signal:\t" << sig);
                    break;
                }
            }

            if (needExit)
                break;

            continue;
        }

        if (sockSet)
        {
            haveData();
        }

    } // while
}
BluetoothAdapter::Selfpipe::Selfpipe()
   : mReady(false)
{
    mSelfpipe[0] = mSelfpipe[1] = -1;
    mReady = (setup().isNoError());
}

BluetoothAdapter::Selfpipe::~Selfpipe()
{
    destroy();
}

ConnectivityAgentError BluetoothAdapter::Selfpipe::setup()
{
    LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
    int res = ::pipe(mSelfpipe);

    if (-1 == res)
    {
        int saved_errno = errno;
        LOG4CPLUS_ERROR(logger, "pipe error: " + convertIntegerToString(saved_errno)
                                + strerror(saved_errno));
        return ConnectivityAgentError(ConnectivityAgentError::ERROR_CONNECT_FAILED);
    }
   
    if (-1 == fcntl(mSelfpipe[1], F_SETFL, fcntl(mSelfpipe[1], F_GETFL) | O_NONBLOCK))
    {
        return ConnectivityAgentError(ConnectivityAgentError::ERROR_CONNECT_FAILED);
    }

    if (-1 == fcntl(mSelfpipe[0], F_SETFD, FD_CLOEXEC))
    {
        return ConnectivityAgentError(ConnectivityAgentError::ERROR_CONNECT_FAILED);
    }
    if (-1 == fcntl(mSelfpipe[1], F_SETFD, FD_CLOEXEC))
    {
        return ConnectivityAgentError(ConnectivityAgentError::ERROR_CONNECT_FAILED);
    }

    return ConnectivityAgentError::NoError();
}

void BluetoothAdapter::Selfpipe::destroy()
{
    LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

    if (-1 != mSelfpipe[0])
    {
        close(mSelfpipe[0]);
    }
    if (-1 != mSelfpipe[1])
    {
        close(mSelfpipe[1]);
    }

    mSelfpipe[0] = mSelfpipe[1] = -1;
}

bool BluetoothAdapter::Selfpipe::isReady() const
{
    return mReady;
}

int BluetoothAdapter::Selfpipe::getReadingEnd() const
{
    LOG4CPLUS_INFO(logger, __PRETTY_FUNCTION__);
    return mSelfpipe[0];
}

ConnectivityAgentError BluetoothAdapter::Selfpipe::signal(tSignal signal)
{
    LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
    ConnectivityAgentError error = ConnectivityAgentError::NoError();
    ssize_t res = -1;

    do 
    {
        res = write(mSelfpipe[1], &signal, sizeof(signal));
    } 
    while (res == -1 && errno == EINTR);

    if (-1 == res)
    {
        int saved_errno = errno;
        LOG4CPLUS_ERROR(logger, "selfpipe write error: "
                                + convertIntegerToString(saved_errno) + strerror(saved_errno));
        error.setErrorCode(ConnectivityAgentError::ERROR_OTHER);
    }

    return error;
}

ConnectivityAgentError BluetoothAdapter::Selfpipe::readSignal(tSignal& signal)
{
    LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
    ConnectivityAgentError error = ConnectivityAgentError::NoError();
    ssize_t res = -1;

    do 
    {
        res = read(mSelfpipe[0], &signal, sizeof(signal));
    } 
    while (res == -1 && errno == EINTR);

    if (-1 == res)
    {
        int saved_errno = errno;
        LOG4CPLUS_ERROR(logger, "selfpipe read error: "
                                + convertIntegerToString(saved_errno) + strerror(saved_errno));
        error.setErrorCode(ConnectivityAgentError::ERROR_OTHER);
    }

    return error;
}

#endif //ANDROID
