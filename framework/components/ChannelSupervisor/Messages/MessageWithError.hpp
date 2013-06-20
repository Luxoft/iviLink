/**
 * Copyright placeholder
 */
/**
 * @ingroup             Channel Supervisor
 * @author              Elena Bratanova <ebratanova@luxoft.com>
 * @date                23.04.2013
 */

#ifndef MESSAGE_WITH_ERROR_HPP
#define MESSAGE_WITH_ERROR_HPP

#include <map>

#include "Message.hpp"
#include "ErrorCode.hpp"

namespace iviLink
{
namespace ChannelSupervisor
{
namespace Messages
{

class MessageWithError: public Message
{
public:
    MessageWithError(const char * tag, UInt32 chID);

    MessageWithError(pugi::xml_document* doc);

    virtual ~MessageWithError()
    {
    }

    void setError(ErrorCode errorCode, const char* errorString);
    void getError(ErrorCode& errorCode, const char*& errorString) const;

protected:
    void appendErrorNode(ErrorCode errorCode, const char* errorString);
protected:
    pugi::xml_node mErrorNode;
    ErrorCode mErrorCode;
    const char * mErrorStr;
};
}  // Messages
}  // ChannelSupervisor
}  // iviLink

#endif // REQUEST_HPP_
