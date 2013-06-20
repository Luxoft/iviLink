/**
 * Copyright placeholder
 */
/**
 * @ingroup             Channel Supervisor
 * @author              Elena Bratanova <ebratanova@luxoft.com>
 * @date                23.04.2013
 *
 * Processes deallocation notifications.
 */

#ifndef __DEALLOCATION_PROCESSOR_HPP
#define __DEALLOCATION_PROCESSOR_HPP

#include "Processor.hpp"

namespace iviLink
{
namespace ChannelSupervisor
{
class DeallocationProcessor: public Processor
{
public:
    DeallocationProcessor();
    virtual ~DeallocationProcessor();
    virtual void onLocalRequest(Messages::Request*, NegotiatorTube*, IResponseSender*,
                    ResponseContainer);
    virtual bool isFinished() const;
private:
    static Logger mLogger;
};

}
}

#endif //__DEALLOCATION_PROCESSOR_HPP
