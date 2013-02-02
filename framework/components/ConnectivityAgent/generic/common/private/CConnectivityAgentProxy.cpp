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


#include <cassert>
#include <cstdlib>
#include <utility>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "CIpc.hpp"

#include "CConnectivityAgentProxy.hpp"

#include "Logger.hpp"
#include "byteOrder.hpp"
#include "API.hpp"
#include "CallbackWrappers.hpp"

#include "ConnectivityAgentConfig.hpp"

using iviLink::Ipc::CIpc;
using iviLink::ConnectivityAgent::Buffer;
using iviLink::ConnectivityAgent::CDataAccessor;
using namespace iviLink::ConnectivityAgent;

CMutex CConnectivityAgentProxy::mSingletonMutex;
CConnectivityAgentProxy* CConnectivityAgentProxy::mSelf = 0;

/// @todo for testing
char const * gpCA_UNIX_SOCK_PATH = 0;

Logger CConnectivityAgentProxy::logger = Logger::getInstance(
		LOG4CPLUS_TEXT("ConnectivityAgent.common.CConnectivityAgentProxy"));

CConnectivityAgentProxy::CConnectivityAgentProxy()
		: CThread("CallbackTread"), mpIpc(NULL)
{
	LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
	static char const* const arr[] =
	{ gpCA_UNIX_SOCK_PATH, first_lsn_path, second_lsn_path };

	for (size_t i = 0; i < sizeof(arr) / sizeof(arr[0]); ++i)
	{
		if (arr[i])
		{
			LOG4CPLUS_INFO(logger,
					"Trying to connect to connectivity agent using address " + std::string(arr[i]));
			CIpc* pipc = new CIpc(arr[i], *this);
			BaseError err = pipc->connect();
			if (err.isNoError())
			{
				mpIpc = pipc;
				break;
			} else
			{
				LOG4CPLUS_WARN(logger, static_cast<std::string>(err));
				delete pipc;
			}
		}
	}

	if (!mpIpc)
	{
		LOG4CPLUS_ERROR(logger, "Unable to connect to connectivity agent");
	}

	start();
}

CConnectivityAgentProxy::~CConnectivityAgentProxy()
{
	LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
	stop(&mCallbackSema);
	delete mpIpc;
}

CConnectivityAgentProxy* CConnectivityAgentProxy::getInstance()
{
	mSingletonMutex.lock();
	if (mSelf == NULL)
	{
		mSelf = new CConnectivityAgentProxy();
	}
	mSingletonMutex.unlock();
	return mSelf;
}

void CConnectivityAgentProxy::deleteInstance()
{
	LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
	delete mSelf;
	mSelf = NULL;
}

void CConnectivityAgentProxy::OnConnection(iviLink::Ipc::DirectionID dirId)
{
	LOG4CPLUS_INFO(logger,
			"CConnectivityAgentProxy::OnConnection dirId = " + convertIntegerToString(dirId));
}

void CConnectivityAgentProxy::OnConnectionLost(iviLink::Ipc::DirectionID dirId)
{
	LOG4CPLUS_INFO(logger,
			"CConnectivityAgentProxy::onConnectionLost dirId = " + convertIntegerToString(dirId));
}

