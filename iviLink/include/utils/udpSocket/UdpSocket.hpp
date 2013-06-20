#ifndef UDPSOCK_HPP
#define UDPSOCK_HPP

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>

#include "Logger.hpp"
#include "Types.hpp"

class UdpSocket
{

    enum Status
    {
        SENDER,
        RECEIVER,
        NA
    };
private:
    static Logger logger;
    UInt32 mPortNumber;
    int mSocketId;
    struct sockaddr_in mAddr;
    Status mStatus;
public:
    UdpSocket(const UInt32 portNum);

    /*
    * init client side
    */
    void initSender();

    /*
    * init server side
    */
    void initReceiver();

    virtual ~UdpSocket();

    /*
    * send message 
    */
    void sendMessage(const UInt8) const;

    /*
    * receive message
    */
    int receiveMessage(UInt8 &message) const;

    std::string printInfo() const;

    void closeSocket();
};

#endif /* UDPSOCKET_HPP */