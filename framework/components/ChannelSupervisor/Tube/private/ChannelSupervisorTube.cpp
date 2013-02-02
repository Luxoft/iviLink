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


#include <cstdlib>
#include <iostream>
#include <string>
#include <map>

#include "ConnectivityAgentError.hpp"
#include "Types.hpp"
#include "IChannelObserver.hpp"
#include "IChannelSupervisorObserver.hpp"
#include "API.hpp"
#include "NegotiatorClient.hpp"
#include "ChannelSupervisorTube.hpp"
#include "ChannelObserver.hpp"
#include "Common.hpp"
#include "CSError.hpp"

#include "CMutex.hpp"

using namespace iviLink::ConnectivityAgent;

const char defErrMsg[] = "no error occurred";
static Logger logger = Logger::getInstance(
		LOG4CPLUS_TEXT("ChannelSupervisor.ChannelSupervisorTube"));
#define ALLOCATION_ATTEMPTS 100

namespace iviLink
{

namespace ChannelSupervisor
{

static std::map<int, IChannelObserver*> observerMap;
static int channel = 0;
static CMutex gMutex;

/**
 * Negotiates channel id with other party and allocates the channel
 * @param observer is registration of the observer for Channel supervisor events receiving
 * @param tag is tag of the channel
 * @param channelId is channel id
 * @param priority is channel priority
 * @return CS_SUCCESS if channel successfully allocated, otherwise CS_ERR_OTHER
 */
BaseError allocateChannel(IChannelSupervisorTubeObserver* observer,
                       std::string const& tag,
                       UInt32 & channelId,
                       TChannelPriority priority)
{
	LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
	MutexLocker globalLock(gMutex);
	LOG4CPLUS_TRACE(logger, "ChannelSupervisorTube::AllocateChannel()=>tag" + tag);

	BaseError err = CSError(CSError::ERROR_OTHER);
	if (observer == NULL)
	{
		return err;
	}

	//create negotiator client object
	NegotiatorClient* negotiatorClient = NegotiatorClient::getInstance();

	IChannelObserver* obs = new ChannelObserver(observer);
	MutexLocker lock(*(((ChannelObserver *) obs)->getMutex()));

	UInt32 i = 0;
	channelId = 0;

	//do negotiation until 1) timeout 2) number of negotiation exceed confuration 3) cid obtained
	while (i < ALLOCATION_ATTEMPTS)
	{
		//negotiate channel with other side
		err = negotiatorClient->NegotiateChannel(tag, channelId);
		LOG4CPLUS_INFO(logger,
				"ChannelSupervisorTube::AllocateChannel()=>Negotiated id "
						+ convertIntegerToString(channelId));

		if (channelId != 0 && err.isNoError())
		{
			observerMap[channel] = obs;

			//allocate channel in Connectivity Agent
			LOG4CPLUS_INFO(logger, "ChannelSupervisorTube:: before AllocateChannel() in CS=>priority "
								+ convertIntegerToString((int)priority));
			ConnectivityAgentError caError = ::allocateChannel(priority, channelId, obs);

			if (caError.isNoError())
			{
				LOG4CPLUS_INFO(logger,
						"ChannelSupervisorTube::AllocateChannel()=>Allocated channelId "
								+ convertIntegerToString(channelId));

				err = negotiatorClient->UpdateMapWithCID(tag, channelId);
				if (err.isNoError())
				{
					err = CSError::NoCSError(defErrMsg);
					break;   //channel successfully allocated
				} else if (err.getCode() != CSError::UPDATE_MAP_WRONG_CID)
				{
					break; //error in map update
				}
			} 
			else if (caError.getCode() == ConnectivityAgentError::ERROR_CHANNEL_BUSY)
			{
				LOG4CPLUS_ERROR(logger,
						std::string("This channel was already allocated, ")
								+ convertIntegerToString(channelId)
								+ std::string(" trying a higher number"));
				continue;
			} 
			else if (caError.getCode() == ConnectivityAgentError::ERROR_ALLOCATION_IN_PROGRESS) 
			{
				err = CSError(CSError::ALLOCATION_ALREADY_IN_PROGRESS, "allocation channel error: already in progress");
				break;   //error in CA. unable to allocate
			} 
			else if (caError.getCode() == ConnectivityAgentError::ERROR_REQUEST_FAILED) 
			{
				err = CSError(CSError::IPC_NO_DATA_ERROR, "allocation channel error: no response from CA");
				break;   //error in CA. unable to allocate
			}  
			else
			{
				err = CSError(CSError::ERROR_OTHER, "allocation channel error");
				break;   //error in CA. unable to allocate
			}
		} 
		else if (err.getCode() != CSError::NEGOTIATION_CHANNEL_ERROR)
		{
			LOG4CPLUS_ERROR(logger,
					"ChannelSupervisorTube::AllocateChannel()=>err.getCode() \
                    != CSError::NEGOTIATION_CHANNEL_ERROR, breaking...");
			break;   //error in negotiation
		}
		err = CSError(CSError::ATTEMPTS_LIMIT_EXCEEDED,
				"unable to allocate channel. Number of attempts exceed limit");
		++i;
	}
	return err;
}

/**
 * Deallocates the channel with the given id
 * @param channelId - id of the channel to be deallocated
 * @param tag     - tag of the channel
 * @return CS_SUCCESS if channel successfully deallocated, otherwise CS_ERR_OTHER
 */
BaseError deallocateChannel(const UInt32 channelId)
{

	LOG4CPLUS_TRACE_METHOD(logger,
			"ChannelSupervisorTube::deallocateChannel()=> channel "
					+ convertIntegerToString(channelId));
	MutexLocker globalLock(gMutex);
	if (isIdRestricted(channelId))
	{
		LOG4CPLUS_ERROR(logger,
				"Trying to deallocate a restricted channel: "
						+ convertIntegerToString((int) channelId));
		return CSError(CSError::ERROR_OTHER);
	}

	NegotiatorClient* negotiatorClient = NegotiatorClient::getInstance();

	BaseError err = CSError::NoCSError(defErrMsg);
	//do real channel deallocation in CA
	ConnectivityAgentError caError = ::closeChannel(channelId);
	LOG4CPLUS_TRACE(logger,
			"ChannelSupervisorTube::deallocateChannel()=>CA error code"
					+ convertIntegerToString((int) caError.getCode()));
	if (caError.getCode() == ConnectivityAgentError::ERROR_REQUEST_FAILED)
	{
		return CSError(CSError::IPC_NO_DATA_ERROR);
	}

	std::map<int, IChannelObserver*>::iterator it;
	it = observerMap.find(channelId);
	if (it != observerMap.end())
	{
		ChannelObserver * obs = (ChannelObserver*) (*it).second;

		LOG4CPLUS_TRACE(logger,
				"ChannelSupervisorTube::deallocateChannel()=>observer found, delete");
		if (obs)
			delete obs;
		obs = 0;
		observerMap.erase(it);

	} else
	{
		LOG4CPLUS_TRACE(logger,
				"ChannelSupervisorTube::deallocateChannel()=>observer not found!!!");
	}

	// Even in case of error we try to free channel from negotiator

	//deallocate the channel (just remove from negotiator map)
	// we don't process mistakes here as it is not important
	negotiatorClient->FreeChannel(channelId);
	LOG4CPLUS_TRACE(logger,
			"ChannelSupervisorTube::deallocateChannel()=>deallocated channel "
					+ convertIntegerToString(channelId));

	return err;
}

/**
 * Sends the data through the channel
 * @param channelId - id of the channel
 * @param tag     - tag of the channel
 * @param data    - pointer to the data array
 * @param size    - data length
 * @return CS_SUCCESS if channel successfully sent, otherwise CS_ERR_OTHER
 */
BaseError sendData(const UInt32 channelId, UInt8 const* data, UInt32 size)
{
	LOG4CPLUS_TRACE(logger,
			"ChannelSupervisorTube::sendData()=>channel " + convertIntegerToString(channelId)
					+ "size " + convertIntegerToString(size));
	if (channelId != CA_SERVICE_CHANNEL)
	{
        ConnectivityAgentError caError = ::sendData(channelId, data, size);
        if (!caError.isNoError())
        {
        	if (caError.getCode() == ConnectivityAgentError::ERROR_NOT_FOUND)
        	{
        		return CSError(CSError::SEND_TO_NOT_EXISTING_CHANNEL, "Channel " + convertIntegerToString(channelId) + " is not allocated");
        	}
        	if(caError.getCode() == ConnectivityAgentError::ERROR_REQUEST_FAILED)
        	{
        		return CSError(CSError::IPC_NO_DATA_ERROR, "ConnectivityAgent not responding, try again later");
        	}
        	return CSError(CSError::ERROR_OTHER, "Some other error");
        }
        return CSError::NoCSError(defErrMsg);
	} 
	else
	{
		LOG4CPLUS_ERROR(logger, "Attempting to send data to CA service channel");
		return CSError(CSError::SEND_TO_RESTRICTED_CHANNEL, "Attempting to send data to CA service channel");
	}
}

/**
 * Receives the data from the channel
 * @param channelId     - id of the channel
 * @param tag        - tag of the channel
 * @param data       - pointer to the data array to be read
 * @param receivedSize  - data length
 * @param maxSize    - size of the array the data will be read to
 * @return CS_SUCCESS if channel successfully read, otherwise CS_ERR_OTHER
 */
BaseError receiveData(const UInt32 channelId, UInt8 * data, UInt32 & receivedSize,
		const UInt32 maxSize)
{
	ConnectivityAgentError caError = ::receiveData(channelId, data, receivedSize, maxSize);
	LOG4CPLUS_TRACE_METHOD(logger,
			"ChannelSupervisorTube::receiveData()=>channel " + convertIntegerToString(channelId)
					+ " size " + convertIntegerToString(receivedSize) + " err "
					+ convertIntegerToString((int) caError.getCode()));
	if (caError.isNoError())
	{
		return CSError::NoCSError(defErrMsg);
	} else
	{
		return CSError(CSError::ERROR_OTHER, "::receiveData failed");
	}
}

BaseError getFreeSize(const UInt32 channelId, UInt32 & freeSize)
{
	ConnectivityAgentError caError = ::getFreeSize(channelId, freeSize);
	LOG4CPLUS_TRACE_METHOD(logger,
			"ChannelSupervisorTube::getFreeSize()=>channel " + convertIntegerToString(channelId)
					+ " freeSize " + convertIntegerToString(freeSize) + " err "
					+ convertIntegerToString((int) caError.getCode()));
	if (caError.isNoError())
	{
		return CSError::NoCSError(defErrMsg);
	} 
	else
	{
		return CSError(CSError::ERROR_OTHER, "::getFreeSize failed");
	}
}

/**
 * Kicks watchdog for the given channel
 * @param channelId     - id of the channel
 * @param tag        - tag of the channel
 * @return CS_SUCCESS if channel successfully kicked, otherwise CS_ERR_OTHER
 */
BaseError kickWatchdog(const UInt32 channelId, std::string const& tag)
{
    LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
	MutexLocker globalLock(gMutex);
    NegotiatorClient * negotiatorClient = NegotiatorClient::getInstance();
    BaseError err = CSError(CSError::ERROR_OTHER);

    err = negotiatorClient->UpdateChannelInfo(tag, channelId);
	if (err.isNoError())
	{
		return CSError::NoCSError(defErrMsg);
	} 
	else
	{
		return CSError(CSError::ERROR_OTHER, "Kick faild channelId " 
        			+ convertIntegerToString(channelId));
	}
}

/**
 * Negotiates the service channel id with other party and allocates the channel
 * @param observer   - registration of the observer for Channel supervisor events receiving
 * @param tag     - tag of the channel
 * @param channelId - channel id
 * @return CS_SUCCESS if channel successfully allocated, otherwise CS_ERR_OTHER
 */
BaseError allocateServiceChannel(IChannelSupervisorTubeObserver* observer, const UInt32 channelId)
{
	LOG4CPLUS_TRACE_METHOD(logger,
			"ChannelSupervisorTube::allocateServiceChannel() => channel "
					+ convertIntegerToString(channelId));

	BaseError err = CSError(CSError::ERROR_OTHER);
	if (observer == NULL)
	{
		return err;
	}
	
	IChannelObserver* obs = new ChannelObserver(observer);

	observerMap[channel] = obs;

	//allocate channel in Connectivity Agent
	ConnectivityAgentError caError = ::allocateChannel(eSafetyCritical, channelId, obs);

	if (caError.isNoError())
	{
		LOG4CPLUS_INFO(logger,
				"ChannelSupervisorTube::allocateServiceChannel() => \
            channel allocated successfully!");
		err = CSError::NoCSError("Service allocated");
	}
	return err;
}

bool isIdRestricted(const UInt32 numberToCheck)
{
	return numberToCheck < CHANNEL_ID_START_NUMBER;
}
} //namespace ChannelSupervisor
} //namespace AXIS