ConnectivityAgentError CConnectivityAgentProxy::allocateChannel(TChannelPriority prio, const UInt32 channel_id,
		IChannelObserver* observer)
{
	LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
	LOG4CPLUS_INFO(logger,
			"CConnectivityAgentProxy::allocateChannel(type = " + convertIntegerToString((int) prio)
					+ ", id = " + convertIntegerToString(channel_id) + " ), this = "
					+ convertIntegerToString((intptr_t) this));

	ConnectivityAgentError result;
	if ((channel_id > 0) && (channel_id <= 0xFFFF) && (NULL != observer))
	{
		mRegistryMutex.lockWrite();
		bool channel_unknown = mChannelRegistry.find(channel_id) == mChannelRegistry.end();
		mRegistryMutex.unlockWrite();

		if (channel_unknown)
		{
			mAllocateRequestCond.lock();
			bool channel_requested = mAllocateRequestMap.find(channel_id)
					!= mAllocateRequestMap.end();
			if (channel_requested)
			{
				result.setErrorCode(ConnectivityAgentError::ERROR_ALLOCATION_IN_PROGRESS);
				channel_unknown = false;
				LOG4CPLUS_WARN(logger,
						"CConnectivityAgentProxy::allocateChannel() => ERROR: channel allocation in progress!");
			} 
			else
			{
				LOG4CPLUS_INFO(logger,
						"CConnectivityAgentProxy::allocateChannel() => insert request");
				mAllocateRequestMap.insert(
						std::make_pair(channel_id,
								AllocateRequestInfo(prio, observer, false, ConnectivityAgentError::ERROR_OTHER)));
			}
			mAllocateRequestCond.unlock();
		}

		if (channel_unknown)
		{
			CDataAccessor requestDA;
			requestDA.setChannelID(channel_id);
			requestDA.setOpCode(E_ALLOCATE_CHANNEL);
			UInt32 data = prio;
			requestDA.setData(reinterpret_cast<UInt8 *>(&data), sizeof(UInt32));
			UInt8* buf = new UInt8[requestDA.getObjectSize()];
			requestDA.copyToRawArray(buf);

			UInt8 respBuf[100];
			UInt32 respSize = sizeof(respBuf);
			BaseError err = mpIpc->request(mMsgIdGen.next(), buf, requestDA.getObjectSize(), respBuf,
					respSize);
			delete[] buf;

			if (!err.isNoError())
			{
				LOG4CPLUS_WARN(logger, static_cast<std::string>(err));
				result.setErrorCode(ConnectivityAgentError::ERROR_REQUEST_FAILED);

				mAllocateRequestCond.lock();
				mAllocateRequestMap.erase(channel_id);
				mAllocateRequestCond.unlock();

				LOG4CPLUS_WARN(logger,
						"CConnectivityAgentProxy::allocateChannel() => ERROR: failed to send ipc request");
			} 
			else
			{
				CDataAccessor responseDA;

				if (respSize > 0)
				{
					responseDA = CDataAccessor(respBuf, respSize);
					result.setErrorCode(static_cast<ConnectivityAgentError::AgentErrorCodes>(responseDA.getErrorCode()));

					mAllocateRequestCond.lock();
					mAllocateRequestMap.erase(channel_id);
					mAllocateRequestCond.unlock();

					LOG4CPLUS_WARN(logger,
							"CConnectivityAgentProxy::allocateChannel() => ERROR: got error response");
				} 
				else
				{
					mAllocateRequestCond.lock();
					while (true)
					{
						tAllocateRequestMap::iterator it = mAllocateRequestMap.find(channel_id);
						if (it == mAllocateRequestMap.end())
						{
							result.setErrorCode(ConnectivityAgentError::ERROR_NOT_FOUND);
							LOG4CPLUS_WARN(logger,
									"CConnectivityAgentProxy::allocateChannel() => ERROR: request was removed");
							break;
						}

						AllocateRequestInfo & info = it->second;
						if (info.mRequestDone)
						{
							LOG4CPLUS_INFO(logger,
									"CConnectivityAgentProxy::allocateChannel() => request done");
							result.setErrorCode(info.mResult);
							mAllocateRequestMap.erase(it);
							break;
						}

						LOG4CPLUS_INFO(logger,
								"CConnectivityAgentProxy::allocateChannel() => before wait");
						mAllocateRequestCond.wait();
						LOG4CPLUS_INFO(logger,
								"CConnectivityAgentProxy::allocateChannel() => after wait");
					}
					mAllocateRequestCond.unlock();
				} // if response empty
			} // if no ipc err

		} else
		{
			LOG4CPLUS_ERROR(logger,
					"CConnectivityAgentProxy::allocateChannel() => ERROR: channel already exists! ");
			result.setErrorCode(ConnectivityAgentError::ERROR_CHANNEL_BUSY);
		}
	}

	return result;
}

