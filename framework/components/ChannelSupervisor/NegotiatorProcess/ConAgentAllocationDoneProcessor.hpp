/**
 * Copyright placeholder
 */
/**
 * @ingroup             Channel Supervisor
 * @author              Elena Bratanova <ebratanova@luxoft.com>
 * @date                23.04.2013
 *
 * Processes notifications that are sent after the channel has been negotiated and allocated
 * in the Connectivity Agent.
 */

#ifndef __CON_AGENT_ALLOCATION_DONE_PROCESSOR_HPP__
#define __CON_AGENT_ALLOCATION_DONE_PROCESSOR_HPP__

#include "Processor.hpp"

namespace iviLink
{
namespace ChannelSupervisor
{
class ConAgentAllocationDoneProcessor: public Processor
{
public:
    ConAgentAllocationDoneProcessor();
    virtual ~ConAgentAllocationDoneProcessor();
    virtual void onRemoteNotification(Messages::Notification*, NegotiatorTube*);
    virtual void onLocalRequest(Messages::Request*, NegotiatorTube*, IResponseSender*,
                    ResponseContainer);
    virtual bool isFinished() const;
private:
    bool mGotRemoteNotificationCaalloc;
    static Logger mLogger;
};

}
}
#endif // __CON_AGENT_ALLOCATION_DONE_PROCESSOR_HPP__
