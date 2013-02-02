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


#include <assert.h>
#include <time.h>
#include <unistd.h>

#include "CTransmittedFramesQueue.hpp"
#include "CSource.hpp"
#include "Frame.hpp"
#include "CCarrierAdapter.hpp"
#include "IBufferProducer.hpp"
#include "CTransmitDispatcher.hpp"

#include "Logger.hpp"

using iviLink::ConnectivityAgent::HAL::CCarrierAdapter;
using namespace iviLink::ConnectivityAgent::L0;
using namespace iviLink::ConnectivityAgent;

Logger CTransmitDispatcher::logger = Logger::getInstance(
        LOG4CPLUS_TEXT("ConnectivityAgent.L0.CTransmitDispatcher"));

CTransmitDispatcher::CTransmitDispatcher(CCarrierAdapter* pCarrierAdaptor,
        CTransmittedFramesQueue* pTransmittedFrameQueue)
        : CThread("TransmitDispatcher"),
          mpCarrier(pCarrierAdaptor),
          mpTransmittedFrameQueue(pTransmittedFrameQueue),
          mDestroyed(false)
{
    mRegistry.reserve(PRIO_NUM);
    TSourceList list;

    for (int i = 0; i < PRIO_NUM; ++i)
    {
        mRegistry.push_back(list);
    }
    start();
}

ConnectivityAgentError CTransmitDispatcher::openChannel(const UInt32 channel_id,
                                    const TChannelPriority prio,
                                    iviLink::ConnectivityAgent::L1::IBufferProducer& bufferProducer)
{
    CSource* pSource = NULL;
    LOG4CPLUS_TRACE(logger, "CTransmitDispatcher::openChannel(chID = "
                    + convertIntegerToString(channel_id)
                    + ", prio = " + convertIntegerToString((int) prio) + ", bufProd = "
                    + convertIntegerToString((intptr_t) &bufferProducer) + ")");
 	ConnectivityAgentError result = ConnectivityAgentError::NoError();
    mRegistryMutex.lock();
    TSourceList::iterator iter = mRegistry[static_cast<UInt8>(prio)].begin();
    while (iter != mRegistry[static_cast<UInt8>(prio)].end())
    {
        if (channel_id == static_cast<CSource*>(*iter)->getChannelID())
        {
         	result.setErrorCode(ConnectivityAgentError::ERROR_CHANNEL_BUSY);
            break;
        }
        ++iter;
    }
    if (iter == mRegistry[static_cast<UInt8>(prio)].end())
    {
        pSource = new CSource(channel_id, mEventSem);
        LOG4CPLUS_INFO(logger, "CTransmitDispatcher::openChannel() number = "
                        + convertIntegerToString(channel_id)
                        + " prio = " + convertIntegerToString(static_cast<UInt8>(prio))
                        + " opening new channel");
        pSource->registerProducer(&bufferProducer);
        bufferProducer.registerConsumer(pSource);
        mRegistry[prio].push_back(pSource);
    }
    mRegistryMutex.unlock();
    return result;
}

ConnectivityAgentError CTransmitDispatcher::closeChannel(const UInt32 channel_id)
{
   LOG4CPLUS_TRACE(logger, "CTransmitDispatcher::closeChannel(channel_id = " +
         convertIntegerToString(channel_id) + ")");

   ConnectivityAgentError result(ConnectivityAgentError::ERROR_NOT_FOUND);
   TSourceList::iterator iter;

    mpTransmittedFrameQueue->deleteFramesForChannel(channel_id);

    mRegistryMutex.lock();

    for (int i = 0; i < PRIO_NUM; ++i)
    {
        iter = mRegistry[i].begin();
        while (iter != mRegistry[i].end())
        {
            if (channel_id == static_cast<CSource*>(*iter)->getChannelID())
            {
                break;
            }
            ++iter;
        }
        if (iter != mRegistry[i].end())
        {
            LOG4CPLUS_INFO(logger, "CTransmitDispatcher::closeChannel(channel_id = "
                            + convertIntegerToString(channel_id) + ") deleting CSource");
            delete *iter;
            mRegistry[i].erase(iter);
         	result.setNoError();
            break;
        }
    }
    mRegistryMutex.unlock();
	return result;
}