ConnectivityAgentError CConnectivityAgentProxy::deallocateChannel(const UInt32 channel_id)
{
	LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
	LOG4CPLUS_INFO(logger,
			"CConnectivityAgentProxy::deallocateChannel(chID = "
					+ convertIntegerToString(channel_id) + ")");
	ConnectivityAgentError result(ConnectivityAgentError::ERROR_NOT_FOUND);

	mRegistryMutex.lockWrite();
	// Channel info will be saved in info and removed from registry here.
	// In case of some errors, it will be restored in registry.
	tChannelInfo info;
	tChannelsRegistryMap::iterator iter = mChannelRegistry.find(channel_id);
	if (iter == mChannelRegistry.end())
	{
		LOG4CPLUS_ERROR(logger,
				"CConnectivityAgentProxy::deallocateChannel(chID = "
						+ convertIntegerToString(channel_id) + ") => ERROR: Channel not found! ");
	} 
	else
	{
		result.setNoError();
		info = iter->second;
		iter->second.mChannelBuffer.forgetData();
		mChannelRegistry.erase(iter);
		mChannelOnDeallocSet.insert(channel_id);
	}
	mRegistryMutex.unlockWrite();

	if (!result.isNoError())
		return result;

	CDataAccessor requestDA;
	requestDA.setChannelID(channel_id);
	requestDA.setOpCode(E_DEALLOCATE_CHANNEL);
	UInt8* buf = new UInt8[requestDA.getObjectSize()];
	requestDA.copyToRawArray(buf);

	// response will be sent later as a separate request
	UInt32 respSize = 0;
	BaseError err = mpIpc->request(mMsgIdGen.next(), buf, requestDA.getObjectSize(), NULL, respSize);
	delete[] buf;

	if (!err.isNoError())
	{
		LOG4CPLUS_WARN(logger, static_cast<std::string>(err));
		result.setErrorCode(ConnectivityAgentError::ERROR_REQUEST_FAILED);
	} 
	else
	{
		CDataAccessor responseDA;

		mDeallocateRequestResultCond.lock();
		{
			LOG4CPLUS_INFO(logger,
					"CConnectivityAgentProxy::deallocateChannel waiting for mLastRequestResultDA");
			while (mDeallocateRequestResultMap.find(channel_id) == mDeallocateRequestResultMap.end())
			{
				LOG4CPLUS_INFO(logger, "before wait");
				mDeallocateRequestResultCond.wait();
				LOG4CPLUS_INFO(logger, "after wait");
			}

			responseDA = mDeallocateRequestResultMap[channel_id];
			mDeallocateRequestResultMap.erase(channel_id);
		}
		mDeallocateRequestResultCond.unlock();

		if (responseDA.getData())
		{
			if ((E_DEALLOCATE_CHANNEL_RESP == responseDA.getOpCode())
					&& (channel_id == responseDA.getChannelID()))
			{
				UInt32 data = 0;
				memcpy(&data, responseDA.getData(), sizeof(UInt32));
				responseDA.resetAll();
				result = ConnectivityAgentError(static_cast<ConnectivityAgentError::AgentErrorCodes>(data));
			} 
			else
			{
				LOG4CPLUS_ERROR(logger,
						"CConnectivityAgentProxy::deallocateChannel() => ERROR: wrong response type("
								+ convertIntegerToString(responseDA.getOpCode())
								+ ") from Agent  !!! ");
				result.setErrorCode(ConnectivityAgentError::ERROR_REQUEST_FAILED);
			}
		} 
		else
		{
			LOG4CPLUS_INFO(logger, "CConnectivityAgentProxy::deallocateChannel() => "
					"channel already deleted form other side");
			result = ConnectivityAgentError::NoError();
		}

	}

	if (!result.isNoError())
	{
		// something gone wrong, we need to restore information in registry
		mRegistryMutex.lockWrite();

		if (mChannelRegistry.find(channel_id) != mChannelRegistry.end())
		{
			LOG4CPLUS_WARN(logger, "CConnectivityAgentProxy::deallocateChannel: "
					"something gone wrong and I'm unable to restore channel info - "
					"there is a channel in registry with such id");
			result.setErrorCode(ConnectivityAgentError::ERROR_NOT_FOUND);
		} 
		else
		{
			mChannelRegistry[channel_id] = info;
			info.mChannelBuffer.forgetData();
			LOG4CPLUS_INFO(logger,
					"CConnectivityAgentProxy::deallocateChannel:"
							"unable to delete channel " + convertIntegerToString(channel_id)
							+ ", so channel info is restored");
		}

		mRegistryMutex.unlockWrite();
	}

	mRegistryMutex.lockWrite();
	mChannelOnDeallocSet.erase(channel_id);
	mRegistryMutex.unlockWrite();

	return result;
}

void CConnectivityAgentProxy::OnRequest(iviLink::Ipc::MsgID id, UInt8 const* pPayload,
		UInt32 payloadSize, UInt8* const pResponseBuffer, UInt32& bufferSize,
		iviLink::Ipc::DirectionID dirId)
{
	LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
	bufferSize = 0;
}


