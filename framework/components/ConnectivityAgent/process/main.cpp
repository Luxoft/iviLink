#ifndef ANDROID
#include "ConnectivityAgentEntryPoint.hpp"

int main(int argc, char ** argv) 
{
	return iviLink::ConnectivityAgent::connectivityAgentEntryPoint(argc, argv);
}

#endif //ANDROID