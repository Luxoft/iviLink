
/**
 *
 */

/**
 * @ingroup iOS Port
 * @author Bratanova Elena <ebratanova@luxoft.com>
 * @date 21.03.2013
 */


#ifdef __APPLE__
#include <cassert>
#include <unistd.h>
#include "IosIpcSocket.hpp"

#define DEFAULT_DIR 1

namespace
{
    std::string genLogTag(const iviLink::Ipc::Address& addr, bool listen)
    {
        return "iviLink::IPC::iOS::Socket::" + addr + "::" + (listen? "server" : "client");
    }
}

using namespace iviLink::Ipc;

CIpcSocket::CIpcSocket(CIpc& ipc, bool listen)
: CThread("CIpcReceiver"), mIpc(ipc), mLogger(Logger::getInstance(genLogTag(ipc.getAddress(), listen)))
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    mIsThreadStarted = false;
    mListenFlag = listen;
    mContListenFlag = false;
    mLatestDirId = 0;
    mDirectionID = -1;
    pipe(mPipeDescriptors);
    mPipeReadDescriptor = mPipeDescriptors[0];
    mPipeWriter = tPipeWriterPtr(new PipeWriter(mPipeDescriptors[1]));
    mProcessingPool = dispatch_queue_create(genLogTag(mIpc.getAddress(), mListenFlag).c_str(), DISPATCH_QUEUE_CONCURRENT);
};

CIpcSocket::~CIpcSocket()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    CThread::join();
}

void CIpcSocket::stopOperations()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);\
    dispatch_release(mProcessingPool);
    CThread::stop();
    close(mPipeDescriptors[0]);
    close(mPipeDescriptors[1]);
    if (mListenFlag)
    {
        IpcSocketDispatcher::getInstance()->unregisterListener(this);
        mListenSema.signal(); // to stop blocking listen if it is in progress
    }
    IpcSocketDispatcher::getInstance()->closeConnection(this);
}

bool CIpcSocket::isConnected() const
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    return !mConnections.empty();
}

bool CIpcSocket::checkThread() const
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    return !isThisThread();
}

void CIpcSocket::threadFunc()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    UInt32 messageSize;
    while (!getStopFlag())
    {
        if (!readDataToReadBuffer(sizeof(UInt32)))
        {
            LOG4CPLUS_WARN(mLogger, "could not read size of packet");
            continue;
        }
        messageSize = 0;
        memcpy(&messageSize, mReadBuffer, sizeof(UInt32));
        LOG4CPLUS_INFO(mLogger, "expected size: " + convertIntegerToString(messageSize));
        if (!readDataToReadBuffer(messageSize))
        {
            LOG4CPLUS_WARN(mLogger, "could not read packet");
            continue;
        }
        IpcMessage message = IpcMessage::fromBuffer(mReadBuffer);
        LOG4CPLUS_INFO(mLogger, "Got message:");
        message.printInfo();
        assert (messageSize >= 16);
        dispatch_async(mProcessingPool, ^{
            handleIncomingMessage(message);
        });
    }
    MutexLocker lock(mIsThreadStartedMutex);
    mIsThreadStarted = false;
}

void CIpcSocket::handleIncomingMessage(IpcMessage message)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    switch (message.mType)
    {
        case TRIGGER:
            mIpc.incomingAsyncRequest(message.mId, message.mPayload, message.mPayloadSize, message.mFrom);
            break;
        case REQUEST:
            mIpc.incomingRequest(message.mId, message.mPayload, message.mPayloadSize, message.mFrom);
            break;
        case RESPONSE:
        {
            CIpc::RequestData * reqData = NULL;
            if (mIpc.mRequestDataContainer.lockData(message.mId, reqData))
            {
                reqData->bufferSize = message.mPayloadSize;
                memcpy(reqData->pResponseBuffer, message.mPayload, message.mPayloadSize);
                mIpc.mRequestDataContainer.unlockData(message.mId);
                reqData->recvSem->signal();
            }
            else
            {
                LOG4CPLUS_ERROR(mLogger, "Could not lock data!");
                assert(false);
            }
        }
            break;
        default:
            assert(false);
    }
}

BaseError CIpcSocket::connect()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    if (mListenFlag)
    {
        mDirectionID = DEFAULT_DIR;
        // TODO revise connection logic if this assertion is ever triggered
        assert(IpcSocketDispatcher::getInstance()->registerListener(this));
        if (!mContListenFlag)
        {
            mListenSema.wait();
        }
    }
    else
    {
        tPipeWriterPtr writer = IpcSocketDispatcher::getInstance()->connectToServer(this, mDirectionID);
        if (writer.get() != NULL)
        {
            startThread();
            mIpc.onConnectSuccess(addConnection(writer));
        }
        else
        {
            assert (false);
            return CIpcError(CIpcError::ERROR_OTHER);
        }
    }
    return BaseError::NoError();
}