void CConnectivityAgentProxy::OnAsyncRequest(iviLink::Ipc::MsgID id, UInt8 const* pPayload,
		UInt32 payloadSize, iviLink::Ipc::DirectionID dirId)
{
	LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
	LOG4CPLUS_INFO(logger,
			"CConnectivityAgentProxy::OnAsyncRequest(): size " + convertIntegerToString(payloadSize));

	CDataAccessor responceDA(pPayload, payloadSize);
	const UInt32 channel_id = responceDA.getChannelID();
	const tOpCode type = static_cast<tOpCode>(responceDA.getOpCode());
	responceDA.printContent();

	switch (type)
	{
		case E_ALLOCATE_CHANNEL_RESP:
		{
			receiveAllocateChannelResponse(responceDA);
			break;
		}
		case E_DEALLOCATE_CHANNEL_RESP:
		{
			LOG4CPLUS_INFO(logger, "eDeallocateChannelResp");
			mDeallocateRequestResultCond.lock();
			mDeallocateRequestResultMap[channel_id] = responceDA;
			LOG4CPLUS_INFO(logger, "broadcasting about channel " + convertIntegerToString(channel_id));
			mDeallocateRequestResultCond.broadcast();
			mDeallocateRequestResultCond.unlock();
			break;
		}
		case E_RECEIVE_DATA_NTF:
		{
			receiveDataNotification(responceDA);
			break;
		}
		case E_DEALLOCATE_CHANNEL_NTF:
		{
			LOG4CPLUS_INFO(logger, "eDeallocateChannelNtf");
			channelDeletedNotification(responceDA);
			break;
		}
		case E_CONNECTION_LOST_NTF:
		{
			OnDisconnected();
			break;
		}
		default:
		{
			LOG4CPLUS_WARN(logger, "CConnectivityAgentProxy::OnRequest() => UNKNOWN Response!");
			break;
		}
	}
}

ConnectivityAgentError CConnectivityAgentProxy::receiveData(const UInt32 channel_id, UInt8* data,
		UInt32 & received_size, const UInt32 max_size)
{
	LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
	LOG4CPLUS_INFO(logger,
			"CConnectivityAgentProxy::receiveData() => channel "
					+ convertIntegerToString(channel_id));

	ConnectivityAgentError result;

	if (data != NULL)
	{
		mRegistryMutex.lockRead();
		tChannelsRegistryMap::iterator iter = mChannelRegistry.find(channel_id);

		if (iter != mChannelRegistry.end())
		{
			Buffer *buf = &(iter->second.mChannelBuffer);
			UInt32 filled_size = buf->getFilledSize();
			if (filled_size == 0)
			{
				LOG4CPLUS_INFO(logger,
						"CConnectivityAgentProxy::receiveData() => No data for channel "
								+ convertIntegerToString(channel_id) + "!");
				result.setErrorCode(ConnectivityAgentError::ERROR_NOT_FOUND);
			} else
			{
				if (filled_size > max_size)
				{
					memcpy(data, buf->getData(), max_size);
					memmove(buf->getData(), buf->getData() + max_size, filled_size - max_size);
					buf->getFilledSize() = filled_size - max_size;
					received_size = max_size;
				} else
				{
					memcpy(data, buf->getData(), filled_size);
					memset(buf->getData(), 0, filled_size);
					buf->getFilledSize() = 0;
					received_size = filled_size;
				}
				result.setNoError();
			}
		} else
		{
			result.setErrorCode(ConnectivityAgentError::ERROR_NOT_FOUND);
		}
		mRegistryMutex.unlockRead();
	}
	return result;
}

