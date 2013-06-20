#include "UdpSocket.hpp"
#include <strings.h>
#include <unistd.h>

Logger UdpSocket::logger = Logger::getInstance(LOG4CPLUS_TEXT("UdpSockets.lib"));

static const char* StatusNames[] =
{
    "SENDER",
    "RECEIVER",
    "NA"
};

UdpSocket::UdpSocket(const UInt32 portNum)
                    : mPortNumber(portNum),
                    mStatus(NA)
{
    LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__ + printInfo());
}

void UdpSocket::initSender()
{
    mStatus = SENDER;
    LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__ + printInfo());
    mSocketId = socket(AF_INET, SOCK_DGRAM, 0);
    while (mSocketId < 0)
    {
        LOG4CPLUS_ERROR(logger, "Socket initialize error " + printInfo());
        mSocketId = socket(AF_INET, SOCK_DGRAM, 0);    
    }
    fcntl(mSocketId, F_SETFL, O_NONBLOCK|FD_CLOEXEC);
    mAddr.sin_family = AF_INET;
    mAddr.sin_port = htons(mPortNumber);
    mAddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    LOG4CPLUS_INFO(logger, "UdpClient initialized " + printInfo());
}

void UdpSocket::initReceiver()
{
    mStatus = RECEIVER;
    LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__ + printInfo());
    mSocketId = socket(AF_INET, SOCK_DGRAM, 0);
    while (mSocketId < 0)
    {
        LOG4CPLUS_ERROR(logger, "Socket initialize error " + printInfo());
        mSocketId = socket(AF_INET, SOCK_DGRAM, 0);
    }
    fcntl(mSocketId, F_SETFL, O_NONBLOCK|FD_CLOEXEC);
    LOG4CPLUS_INFO(logger, "Udp listener initialized " + printInfo());
    mAddr.sin_family = AF_INET;
    mAddr.sin_port = htons(mPortNumber);
    mAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    while (bind(mSocketId, (struct sockaddr*) &mAddr, sizeof(mAddr)) < 0)
    {
        LOG4CPLUS_ERROR(logger, "Socket initialize error " + printInfo());
    }
    LOG4CPLUS_INFO(logger, "UDP socket binded " + printInfo());
}

UdpSocket::~UdpSocket() 
{ 
    LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__ + printInfo());
}

void UdpSocket::sendMessage(const UInt8 msg) const
{
    LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__ + printInfo());
    if (mStatus != SENDER)
    {
        LOG4CPLUS_WARN(logger, "Trying to use Receiver as a Sender!");
    }
    sendto(mSocketId, &msg, sizeof(msg), 0, (struct sockaddr *)&mAddr, sizeof(mAddr));
}

int UdpSocket::receiveMessage(UInt8 &message) const
{
    LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__ + printInfo());
    if (mStatus != RECEIVER)
    {
        LOG4CPLUS_WARN(logger, "Trying to use Sender as a Receiver!");
        return 0;
    }
    message = 0;
    const UInt32 messageSize = 1;
    UInt8 messageArray[messageSize];
    bzero(messageArray, messageSize);
    int bytesRead = recvfrom(mSocketId, messageArray, messageSize, 0, NULL, NULL);
    message = messageArray[0];
    LOG4CPLUS_INFO(logger, "Bytes read: " + (bytesRead > 0? convertIntegerToString(bytesRead) : "<0") 
        + " message: " + convertIntegerToString(message) + printInfo());
    return bytesRead;
}

void UdpSocket::closeSocket()
{
    LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
    close(mSocketId);
}

std::string UdpSocket::printInfo() const
{
    return " " + convertIntegerToString(mPortNumber) + ":" + StatusNames[mStatus];
}
