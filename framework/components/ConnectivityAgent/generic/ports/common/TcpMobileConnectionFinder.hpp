#ifndef TCP_MOBILE_CONNECTION_FINDER
#define TCP_MOBILE_CONNECTION_FINDER

#include <sys/socket.h>
#include <arpa/inet.h>

#include "CConnectionFinder.hpp"
#include "TcpFinderCommon.hpp"
#include "ConnectionInfo.hpp"

namespace iviLink
{
namespace ConnectivityAgent
{
namespace HAL
{
/**
 *  A simplified version of CTcpAutoConnectionFinder - does not use netlink sockets.
 */
class TcpMobileConnectionFinder : public CConnectionFinder
{
private:
    static Logger mLogger;

public:
    TcpMobileConnectionFinder(IFoundConnectionHandler& handler, EGenderType gender);
    virtual ~TcpMobileConnectionFinder();

    virtual void setAutoConnectionAllowed(bool);

protected:
    virtual void performSearch();

private:
    bool createClientCarrierAdapter(std::string const& addr);
    CCarrierAdapter*  createServerCarrierAdapter();

    void initSockaddrIn(sockaddr_in& addr, unsigned long s_addr);

    CTcpConnectionInfo createConnectionInfo(bool isServer, const std::string& host);
private:
    int mBroadcastSock;
    tBrdMessage mMessage;
    sockaddr_in mBroadcastAddr;
    UInt8 mRoundRandNumber;
    bool mAutoconnectionAllowed;
};

}
}
}

#endif //TCP_MOBILE_CONNECTION_FINDER