void CConnectivityAgentProxy::receiveAllocateChannelResponse(CDataAccessor & accessor)
{
	UInt32 channel_id = accessor.getChannelID();
	ConnectivityAgentError error(static_cast<ConnectivityAgentError::AgentErrorCodes>(accessor.getErrorCode()));

	LOG4CPLUS_TRACE_METHOD(logger,
			"CConnectivityAgentProxy::receiveAllocateChannelResponse"
					" channel_id = " + convertIntegerToString(channel_id) + " err = "
					+ convertIntegerToString((int) error.getCode()));

	mAllocateRequestCond.lock();
	{
		tAllocateRequestMap::iterator it = mAllocateRequestMap.find(channel_id);
		if (it == mAllocateRequestMap.end())
		{
			LOG4CPLUS_WARN(logger,
					"CConnectivityAgentProxy::receiveAllocateChannelResponse() => channel is not requested");
		} else
		{
			AllocateRequestInfo & info = it->second;
			info.mRequestDone = true;
			info.mResult = static_cast<ConnectivityAgentError::AgentErrorCodes>(error.getCode());

			if (error.isNoError())
			{
				mRegistryMutex.lockWrite();
				mChannelRegistry[channel_id].mType = info.mType;
				mChannelRegistry[channel_id].mpClient = info.mpClient;
				mChannelRegistry[channel_id].mChannelBuffer.reserveSize(MAX_SIZE);
				mRegistryMutex.unlockWrite();
			}

			mAllocateRequestCond.broadcast();
		}
	}
	mAllocateRequestCond.unlock();
}

void CConnectivityAgentProxy::receiveDataNotification(CDataAccessor & accessor)
{
	LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
	UInt32 channel_id = accessor.getChannelID();

	UInt32 data_size = accessor.getDataSize();

	LOG4CPLUS_INFO(logger,
			"CConnectivityAgentProxy::receiveDataNotification() => channel "
					+ convertIntegerToString(channel_id) + ", data_size "
					+ convertIntegerToString(data_size));

	mRegistryMutex.lockRead();
	tChannelsRegistryMap::iterator iter = mChannelRegistry.find(channel_id);
	if (iter != mChannelRegistry.end())
	{
		Buffer *buf = &(iter->second.mChannelBuffer);
		UInt32 free_size = buf->getAllocatedSize() - buf->getFilledSize();

		if (free_size < data_size)
		{

			mCallbackListMutex.lock();
			CCallbackWrapper* pCallback = new CBufferOverflowCallback(iter->second.mpClient,
					channel_id);
			mCallbackList.push_back(pCallback);
			mCallbackListMutex.unlock();
			mCallbackSema.signal();
			LOG4CPLUS_ERROR(logger,
					"CConnectivityAgentProxy::receiveDataNotification() => overflow!");
			//copy or not?
		} else
		{
			buf->appendData(accessor.getData(), data_size);
			free_size -= data_size;
			mCallbackListMutex.lock();
			CCallbackWrapper* pCallback = new CDataReceivedCallback(iter->second.mpClient,
					channel_id, data_size);
			mCallbackList.push_back(pCallback);
			mCallbackListMutex.unlock();
			mCallbackSema.signal();
		}
	} else
	{
		LOG4CPLUS_ERROR(logger,
				"CConnectivityAgentProxy::receiveDataNotification() => unknown channel!");
	}
	mRegistryMutex.unlockRead();
}

void CConnectivityAgentProxy::channelDeletedNotification(CDataAccessor & accessor)
{
	LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
	UInt32 channel_id = accessor.getChannelID();

	mRegistryMutex.lockWrite();
	{
		tChannelsRegistryMap::iterator iter = mChannelRegistry.find(channel_id);
		if (iter == mChannelRegistry.end())
		{
			LOG4CPLUS_INFO(logger,
					"channel " + convertIntegerToString(channel_id) + " is not found");
			if (mChannelOnDeallocSet.find(channel_id) != mChannelOnDeallocSet.end())
			{
				LOG4CPLUS_INFO(logger,
						"channel is being deallocated from this side, signaling about it");
				mDeallocateRequestResultCond.lock();

				mDeallocateRequestResultMap[channel_id] = CDataAccessor();

				mDeallocateRequestResultCond.broadcast();
				mDeallocateRequestResultCond.unlock();
			}
		} else
		{
			LOG4CPLUS_INFO(logger, "channel " + convertIntegerToString(channel_id) + " found");
			mCallbackListMutex.lock();
			{
				CCallbackWrapper* pCallback = new CChannelDeletedCallback(iter->second.mpClient,
						channel_id);
				mCallbackList.push_back(pCallback);
				mCallbackSema.signal();
			}
			mCallbackListMutex.unlock();

			mChannelRegistry.erase(channel_id);
		}

	}
	mRegistryMutex.unlockWrite();
}

