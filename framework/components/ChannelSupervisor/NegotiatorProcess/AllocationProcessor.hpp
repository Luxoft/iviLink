/**
 * Copyright placeholder
 */
/**
 * @ingroup             Channel Supervisor
 * @author              Elena Bratanova <ebratanova@luxoft.com>
 * @date                23.04.2013
 *
 * Processes notifications initial allocation messages.
 */

#ifndef __ALLOCATION_PROCESSOR_HPP__
#define __ALLOCATION_PROCESSOR_HPP__

#include "Processor.hpp"

namespace iviLink
{
namespace ChannelSupervisor
{
class AllocationProcessor: public Processor
{
public:
    AllocationProcessor(bool isMaster);
    virtual ~AllocationProcessor();
    virtual void onRemoteNotification(Messages::Notification*, NegotiatorTube*);
    virtual void onLocalRequest(Messages::Request*, NegotiatorTube*, IResponseSender*,
                    ResponseContainer);
    virtual bool isFinished() const;
private:
    bool mGotAllocateRequest;
    bool mIsMaster; 
    static Logger mLogger;
};

}
}

#endif //__ALLOCATION_PROCESSOR_HPP__