BaseError CIpcSocket::setContinuousListen()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    mContListenFlag = true;
    return BaseError::NoError();
}

BaseError CIpcSocket::stopContinuousListen()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    mContListenFlag = false;
    IpcSocketDispatcher::getInstance()->unregisterListener(this);
    return BaseError::NoError();
}

BaseError CIpcSocket::send(MsgID id, Type messageType,
                           UInt8 const* pPayload, UInt32 payloadSize,
                           DirectionID const * const pDirId)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    DirectionID dirId = (pDirId ? *pDirId : DEFAULT_DIR);
    MutexLocker lock(mConnectionsMutex);
    tConnectionsMap::iterator iter = mConnections.find(dirId);
    if (iter != mConnections.end() && (*iter).second.get() != NULL)
    {
        IpcMessage message(mDirectionID, id, messageType, pPayload, payloadSize);
        LOG4CPLUS_INFO(mLogger, "Sending message: ");
        message.printInfo();
        LOG4CPLUS_INFO(mLogger, "Sending message: before lock");
        MutexLocker lock(mWriteBufferMutex);
        LOG4CPLUS_INFO(mLogger, "Sending message: after lock");
        int sizeToSend = message.writeToBuffer(mWriteBuffer);
        bool result = mConnections[dirId].get()->writeToPipe(mWriteBuffer, sizeToSend);
        if (!result)
        {
            LOG4CPLUS_ERROR(mLogger, "Broken pipe!");
            invalidateConnectionUnsafe(dirId);
            return CIpcError(CIpcError::ERROR_CONNECTION_LOST);
        }
        else
        {
            return BaseError::NoError();
        }
    }
    else
    {
        LOG4CPLUS_ERROR(mLogger, "Cannot find such direction: " + convertIntegerToString(dirId));
        return CIpcError(CIpcError::ERROR_CONNECTION_LOST);
    }
}

DirectionID CIpcSocket::onClientConnected(IDispatcherClient * client)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    mDirectionID = DEFAULT_DIR;
    startThread();
    if (!mContListenFlag)
    {
        mListenSema.signal();
    }
    DirectionID res = addConnection(client->getPipeWriter());
    mIpc.onListenSuccess(res);
    return res;
}

void CIpcSocket::onClientDisconnectedFromDispatcher(IDispatcherClient* client)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    int direction = -1;
    mConnectionsMutex.lock();
    tConnectionsMap::iterator iter = mConnections.begin();
    for (; iter != mConnections.end(); iter++)
    {
        if ((*iter).second == client->getPipeWriter())
        {
            direction = (*iter).first;
            mConnections.erase(iter);
            break;
        }
    }
    mConnectionsMutex.unlock();
    if (direction != -1)
    {
        mIpc.onConnectionLost(direction);
    }
}

Address CIpcSocket::getAddress() const
{
    return mIpc.getAddress();
}

tPipeWriterPtr CIpcSocket::getPipeWriter() const
{
    return mPipeWriter;
}

int CIpcSocket::addConnection(tPipeWriterPtr writer)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    int newestDirId = -1;
    MutexLocker lock(mConnectionsMutex);
    mLatestDirId++;
    newestDirId = mLatestDirId;
    mConnections[mLatestDirId] = writer;
    return newestDirId;
}

void CIpcSocket::invalidateConnectionUnsafe(DirectionID id)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    tConnectionsMap::iterator iter = mConnections.find(id);
    if (iter != mConnections.end())
    {
        mIpc.onConnectionLost((*iter).first);
        mConnections.erase(iter);
    }
}

bool CIpcSocket::readDataToReadBuffer(int dataSize)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    memset(mReadBuffer, 0, IOS_IPC_BUF_SIZE);
    int totalRead = 0;
    while (totalRead != dataSize)
    {
        int currentlyRead = read(mPipeReadDescriptor, mReadBuffer + totalRead, dataSize - totalRead);
        if (currentlyRead == 0) // closed pipe
        {
            LOG4CPLUS_WARN(mLogger, "closed pipe! totalRead: " + convertIntegerToString(totalRead));
            LOG4CPLUS_WARN(mLogger, "closed pipe! dataSize: " + convertIntegerToString(dataSize));
            return false;
        }
        totalRead += currentlyRead;
    }
    return true;
}

void CIpcSocket::startThread()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    MutexLocker lock(mIsThreadStartedMutex);
    if (!mIsThreadStarted)
    {
        mIsThreadStarted = true;
        CThread::start();
    }
}

#endif