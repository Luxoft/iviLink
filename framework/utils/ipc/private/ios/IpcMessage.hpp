/**
 *
 */

/**
 * @ingroup iOS Port
 * @author Bratanova Elena <ebratanova@luxoft.com>
 * @date 21.03.2013
 *
 * Formatter of IPC communication protocol: serializes and deserializes data.
 */

#ifdef __APPLE__

#ifndef __iviLinkCoreStack__IpcMessage__
#define __iviLinkCoreStack__IpcMessage__

#include <iostream>
#include "IDispatcherClient.hpp"
#include "ipc_common.hpp"
#include "Logger.hpp"

namespace iviLink
{
    namespace Ipc
    {
        /**
         * Formatter of IPC communication protocol: serializes and deserializes data.
         */
        class IpcMessage
        {
        public:
            /**
             * @param fromDirection sender of the IPC message (the receiver should know the sender as this direction)
             */
            IpcMessage(DirectionID fromDirection, MsgID id, Type messageType, UInt8 const*const payload, UInt32 payloadSize);
            IpcMessage(const IpcMessage &);
            /**
             * Fill an IpcMessage with data from buffer.
             */
            static IpcMessage fromBuffer(UInt8 buffer[MAX_SIZE]);
            /**
             * Writes IpcMessage contents to the provided buffer.
             * @return total bytes occupied in buffer
             */
            int writeToBuffer(UInt8 buffer[MAX_SIZE]) const;
            ~IpcMessage();
            
            void printInfo() const;
        public:
            DirectionID mFrom;
            MsgID mId;
            Type mType;
            UInt8 mPayload[IOS_IPC_BUF_SIZE];
            UInt32 mPayloadSize;
            UInt32 mSize;
        private:
            static Logger mLogger;
        };
    }
}

#endif /* defined(__iviLinkCoreStack__IpcMessage__) */

#endif