ConnectivityAgentError CConnectivityAgentProxy::sendData(const UInt32 channel_id, UInt8 const* data,
		const UInt32 size)
{
	LOG4CPLUS_INFO(logger,
			"CConnectivityAgentProxy::sendData() => channel " + convertIntegerToString(channel_id)
					+ ", size " + convertIntegerToString(size));
	ConnectivityAgentError result;

	const int MAX_BUF_SIZE = 0xFFFF;

	if ((data != NULL) && ((Int32) size > 0) && (size < MAX_BUF_SIZE))
	{
		mRegistryMutex.lockRead();
		bool found = (mChannelRegistry.find(channel_id) != mChannelRegistry.end());
		mRegistryMutex.unlockRead();

		if (found)
		{
			CDataAccessor requestDA;
			requestDA.setChannelID(channel_id);
			requestDA.setOpCode(E_SEND_DATA);
			requestDA.setData(data, size);
			UInt8 buf[MAX_BUF_SIZE];
			requestDA.copyToRawArray(buf);
			assert(requestDA.getObjectSize() <= MAX_BUF_SIZE);
			if (requestDA.getObjectSize() > MAX_BUF_SIZE)
			{
				LOG4CPLUS_ERROR(logger,
						"CConnectivityAgentProxy::sendData() => ERROR:  \
                     requestDA.getObjectSize() > max buf size");
				result.setErrorCode(ConnectivityAgentError::ERROR_INCONSISTENT_DATA);
				return result;
			}

			UInt32 respSize = 0;
			BaseError err = mpIpc->request(mMsgIdGen.next(), buf, requestDA.getObjectSize(), NULL,
					respSize);
			if (err.isNoError())
			{
				result.setNoError();
			} else
			{
				LOG4CPLUS_WARN(logger, static_cast<std::string>(err));
				result.setErrorCode(ConnectivityAgentError::ERROR_REQUEST_FAILED);
			}
		} else
		{
			LOG4CPLUS_ERROR(logger,
					"CConnectivityAgentProxy::sendData() => ERROR: channel "
							+ convertIntegerToString(channel_id) + " is not opened yet!");
			result.setErrorCode(ConnectivityAgentError::ERROR_NOT_FOUND);
		}
	} 
	else
	{
		result.setErrorCode(ConnectivityAgentError::ERROR_INCONSISTENT_DATA);
	}

	return result;
}

ConnectivityAgentError CConnectivityAgentProxy::getFreeSize(const UInt32 channel_id, UInt32& free_size) const
{
	LOG4CPLUS_TRACE_METHOD(logger,
			"CConnectivityAgentProxy::getFreeSize " + std::string("channel_id")
					+ convertIntegerToString(channel_id));

	ConnectivityAgentError result;

	mRegistryMutex.lockRead();
	{
		tChannelsRegistryMap::const_iterator iter = mChannelRegistry.find(channel_id);
		if (iter != mChannelRegistry.end())
		{
			Buffer const * const buf = &(iter->second.mChannelBuffer);
			free_size = buf->getAllocatedSize() - buf->getFilledSize();
			result.setNoError();
		} else
		{
			result.setErrorCode(ConnectivityAgentError::ERROR_NOT_FOUND);
		}
	}
	mRegistryMutex.unlockRead();

	return result;
}

ConnectivityAgentError CConnectivityAgentProxy::getConnectionInformation(ConnectionInformation* data)
{
	LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
	ConnectivityAgentError result;

	CDataAccessor requestDA;
	requestDA.setOpCode(E_GET_CONNECTION_ADDR);
	UInt8* buf = new UInt8[requestDA.getObjectSize()];
	requestDA.copyToRawArray(buf);

	UInt32 respSize = 4084; // max size that can pass through ipc
	UInt8 resp[respSize];

	BaseError err = mpIpc->request(mMsgIdGen.next(), buf, requestDA.getObjectSize(), resp, respSize);
	if (err.isNoError())
	{
		if (respSize)
		{
			CDataAccessor response(resp, respSize);
			size_t res_size = response.getDataSize();
			if(response.getErrorCode() == ConnectivityAgentError::ERROR_NOT_FOUND)
			{
				result.setErrorCode((ConnectivityAgentError::AgentErrorCodes)response.getErrorCode());
				LOG4CPLUS_INFO(logger, "Connection ADDR: LOCAL = " + data->getLocalAddress());
				LOG4CPLUS_INFO(logger, "Connection ADDR: REMOTE = " + data->getRemoteAddress());
				return result;
			}
			if (res_size > 0)
			{
				data->deserialize(response.getData());
				result.setNoError();
				LOG4CPLUS_INFO(logger, "Connection ADDR: LOCAL = " + data->getLocalAddress());
				LOG4CPLUS_INFO(logger, "Connection ADDR: REMOTE = " + data->getRemoteAddress());
			}
			else
			{
				result.setErrorCode(ConnectivityAgentError::ERROR_REQUEST_FAILED);
			}
		}
	} 
	else
	{
		LOG4CPLUS_WARN(logger, static_cast<std::string>(err));
		result.setErrorCode(ConnectivityAgentError::ERROR_REQUEST_FAILED);
	}

	delete[] buf;

	return result;
}

