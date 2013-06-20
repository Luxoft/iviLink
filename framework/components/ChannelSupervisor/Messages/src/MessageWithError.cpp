/**
 * Copyright placeholder
 */
/**
 * @ingroup             Channel Supervisor
 * @author              Elena Bratanova <ebratanova@luxoft.com>
 * @date                23.04.2013
 */
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <cassert>
#include <string>
#include "MessageWithError.hpp"
#include "NegotiatorConstants.hpp"
#include "XmlHelper.hpp"

static const char * XML_ERRROR_TAG = "error";
static const char * XML_ERROR_CODE_TAG = "code";

using namespace iviLink::ChannelSupervisor::Messages;

void MessageWithError::appendErrorNode(ErrorCode errorCode,
                const char* errorString)
{
    mErrorNode = mMessageDocMainNode.append_child(XML_ERRROR_TAG);
    mErrorNode.append_attribute(XML_ERROR_CODE_TAG).set_value(errorCode);
    mErrorNode.append_child(pugi::node_pcdata).set_value(errorString);
}

void MessageWithError::setError(ErrorCode errorCode, const char* errorString)
{
    mErrorCode = errorCode;
    mErrorStr = errorString;
    mErrorNode.attribute(XML_ERROR_CODE_TAG).set_value(mErrorCode);
    mErrorNode.first_child().set_value(mErrorStr);
}

void MessageWithError::getError(ErrorCode& errorCode, const char*& errorString) const
{
    errorCode = mErrorCode;
    errorString = mErrorStr;
}

MessageWithError::MessageWithError(const char * tag, UInt32 channelID)
    : Message(tag, channelID)
    , mErrorCode(ERRORCODE_SUCCESS)
    , mErrorStr("Success")
{
    appendErrorNode(mErrorCode, mErrorStr);
}

MessageWithError::MessageWithError(pugi::xml_document* doc)
    : Message(doc)
    , mErrorCode(ERRORCODE_SUCCESS)
    , mErrorStr("Success")
{
    mErrorNode = mMessageDocMainNode.child("error");
    if (mErrorNode)
    {
        // initializing from xml
        const char* errorCodeString = mErrorNode.attribute(XML_ERROR_CODE_TAG).value();
        char *ptr;
        if (errorCodeString != NULL && errorCodeString[0] != '\0')
        {
            mErrorCode = (ErrorCode) strtol(errorCodeString, &ptr, 10);
            mErrorStr = mErrorNode.first_child().value();
        }
    }
    else
    {
        // adding error node with default values
        appendErrorNode(mErrorCode, mErrorStr);
    }
}
