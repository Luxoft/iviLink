#ifndef __CHANNEL_TAG_MAP_HPP
#define __CHANNEL_TAG_MAP_HPP

#include <map>

#include "Logger.hpp"
#include "CRWMutex.hpp"
#include "CMutex.hpp"
#include "Channel.hpp"

namespace iviLink
{

namespace Channel
{

typedef std::map<tChannelId, std::string> tChannelTagMap;

class ChannelTagMap 
{
public:
    static ChannelTagMap * getInstance();
    static void deleteInstance();
    void getCopyOfMap(tChannelTagMap& map);

    void insertIntoMap(std::string const& tag, UInt32 channelID);
    void eraseFromMap(UInt32 channelID);

    bool hasChannelInMap(UInt32 channelID);
private:
    ChannelTagMap() {}
    ~ChannelTagMap() {}
    ChannelTagMap(ChannelTagMap const&); // not implemented
    ChannelTagMap& operator=(ChannelTagMap const&); // not implemented
private:
    tChannelTagMap mMap;
    CRWMutex mMapMutex;

    static ChannelTagMap * mInstance;
    static CMutex mSingletonMutex;
    static Logger mLogger;
};

}

}


#endif // __CHANNEL_TAG_MAP_HPP