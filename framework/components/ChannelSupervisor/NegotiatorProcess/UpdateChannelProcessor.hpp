/**
 * Copyright placeholder
 */
/**
 * @ingroup             Channel Supervisor
 * @author              Elena Bratanova <ebratanova@luxoft.com>
 * @date                23.04.2013
 *
 * Processes watchdog requests.
 */

#ifndef __UPDATE_CHANNEL_INFO_PROCESSOR_HPP__
#define __UPDATE_CHANNEL_INFO_PROCESSOR_HPP__

#include "Processor.hpp"

namespace iviLink
{
namespace ChannelSupervisor
{
class UpdateChannelProcessor: public Processor
{
public:
    UpdateChannelProcessor();
    virtual ~UpdateChannelProcessor();
    virtual void onRemoteNotification(Messages::Notification*, NegotiatorTube*);
    virtual void onLocalRequest(Messages::Request*, NegotiatorTube*, IResponseSender*,
                    ResponseContainer);
    virtual bool isFinished() const;
private:
    void updateMapAndSendResponse();
    static Logger mLogger;
};

}
}

#endif //__UPDATE_CHANNEL_INFO_PROCESSOR_HPP__
