/**
 * Copyright placeholder
 */
/**
 * @ingroup             Channel Supervisor
 * @author              Elena Bratanova <ebratanova@luxoft.com>
 * @date                23.04.2013
 */

#ifndef __XML__HELPER__HPP
#define __XML__HELPER__HPP

#include <string>
#include "pugixml.hpp"
#include "Types.hpp"

#define XML_MESSAGE_TAG "message"

namespace iviLink
{
namespace ChannelSupervisor
{
namespace Messages
{

class XmlHelper
{
public:
    static std::string docToString(pugi::xml_document*);
    static int extractIntegerFromNode(pugi::xml_node node);
    static char* extractCharFromNode(pugi::xml_node node);
    static std::string parseTag(pugi::xml_document* doc);
    static UInt32 parseChannelID(pugi::xml_document* doc);

    static void appendName(pugi::xml_node node, const char* value);
    static void appendType(pugi::xml_node node, const char* value);
    static void appendChannelID(pugi::xml_node node, UInt32 value);
    static void appendChannelTag(pugi::xml_node node, const char* value);

    static std::string getMessageNameFromDoc(pugi::xml_document* doc);

    static void appendAttributeAndValue(pugi::xml_node node, const char* name, const char* value);
    static pugi::xml_node appendIntegerNode(pugi::xml_node node, const char* name, int value);
    static pugi::xml_node appendCharStringNode(pugi::xml_node node, const char* name,
            const char* value);
};

}
}
}


#endif // __XML__HELPER__HPP