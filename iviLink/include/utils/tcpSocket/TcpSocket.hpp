#ifndef TCP_SOCKET_HPP
#define TCP_SOCKET_HPP

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>

#include "Logger.hpp"
#include "Types.hpp"

class TcpSocket
{
private:
   static Logger logger;
    UInt32 portNumber;
    int socketId;
    int listener;
    struct sockaddr_in addr;
    UInt8 buf;
public:
    TcpSocket(const UInt32 portNum);

    virtual ~TcpSocket();

    /*
    * init client side socket
    */
    void initClient();

    /*
    * init server side socket
    */
    void initServer();

    /*
    * accept socket
    */
    void acceptSocket();

    /*
    * send message @msg
    */
    void sendMessage(const UInt8 msg) const;

    /*
    * receive @message
    */
    UInt32 receiveMessage(UInt8 &message) const;

    int getSocketID() { return socketId; }

    /*
    * close socket
    */
    void closeSocket();
};

#endif //TCP_SOCKET_HPP
