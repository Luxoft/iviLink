/* 
 * 
 * iviLINK SDK, version 1.1.2
 * http://www.ivilink.net
 * Cross Platform Application Communication Stack for In-Vehicle Applications
 * 
 * Copyright (C) 2012, Luxoft Professional Corp., member of IBS group
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
 * 
 */

#include <cassert>
#include <strings.h>

#ifdef __APPLE__
#include "FatalErrorHandler.h"
#endif

#include "ChannelSupervisorTube.hpp"
#include "ProcessorsHolder.hpp"
#include "NegotiatorTube.hpp"
#include "Types.hpp"

using namespace iviLink::ChannelSupervisor;
using namespace iviLink::ChannelSupervisor::Messages;

Logger NegotiatorTube::mLogger = Logger::getInstance(
                LOG4CPLUS_TEXT("ChannelSupervisor.NegotiatorProcess.NegotiatorTube"));

NegotiatorTube::NegotiatorTube()
: CThread("NegotiatorTube", true)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
}

void NegotiatorTube::init()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    if (!allocateServiceChannel(this, CS_SERVICE_CHANNEL).isNoError())
    {
        LOG4CPLUS_FATAL(mLogger, "Could not allocate channel!");
#ifdef __APPLE__
        showDialogAndDie("Sorry! Could not establish Channel Supervisor communication");
#endif
    }
    CThread::start();
}

NegotiatorTube::~NegotiatorTube()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    deallocateChannel (CS_SERVICE_CHANNEL);
    mDataToSendQueueSema.signal();
    CThread::stop();
}

void NegotiatorTube::sendDataTube(std::string data)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    MutexLocker lock(mDataToSendQueueMutex);
    mDataToSendQueue.push_back(data);
    mDataToSendQueueSema.signal();
}

void NegotiatorTube::threadFunc()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    while (!getStopFlag())
    {
        mDataToSendQueueSema.wait();
        mDataToSendQueueMutex.lock();
        if (mDataToSendQueue.empty())
        {
            LOG4CPLUS_INFO(mLogger, "queue is empty, stopping sender thread");
            mDataToSendQueueMutex.unlock();
            return;
        }
        else
        {
            std::string dataToSend = mDataToSendQueue.front();
            mDataToSendQueue.pop_front();
            mDataToSendQueueMutex.unlock();
            LOG4CPLUS_INFO(mLogger, "send: \n" + dataToSend);
            UInt8 * castData = reinterpret_cast<UInt8*>(const_cast<char*>(dataToSend.c_str()));
            iviLink::ChannelSupervisor::sendData(CS_SERVICE_CHANNEL, castData, dataToSend.length());
        }
    }
}

void NegotiatorTube::onDataReceived(const UInt32 channel_id, const UInt32 read_size)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    UInt8 data[read_size];
    bzero(data, read_size);
    UInt32 receivedSize = 0;
    BaseError err = iviLink::ChannelSupervisor::receiveData(channel_id, data, receivedSize,
                    read_size);
    if (err.isNoError())
    {
        pugi::xml_document* doc = new pugi::xml_document();
        doc->load((char*) data);
        Notification * notification = new Notification(doc);
        if (notification->getType() == Notification::NOTIFICATIONTTYPE_CLIENT_DISCONNECTED)
        {
            ProcessorsHolder::getInstance()->processRemoteClientDisconnected(
                notification->getClientID());
            delete notification;
        }
        else if (notification->getType() == Notification::NOTIFICATIONTTYPE_INVALIDATE_TIMEDOUT_REQUEST)
        {
            ProcessorsHolder::getInstance()->invalidateRemoteTimedOutRequest(
                notification->getClientID(), notification->getRequestID());
            delete notification;
        }
        else
        {
            // created notification is deleted in processTubeNotification
            ProcessorsHolder::getInstance()->processTubeNotification(notification, this);
        }
    }
    else
    {
        LOG4CPLUS_WARN(mLogger, "Could not receive data: " + static_cast<std::string>(err));
    }
}

void NegotiatorTube::onBufferOverflow(const UInt32 channel_id)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
}

void NegotiatorTube::onChannelDeleted(const UInt32 channel_id)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
}

void NegotiatorTube::onConnectionLost()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
}
