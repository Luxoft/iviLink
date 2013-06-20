/**
 * Copyright placeholder
 */
/**
 * @ingroup             Channel Supervisor
 * @author              Elena Bratanova <ebratanova@luxoft.com>
 * @date                23.04.2013
 *
 * Interface of the Negotator Client that communicates with the Negotiator Process.
 */

#ifndef __I_NEGOTIATOR_CLIENT_HPP__
#define __I_NEGOTIATOR_CLIENT_HPP__

#include <string>

#include "Types.hpp"
#include "BaseError.hpp"

namespace iviLink
{

namespace ChannelSupervisor
{

class INegotiatorClient
{
public:
    virtual BaseError negotiateChannel(std::string const& tag, UInt32 & channelID) = 0;
    virtual BaseError updateMapWithCID(std::string const& tag, UInt32 const& channelID) = 0;
    virtual BaseError freeChannel(UInt32 const& channelID) = 0;
    virtual BaseError updateChannelInfo(std::string const& tag, UInt32 const& channelID) = 0;
protected:
    virtual ~INegotiatorClient()
    {
    }
};

}
}

#endif // __I_NEGOTIATOR_CLIENT_HPP__
