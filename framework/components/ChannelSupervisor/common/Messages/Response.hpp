/* 
 * iviLINK SDK, version 1.2
 * http://www.ivilink.net
 * Cross Platform Application Communication Stack for In-Vehicle Applications
 * 
 * Copyright (C) 2012-2013, Luxoft Professional Corp., member of IBS group
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; version 2.1.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 * 
 */ 


#ifndef RESPONSE_HPP_
#define RESPONSE_HPP_

#include <list>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "pugixml.hpp"
#include "Message.hpp"
#include "ErrorCode.hpp"

namespace iviLink
{
namespace ChannelSupervisor
{
namespace Messages
{

class ResponseTypesMap;

class Response: public Message
{
public:

    enum ResponseType
    {
        RESPONSETYPE_UNKNOWN = 0,
        RESPONSETYPE_ALLOCATE_CHANNEL = 1,
        RESPONSETYPE_DEALLOCATE_CHANNEL,
        RESPONSETYPE_COMMAND_REJECT,
        RESPONSETYPE_COMMAND_TIMEOUT,
        RESPONSETYPE_CA_ALLOCATION_DONE,
        RESPONSETYPE_UPDATE_CHANNEL_INFO,
    };

    Response(ResponseType responseType = RESPONSETYPE_UNKNOWN);

    Response(pugi::xml_document* doc);

    virtual ~Response()
    {
    }

    virtual ResponseType GetResponseName() const
    {
        return m_responseType;
    }

    void SetError(ErrorCode errorCode, const char* errorString)
    {
        SetError(m_errorNode, errorCode, errorString);
    }

    void GetError(ErrorCode& errorCode, const char*& errorString)
    {
        GetError(m_errorNode, errorCode, errorString);
    }

    static ResponseType GetResponseType(pugi::xml_document* _doc);

protected:

    static pugi::xml_node AppendResultNode(pugi::xml_node resultsNode, const char* modelId);
    static pugi::xml_node AppendErrorNode(pugi::xml_node node, ErrorCode errorCode,
            const char* errorString);

    static void SetError(pugi::xml_node errorNode, ErrorCode errorCode, const char* errorString);
    static void GetError(pugi::xml_node errorNode, ErrorCode& errorCode, const char*& errorString);

    static int ExtractIntegerNode(pugi::xml_node node);
    static char* ExtractCharNode(pugi::xml_node node);

    pugi::xml_node m_errorNode;

    ErrorCode m_errorCode;
    const char *m_errorStr;

    ResponseType m_responseType;

    static ResponseTypesMap m_responseTypes;

};

class ResponseTypesMap: public std::map<Response::ResponseType, std::string>
{
public:
    ResponseTypesMap()
    {
        (*this)[Response::RESPONSETYPE_ALLOCATE_CHANNEL] = "allocate-channel";
        (*this)[Response::RESPONSETYPE_DEALLOCATE_CHANNEL] = "deallocate-channel";
        (*this)[Response::RESPONSETYPE_COMMAND_REJECT] = "command-reject";
        (*this)[Response::RESPONSETYPE_COMMAND_TIMEOUT] = "command-timeout";
        (*this)[Response::RESPONSETYPE_CA_ALLOCATION_DONE] = "ca-allocation-done";
        (*this)[Response::RESPONSETYPE_UPDATE_CHANNEL_INFO] = "update-channleInfo";
    }
};

}  // Messages
}  // ChannelSupervisor
}  // AXIS

#endif // RESPONSE_HPP_