CTransmitDispatcher::~CTransmitDispatcher()
{
    LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
    CThread::stop(&mEventSem);
    mRegistryMutex.lock();

    mDestroyed = true;

    for (int i = 0; i < PRIO_NUM; ++i)
    {
        for (TSourceList::iterator iter = mRegistry[i].begin(); iter != mRegistry[i].end(); ++iter)
        {
            delete *iter;
        }

        mRegistry[i].clear();
    }

    mRegistryMutex.unlock();
}

void CTransmitDispatcher::threadFunc()
{
    iviLink::ConnectivityAgent::HAL::Frame frame;

    LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

    UInt32 realTimeMessagesLimit = 2;
    UInt32 realTimeMessagesCounter = 0;

    while (true)
    {
        mEventSem.waitTimeout(500);
        if (getStopFlag())
        {
            break;
        }

        mRegistryMutex.lock();

        if (mDestroyed)
        {
            mRegistryMutex.unlock();
            break;
        }

        for (int i = 0; i < PRIO_NUM; ++i)
        {
            TChannelPriority currentPriority = static_cast<TChannelPriority>(i);
            if (!mRegistry[i].empty())
            {
                for (TSourceList::iterator iter = mRegistry[i].begin();
                        iter != mRegistry[i].end();
                        ++iter)
                {
                    if (static_cast<CSource*>(*iter)->hasData())
                    {
                        if (currentPriority != eSafetyCritical)
                        {
                            if (hasCertainPriorityData(eSafetyCritical))
                            {
                                break;
                            }
                        }
                        if ((*iter)->OnDispatch(frame, getQuoteBySourceType(currentPriority)).isNoError())
                        {
                            mpTransmittedFrameQueue->lockQueue();
                            if (mpCarrier->sendFrame(&frame).isNoError())
                            {
                                mpTransmittedFrameQueue->enqueue(frame);
                            }
                            else
                            {
                                LOG4CPLUS_ERROR(logger, "Could not send frame chID = "
                                + convertIntegerToString(frame.mFrameHeader.channel_id)
                                + ", num = " + convertIntegerToString(frame.mFrameHeader.number));
                            }
                            mpTransmittedFrameQueue->unlockQueue();

                            if (currentPriority == eRealTime)
                            {
                                if (realTimeMessagesCounter < (realTimeMessagesLimit - 1))
                                {
                                    realTimeMessagesCounter++;
                                }
                                else
                                {
                                    realTimeMessagesCounter = 0;
                                    break;
                                }
                            }
                            else if (currentPriority == ePlainData)
                            {
                                if (hasCertainPriorityData(eRealTime))
                                {
                                    break;
                                }
                            }
                        }
                    }
                }
            }

            // retransmission loop
            iviLink::ConnectivityAgent::HAL::Frame* pFrame;
            while ((pFrame = mpTransmittedFrameQueue->getFrameToRetransmit()) != NULL)
            {
                LOG4CPLUS_INFO(logger, "CTransmitDispatcher::threadFunc() retransmit frame chID = "
                                + convertIntegerToString(pFrame->mFrameHeader.channel_id)
                                + ", num = " + convertIntegerToString(pFrame->mFrameHeader.number));
                mpCarrier->sendFrame(pFrame);
                mpTransmittedFrameQueue->reenqueueRetransmittedFrame(pFrame);
            }

        }
        mRegistryMutex.unlock();

    } // while (true)
}

UInt32 CTransmitDispatcher::getQuoteBySourceType(TChannelPriority type)
{
    switch (type)
    {
        case eSafetyCritical:
            return SC_QUOTE;
        case eRealTime:
            return RT_QUOTE;
        case ePlainData:
            return PD_QUOTE;
        default:
            return 0xFFFFFFFF;
    }
}

void CTransmitDispatcher::replaceCarrier(CCarrierAdapter* pCarrierAdaptor)
{
    mpCarrier = pCarrierAdaptor;
}

bool CTransmitDispatcher::hasCertainPriorityData(const TChannelPriority priority)
{
    if (!mRegistry[priority].empty())
    {
        for (TSourceList::iterator iter = mRegistry[priority].begin();
                iter != mRegistry[priority].end();
                ++iter)
        {
            if (static_cast<CSource*>(*iter)->hasData())
                return true;
        }
    }

    return false;
}
