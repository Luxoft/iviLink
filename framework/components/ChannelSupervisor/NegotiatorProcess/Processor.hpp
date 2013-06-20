/**
 * Copyright placeholder
 */
/**
 * @ingroup             Channel Supervisor
 * @author              Elena Bratanova <ebratanova@luxoft.com>
 * @date                23.04.2013
 *
 * Common interface of all entities responsible for processing 
 * messages from clients and the remote negotiator.
 */

#ifndef __NEGOTIATOR__PROCESSOR__
#define __NEGOTIATOR__PROCESSOR__

#include "Notification.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "NegotiatorTube.hpp"
#include "ResponseContainer.hpp"

namespace iviLink
{
namespace ChannelSupervisor
{
class IResponseSender
{
public:
    virtual void sendResponse(ResponseContainer) = 0;
    virtual ~IResponseSender() {}
};

class Processor
{
public:
    Processor();
    /**
     * Invoked when the NegotiatorTube receives a notification
     * with matching NotificationType. 
     * This base implementation just inits mTube, mRemoteClientID
     * and mRemoteRequestID
     */
    virtual void onRemoteNotification(Messages::Notification*, NegotiatorTube*);
    /**
     * Invoked when the NegotiatorIPCHandler receives a request
     * with suitable RequestType.
     * This base implementation just inits mTube, mContainer, mResponseSender
     * mLocalClientID and mLocalRequestID
     */
    virtual void onLocalRequest(Messages::Request*, NegotiatorTube*, IResponseSender*,
                    ResponseContainer);
    /**
     * @return true when all needed remote messages have been sent and received,
     * and a response to the client has been sent
     */
    virtual bool isFinished() const = 0;
    /**
     * @return tag of the channel being processed.
     */
    virtual std::string getTag() const;
    virtual UInt32 getChannelID() const;
    virtual Messages::Notification::NotificationType getNotificationType() const;
    virtual Messages::Request::RequestType getRequestType() const;
    virtual Int32 getLocalClientID() const;
    virtual Int32 getRemoteClientID() const;
    virtual UInt32 getLocalRequestID() const;
    virtual UInt32 getRemoteRequestID() const;
    virtual ~Processor() {}
protected:
    void sendResponse(Messages::Response::ResponseType, Messages::ErrorCode eCode = Messages::ERRORCODE_SUCCESS,
        const char * errStr = "Success");
    void sendNotification(Messages::Notification::NotificationType);
protected:
    Messages::Request::RequestType mRequestType;
    Messages::Notification::NotificationType mNotificationType;
    IResponseSender * mResponseSender;
    NegotiatorTube * mTube;
    ResponseContainer mContainer;
    UInt32 mChannelID;
    std::string mTag;

    Int32 mLocalClientID;
    Int32 mLocalRequestID;
    Int32 mRemoteClientID;
    Int32 mRemoteRequestID;

private:
    static Logger mLogger;
};

}
}

#endif // __NEGOTIATOR__PROCESSOR__
