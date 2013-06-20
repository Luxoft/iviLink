#ifndef MESSAGE_PROTOCOL_MESSAGE_HPP
#define MESSAGE_PROTOCOL_MESSAGE_HPP

struct MessageHeader
{
   UInt16 type;
   UInt32 size;
} __attribute__((__packed__));

struct Message
{
   MessageHeader header;
   UInt8 data[0];
} __attribute__((__packed__));


enum
{
    BUFFER_SIZE = 4096
};

class CMsgIdGen
{
public:
    CMsgIdGen()
    {
        mId = -1;
    }

    ~CMsgIdGen() {} 
        
    iviLink::Ipc::MsgID getNext() 
    {
        mId += 2;
        return mId;
    }

private:
    iviLink::Ipc::MsgID mId;
};

#endif //MESSAGE_PROTOCOL_MESSAGE_HPP