void CConnectivityAgentProxy::deallocateChannelForcibly(UInt32 channelID)
{
	LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
	LOG4CPLUS_INFO(logger,
			"CConnectivityAgentProxy::deallocateChannelForcibly(chID = "
					+ convertIntegerToString(channelID) + ")");

	mRegistryMutex.lockWrite();
	mChannelOnDeallocSet.insert(channelID);
	mRegistryMutex.unlockWrite();

	CDataAccessor requestDA;
	requestDA.setChannelID(channelID);
	requestDA.setOpCode(E_DEALLOCATE_CHANNEL_WATCHDOG);
	UInt8* buf = new UInt8[requestDA.getObjectSize()];
	requestDA.copyToRawArray(buf);

	// response will be sent later as a separate request
	UInt32 respSize = 4084; // max size that can pass through ipc
	UInt8 resp[respSize];
	BaseError err = mpIpc->request(mMsgIdGen.next(), buf, requestDA.getObjectSize(), resp, respSize);
	delete[] buf;

	if (!err.isNoError())
	{
		LOG4CPLUS_WARN(logger, static_cast<std::string>(err));
		return;
	} 
	else
	{

		CDataAccessor responseInitial(resp, respSize);
		if (responseInitial.getErrorCode() == ConnectivityAgentError::ERROR_NOT_FOUND)
		{
			LOG4CPLUS_INFO(logger,
				"CConnectivityAgentProxy::deallocateChannelForcibly(chID = "
					+ convertIntegerToString(channelID) + ") channel already deleted");

			mRegistryMutex.lockWrite();
			mChannelOnDeallocSet.erase(channelID);
			mRegistryMutex.unlockWrite();
			return;
		}
		
		LOG4CPLUS_INFO(logger,
			"CConnectivityAgentProxy::deallocateChannelForcibly(chID = "
					+ convertIntegerToString(channelID) + ") channel was not deleted, waiting for confirmation");
		mDeallocateRequestResultCond.lock();
		{
			LOG4CPLUS_INFO(logger,
					"CConnectivityAgentProxy::deallocateChannel waiting for mLastRequestResultDA");
			while (mDeallocateRequestResultMap.find(channelID) == mDeallocateRequestResultMap.end())
			{
				LOG4CPLUS_INFO(logger, "before wait");
				mDeallocateRequestResultCond.wait();
				LOG4CPLUS_INFO(logger, "after wait");
			}

			mDeallocateRequestResultMap.erase(channelID);
		}
		mDeallocateRequestResultCond.unlock();
	}

	mRegistryMutex.lockWrite();
	mChannelOnDeallocSet.erase(channelID);
	mRegistryMutex.unlockWrite();
}

void CConnectivityAgentProxy::OnDisconnected()
{
	LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

	mRegistryMutex.lockWrite();
	tChannelsRegistryMap::iterator iter = mChannelRegistry.begin();
	mCallbackListMutex.lock();
	while (iter != mChannelRegistry.end())
	{
		CCallbackWrapper* pCallback = new CConnectionLostCallback(iter->second.mpClient);
		mCallbackList.push_back(pCallback);
		mCallbackSema.signal();
	}
	mCallbackListMutex.unlock();
	mRegistryMutex.unlockWrite();
}

void CConnectivityAgentProxy::threadFunc()
{
	while (false == getStopFlag())
	{
		mCallbackSema.wait();
		mCallbackListMutex.lock();
		for (std::list<CCallbackWrapper*>::iterator iter = mCallbackList.begin();
				iter != mCallbackList.end(); ++iter)
		{

			LOG4CPLUS_INFO(logger, "CConnectivityAgentProxy:: callbacks execution");
			static_cast<CCallbackWrapper*>(*iter)->execute();
			delete *iter;
		}
		mCallbackList.clear();
		mCallbackListMutex.unlock();
	}
}
