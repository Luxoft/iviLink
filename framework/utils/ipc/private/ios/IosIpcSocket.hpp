
/**
 *
 */

/**
 * @ingroup iOS Port
 * @author Bratanova Elena <ebratanova@luxoft.com>
 * @date 21.03.2013
 *
 * iOS-specific CIpcSocket implementation - used pipes for communication.
 */


#ifdef __APPLE__

#ifndef IOS_IPC_SOCKET
#define IOS_IPC_SOCKET

#include <dispatch/dispatch.h>

#include "CThread.hpp"
#include "Logger.hpp"
#include "Types.hpp"

#include "CIpcError.hpp"
#include "CIpc.hpp"
#include "ipc_common.hpp"
#include "IpcSocketDispatcher.hpp"
#include "IpcMessage.hpp"

namespace iviLink
{
    namespace Ipc
    {
        typedef std::map<DirectionID, tPipeWriterPtr> tConnectionsMap;
        
        class CIpcSocket : public CThread, public IDispatcherClient
        {
        private:
            Logger mLogger;
            
        public:
            CIpcSocket(CIpc& ipc, bool listen);
            
            ~CIpcSocket();
            
            void stopOperations();
            
            bool isConnected() const;
            
            bool checkThread() const;
            
            BaseError connect();
            
            BaseError setContinuousListen();
            
            BaseError stopContinuousListen();
            
            BaseError send(MsgID id, Type messageType,
                           UInt8 const* pPayload, UInt32 payloadSize,
                           DirectionID const * const pDirId);
            
            virtual DirectionID onClientConnected(IDispatcherClient * client);
            virtual void onClientDisconnectedFromDispatcher(IDispatcherClient* client);
            virtual Address getAddress() const;
            virtual tPipeWriterPtr getPipeWriter() const;
            
            virtual void threadFunc();
        private:
            int addConnection(tPipeWriterPtr);
            void invalidateConnectionUnsafe(DirectionID id);
            /**
             * @return false if pipe is closed and data cannot be read.
             */
            bool readDataToReadBuffer(int dataSize);
            
            void startThread();
            
            void handleIncomingMessage(IpcMessage message);
        private:
            CIpc& mIpc;
            bool mContListenFlag;
            bool mListenFlag;
            
            CMutex mIsThreadStartedMutex;
            bool mIsThreadStarted;
            
            CSignalSemaphore mListenSema;
            
            int mLatestDirId;
            
            /**
             * if it is a listening socket, mDirectionID is set to 1
             * otherwise its value is granted by the socket it connects to
             */
            DirectionID mDirectionID;
            
            int mPipeReadDescriptor;
            int mPipeDescriptors[2];
            /**
             * Shared pointer to the PipeWriter 
             * that can be used to send data to this socket.
             */
            tPipeWriterPtr mPipeWriter;
            tConnectionsMap mConnections;
            CMutex mConnectionsMutex;
            
            /**
             * Data from mPipeReadDescriptor is read into this buffer.
             */
            UInt8 mReadBuffer[IOS_IPC_BUF_SIZE];
            CMutex mWriteBufferMutex;
            /**
             * Data to send is written here prior to sending.
             */
            UInt8 mWriteBuffer[IOS_IPC_BUF_SIZE];
    
            // concurrent queue for processing data from socket
            dispatch_queue_t mProcessingPool;
        };
        
    }  // namespace Ipc
}  // namespace iviLink

#endif /* CIPCSOCKET_HPP_ */

#endif 
