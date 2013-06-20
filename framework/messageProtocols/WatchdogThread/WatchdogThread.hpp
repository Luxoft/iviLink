#ifndef WATCH_DOG_THREAD_HPP
#define WATCH_DOG_THREAD_HPP

#include "CThread.hpp"
#include "UdpSocket.hpp"
#include "Types.hpp"

#define SLEEP_TIMEOUT_MILLIS 3000
#define THREAD_NAME "WatchdogThread"
#define WATCHDOG_BASE_UDP_PORT 3600
#define DEFAULT_MESSAGE 1

class WatchdogThread: public CThread
{

    const UInt32 mUdpPort;
    const UInt32 mMessageToSend;
public:
    WatchdogThread(const UInt32 udpPortOffset, const UInt32 message = DEFAULT_MESSAGE)
    : CThread(THREAD_NAME),
      mUdpPort(WATCHDOG_BASE_UDP_PORT + udpPortOffset),
      mMessageToSend(message)
    {
    }
private:
     virtual void threadFunc()
     {    
        UdpSocket udpSocket(mUdpPort);
        udpSocket.initSender();
        while (true)
        {
            udpSocket.sendMessage((UInt8) mMessageToSend);
            sleep(SLEEP_TIMEOUT_MILLIS);
        }
     }

};

#endif //WATCH_DOG_THREAD_HPP