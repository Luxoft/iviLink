#ifndef TCP_FINDER_COMMON
#define TCP_FINDER_COMMON

/// Text used in broadcast messages for basic verification
#define BRD_MESSAGE_TEXT "IVILINK?"
#define FINDER_SLEEP_BETWEEN_RETRIES_MS 300

namespace iviLink
{
namespace ConnectivityAgent
{
namespace HAL
{
    enum eMessageResult {
        eSERVER,
        eCLIENT,
        eERROR
    };
    
    /// Type for broadcast message data
    struct tBrdMessage
    {
        char handshake[sizeof(BRD_MESSAGE_TEXT)];   /// used to perform handshake
        UInt8 random;                               /// used to hold random number
    } __attribute__((__packed__));
}
}
}

#endif //TCP_FINDER_COMMON