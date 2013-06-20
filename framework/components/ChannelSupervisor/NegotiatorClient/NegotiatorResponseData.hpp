/**
 * Copyright placeholder
 */
/**
 * @ingroup             Channel Supervisor
 * @author              Elena Bratanova <ebratanova@luxoft.com>
 * @date                23.04.2013
 *
 * Structure for data of IPC responses from Negotiator Process.
 */

#ifndef __NEGOTIATOR_RESPONSE_DATA_HPP_
#define __NEGOTIATOR_RESPONSE_DATA_HPP_

#include "CSignalSemaphore.hpp"
#include "Response.hpp"

namespace iviLink
{

namespace ChannelSupervisor
{

struct NegotiatorResponseData
{
    CSignalSemaphore * mSema;
    Messages::Response * mResponse;

    NegotiatorResponseData()
    {
        mResponse = NULL;
        mSema = new CSignalSemaphore();
    }

    ~NegotiatorResponseData()
    {
        // mResponse should be deleted elsewhere
        if (mSema)
        {
            delete mSema;
        }
    }
};

}

}

#endif // __NEGOTIATOR_RESPONSE_DATA_HPP_
