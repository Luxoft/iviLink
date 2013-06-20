/**
 * Copyright placeholder
 */
/**
 * @ingroup             Channel Supervisor
 * @author              Elena Bratanova <ebratanova@luxoft.com>
 * @date                23.04.2013
 *
 * Structure for data of IPC responses for clients.
 */

#ifndef __RESPONSE_CONTAINER_HPP
#define __RESPONSE_CONTAINER_HPP

#include "ipc_common.hpp"
#include "Response.hpp"

namespace iviLink
{
namespace ChannelSupervisor
{
struct ResponseContainer
{
    iviLink::Ipc::DirectionID mClientID;
    iviLink::Ipc::MsgID mResponseID;
    std::string mResponseSerialized;

    ResponseContainer()
    {
        mClientID = 0;
        mResponseID = 0;
        mResponseSerialized = std::string("");
    }

    ResponseContainer(iviLink::Ipc::DirectionID dirID, iviLink::Ipc::MsgID messageId,
        std::string response)
    {
        mClientID = dirID;
        mResponseID = messageId;
        mResponseSerialized = response;
    }

    ~ResponseContainer()
    {
    }

    ResponseContainer(const ResponseContainer& toCopy)
    {
        mClientID = toCopy.mClientID;
        mResponseID = toCopy.mResponseID;
        mResponseSerialized = toCopy.mResponseSerialized;
    }
};

}
}

#endif
