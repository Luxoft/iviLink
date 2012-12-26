/* 
 * iviLINK SDK, version 1.1.19
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
 */ 
 

#include <cstdlib>
#include <iostream>
#include <string>
#include <map>
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
CError allocateChannel(IChannelSupervisorTubeObserver* observer,
                       std::string const& tag,
                       UInt32 & channelId,
                       TChannelPriority priority)
{
	LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
	MutexLocker globalLock(gMutex);
	LOG4CPLUS_TRACE(logger, "ChannelSupervisorTube::AllocateChannel()=>tag" + tag);

	CError err = CSError(CSError::ERROR_OTHER);
	if (observer == NULL)
	{
		return err;
	}

	//create negotiator client object
	NegotiatorClient* negotiatorClient = NegotiatorClient::getInstance();

	IChannelObserver* obs = new ChannelObserver(observer);
	MutexLocker lock(*(((ChannelObserver *) obs)->getMutex()));

	UInt32 i = 0;

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
			ERROR_CODE caError = ::allocateChannel(priority, channelId, obs);

			if (caError == ERR_OK)
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
			} else if (caError == ERR_NUMBER_BUSY)
			{
				LOG4CPLUS_ERROR(logger,
						std::string("This channel was already allocated, ")
								+ convertIntegerToString(channelId)
								+ std::string(" trying a higher number"));
				continue;
			} else
			{
				err = CSError(CSError::ERROR_OTHER, "allocation channel error");
				break;   //error in CA. unable to allocate
			}
		} else if (err.getCode() != CSError::NEGOTIATION_CHANNEL_ERROR)
		{
			LOG4CPLUS_ERROR(logger,
					"ChannelSupervisorTube::AllocateChannel()=>err.getCode() \
                    != CSError::NEGOTIATION_CHANNEL_ERROR, breaking...");
			break;   //error in negotiation
		}
		err = CSError(CSError::ERROR_OTHER,
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
CError deallocateChannel(const UInt32 channelId)
{

	LOG4CPLUS_TRACE_METHOD(logger,
			"ChannelSupervisorTube::deallocateChannel()=> channel "
					+ convertIntegerToString(channelId));

	if (isIdRestricted(channelId))
	{
		LOG4CPLUS_ERROR(logger,
				"Trying to deallocate a restricted channel: "
						+ convertIntegerToString((int) channelId));
		return CSError(CSError::ERROR_OTHER);
	}

	NegotiatorClient* negotiatorClient = NegotiatorClient::getInstance();

	CError err = CSError::NoCSError(defErrMsg);
	//do real channel deallocation in CA
	ERROR_CODE caError = ::closeChannel(channelId);
	LOG4CPLUS_TRACE(logger,
			"ChannelSupervisorTube::deallocateChannel()=>CA error code"
					+ convertIntegerToString((int) caError));

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

	if (caError != ERR_OK)
	{
		err = CSError(CSError::ERROR_OTHER, "deallocation channel error");
	}
	// Even in case of error we try to free channel from negotiator

	//deallocate the channel (just remove from negotiator map)
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
CError sendData(const UInt32 channelId, UInt8 const* data, UInt32 size)
{
	LOG4CPLUS_TRACE(logger,
			"ChannelSupervisorTube::sendData()=>channel" + convertIntegerToString(channelId)
					+ "size" + convertIntegerToString(size));
	if (channelId != SERVICE_CHANNEL_NUMBER)
	{
		::sendData(channelId, data, size);
		return CSError::NoCSError(defErrMsg);
	} else
	{
		LOG4CPLUS_ERROR(logger, "Attempting to send data to CA service channel");
		return CSError(CSError::ERROR_OTHER, "Attempting to send data to CA service channel");
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
CError receiveData(const UInt32 channelId, UInt8 * data, UInt32 & receivedSize,
		const UInt32 maxSize)
{
	ERROR_CODE err = ::receiveData(channelId, data, receivedSize, maxSize);
	LOG4CPLUS_TRACE_METHOD(logger,
			"ChannelSupervisorTube::receiveData()=>channel " + convertIntegerToString(channelId)
					+ " size " + convertIntegerToString(receivedSize) + " err "
					+ convertIntegerToString((int) err));
	if (err == ERR_OK)
	{
		return CSError::NoCSError(defErrMsg);
	} else
	{
		return CSError(CSError::ERROR_OTHER, "::receiveData failed");
	}
}

CError getFreeSize(const UInt32 channelId, UInt32 & freeSize)
{
	ERROR_CODE err = ::getFreeSize(channelId, freeSize);
	LOG4CPLUS_TRACE_METHOD(logger,
			"ChannelSupervisorTube::getFreeSize()=>channel " + convertIntegerToString(channelId)
					+ " freeSize " + convertIntegerToString(freeSize) + " err "
					+ convertIntegerToString((int) err));
	if (err == ERR_OK)
	{
		return CSError::NoCSError(defErrMsg);
	} else
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
CError kickWatchdog(const UInt32 channelId, std::string tag)
{
	return CSError::NoCSError(defErrMsg);
}

/**
 * Negotiates channel id with other party and allocates the channel
 * @param observer   - registration of the observer for Channel supervisor events receiving
 * @param tag     - tag of the channel
 * @param channelId - channel id
 * @return CS_SUCCESS if channel successfully allocated, otherwise CS_ERR_OTHER
 */
CError init(IChannelSupervisorTubeObserver* observer)
{
	LOG4CPLUS_TRACE_METHOD(logger,
			"ChannelSupervisorTube::init() => channel "
					+ convertIntegerToString(CS_SERVICE_CHANNEL));

	CError err = CSError(CSError::ERROR_OTHER);
	if (observer == NULL)
	{
		return err;
	}

	IChannelObserver* obs = new ChannelObserver(observer);

	observerMap[channel] = obs;

	//allocate channel in Connectivity Agent
	ERROR_CODE caError = ::allocateChannel(eSafetyCritical, CS_SERVICE_CHANNEL, obs);

	if (caError == ERR_OK)
	{
		LOG4CPLUS_INFO(logger,
				"ChannelSupervisorTube::init() => \
            channel allocated successfully!");
		err = CSError::NoCSError("Service allocated");
	}
	return err;
}

bool isIdRestricted(const UInt32 numberToCheck)
{
	return numberToCheck == CS_SERVICE_CHANNEL || numberToCheck == SERVICE_CHANNEL_NUMBER;
}
} //namespace ChannelSupervisor
} //namespace AXIS
