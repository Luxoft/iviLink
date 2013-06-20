/**
 * Copyright placeholder
 */
/**
 * @ingroup             Channel Supervisor
 * @author              Elena Bratanova <ebratanova@luxoft.com>
 * @date                23.04.2013
 */

#include <sstream>
#include <cstdio>
#include <cstring>
#include <cstdlib>

#include "Logger.hpp"
#include "XmlHelper.hpp"

static const char * CHANNEL_ID_TAG = "channel-id";
static const char * CHANNEL_TAG_TAG = "tag";
static const char * MESSAGE_TYPE_TAG = "type";
static const char * MESSAGE_NAME_TAG = "name";

using namespace iviLink::ChannelSupervisor::Messages;

std::string XmlHelper::docToString(pugi::xml_document * doc)
{
    std::stringstream ss;
    doc->print(ss);
    return ss.str();
}

pugi::xml_node XmlHelper::appendIntegerNode(pugi::xml_node node, const char* name, int value)
{   
    pugi::xml_node intNode = node.append_child(name);
    intNode.append_child(pugi::node_pcdata).set_value(convertIntegerToString(value).c_str());
    return intNode;
}

pugi::xml_node XmlHelper::appendCharStringNode(pugi::xml_node node, const char* name,
                const char* value)
{
    pugi::xml_node charNode = node.append_child(name);
    if (value != NULL && value[0] != '\0')
    {
        charNode.append_child(pugi::node_pcdata).set_value(value);
    }
    return charNode;
}

int XmlHelper::extractIntegerFromNode(pugi::xml_node node)
{
    int value = 0;
    const char* valueStr = node.first_child().value();
    if (valueStr != NULL && valueStr[0] != '\0')
    {
        char *ptr;
        value = strtol(valueStr, &ptr, 10);
    }
    return value;
}

char* XmlHelper::extractCharFromNode(pugi::xml_node node)
{
    const char * value = node.first_child().value();
    return const_cast<char*>(value);
}

std::string XmlHelper::parseTag(pugi::xml_document* doc)
{
    pugi::xml_node mainNode = doc->child(XML_MESSAGE_TAG);
    if (mainNode)
    {
        pugi::xml_node tagNode = mainNode.child(CHANNEL_TAG_TAG);
        if (tagNode)
        {
            return (std::string) tagNode.first_child().value();
        }
    }
    return "";
}

UInt32 XmlHelper::parseChannelID(pugi::xml_document * doc)
{
    UInt32 result = 0;
    pugi::xml_node mainNode = doc->child(XML_MESSAGE_TAG);
    if (mainNode)
    {
        pugi::xml_node cidNode = mainNode.child(CHANNEL_ID_TAG);
        if (cidNode)
        {
            const char* cid = cidNode.first_child().value();
            if (cid != NULL && cid[0] != '\0')
            {
                char *ptr;
                result = strtol(cid, &ptr, 10);
            }
        }
    }
    return result;
}

void XmlHelper::appendAttributeAndValue(pugi::xml_node node, const char* name, const char* value)
{
    node.append_attribute(name).set_value(value);
}

void XmlHelper::appendName(pugi::xml_node node, const char* value)
{
    appendAttributeAndValue(node, MESSAGE_NAME_TAG, value);
}

void XmlHelper::appendType(pugi::xml_node node, const char* value)
{
    appendAttributeAndValue(node, MESSAGE_TYPE_TAG, value);
}

void XmlHelper::appendChannelID(pugi::xml_node node, UInt32 value)
{
    appendIntegerNode(node, CHANNEL_ID_TAG, value);
}

void XmlHelper::appendChannelTag(pugi::xml_node node, const char * value)
{
    appendCharStringNode(node, CHANNEL_TAG_TAG, value);
}

std::string XmlHelper::getMessageNameFromDoc(pugi::xml_document* doc)
{
    pugi::xml_node mainNode = doc->child(XML_MESSAGE_TAG);
    if (mainNode)
    {
        return mainNode.attribute(MESSAGE_NAME_TAG).value();
    }
    return "";
}
