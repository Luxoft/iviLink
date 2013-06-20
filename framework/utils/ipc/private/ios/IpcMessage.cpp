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

#include "IpcMessage.hpp"

namespace iviLink
{
    namespace Ipc
    {
        Logger IpcMessage::mLogger = Logger::getInstance("iviLink::IPC::iOS::IpcMessage");
        IpcMessage::IpcMessage(DirectionID fromDirection, MsgID id, Type messageType, UInt8 const*const payload, UInt32 payloadSize)
        : mFrom(fromDirection), mId(id), mType(messageType), mPayloadSize(payloadSize)
        {
            mSize = 0;
            mSize += sizeof(DirectionID);
            mSize += sizeof(MsgID);
            mSize += sizeof(Type);
            mSize += sizeof(mPayloadSize);
            mSize += mPayloadSize;
            assert(mSize <= IOS_IPC_BUF_SIZE);
            if (mPayloadSize != 0)
            {
                assert(payload != NULL);
                memset(mPayload, 0, sizeof(mPayload));
                memcpy(mPayload, payload, payloadSize);
            }
        }
        
        IpcMessage::IpcMessage(const IpcMessage& message)
        {
            mFrom = message.mFrom;
            mId = message.mId;
            mType = message.mType;
            mPayloadSize = message.mPayloadSize;
            if (mPayloadSize != 0)
            {
                memcpy(mPayload, message.mPayload, mPayloadSize);
            }
        }
        
        IpcMessage IpcMessage::fromBuffer(UInt8 buffer[IOS_IPC_BUF_SIZE])
        {
            int offset = 0;
            DirectionID dirId = 0;
            memcpy(&dirId, buffer + offset, sizeof(DirectionID));
            offset += sizeof(DirectionID);
            MsgID id = 0;
            memcpy(&id, buffer + offset, sizeof(MsgID));
            offset += sizeof(MsgID);
            UInt32 msgType = 0;
            memcpy(&msgType, buffer + offset, sizeof(UInt32));
            offset += sizeof(UInt32);
            UInt32 payloadSize = 0;
            memcpy(&payloadSize, buffer + offset, sizeof(UInt32));
            offset += sizeof(UInt32);
            UInt8* payload = buffer + offset;
            return IpcMessage(dirId, id, (Type)msgType, payload, payloadSize);
        }
        
        int IpcMessage::writeToBuffer(UInt8 buffer[IOS_IPC_BUF_SIZE]) const
        {
            memset(buffer, 0, IOS_IPC_BUF_SIZE);
            int offset = 0;
            memcpy(buffer, &mSize, sizeof(UInt32));
            offset += sizeof(UInt32);
            memcpy(buffer + offset, &mFrom, sizeof(DirectionID));
            offset += sizeof(DirectionID);
            memcpy(buffer + offset, &mId, sizeof(MsgID));
            offset += sizeof(MsgID);
            UInt32 type = mType;
            memcpy(buffer + offset, &type, sizeof(UInt32));
            offset += sizeof(UInt32);
            memcpy(buffer + offset, &mPayloadSize, sizeof(UInt32));
            offset += sizeof(UInt32);
            memcpy(buffer + offset, mPayload, mPayloadSize);
            return mSize + sizeof(UInt32);
        }
        
        IpcMessage::~IpcMessage()
        {
            
        }
        
        void IpcMessage::printInfo() const
        {
            std::string info = "mFrom: " + convertIntegerToString(mFrom);
            info += " ";
            info += "mId: " + convertIntegerToString(mId);
            info += " ";
            info += "mType: " + convertIntegerToString(mType);
            info += " ";
            info += "mPayloadSize: " + convertIntegerToString(mPayloadSize);
            info += " ";
            info += "mSize: " + convertIntegerToString(mSize);
            LOG4CPLUS_DEBUG(mLogger, "IpcMessage: " + info);
        }
    }
}

#endif