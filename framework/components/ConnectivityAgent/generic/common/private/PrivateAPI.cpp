#include "PrivateAPI.hpp"

#include "CConnectivityAgentProxy.hpp"


void forceChannelDeallocation(UInt32 channelID)
{
   CConnectivityAgentProxy::getInstance()->deallocateChannelForcibly(channelID);
}