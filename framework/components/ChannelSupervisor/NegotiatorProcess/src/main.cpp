#ifndef ANDROID

#include "NegotiatorEntryPoint.hpp"

int main(int argc, char ** argv)
{
    return iviLink::ChannelSupervisor::negotiatorEntryPoint(argc, argv);
}

#endif //ANDROID
