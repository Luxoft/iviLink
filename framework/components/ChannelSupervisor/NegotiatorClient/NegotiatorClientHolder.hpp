/**
 * Copyright placeholder
 */
/**
 * @ingroup             Channel Supervisor
 * @author              Elena Bratanova <ebratanova@luxoft.com>
 * @date                23.04.2013
 *
 * Holder of singleton instance of the Negotiator Client
 */

#ifndef __NEGOTIATOR_CLIENT_HOLDER_HPP
#define __NEGOTIATOR_CLIENT_HOLDER_HPP

#include "INegotiatorClient.hpp"

namespace iviLink
{
namespace ChannelSupervisor
{
class NegotiatorClientHolder
{
public:
    static INegotiatorClient * getInstance();
};

}
}

#endif // __NEGOTIATOR_CLIENT_HOLDER_HPP
