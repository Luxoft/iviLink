#include <cstdlib>
#include <unistd.h>
#include "Logger.hpp"

void killProcess(int exitCode)
{
    Logger logger = Logger::getInstance(LOG4CPLUS_TEXT("utils"));
    LOG4CPLUS_TRACE(logger, "begin killProcess (" + convertIntegerToString(exitCode)+")");
#ifndef ANDROID
	exit(exitCode);
#else
	kill(getpid(), SIGKILL);
#endif //ANDROID
}


void killProcess()
{
    Logger logger = Logger::getInstance(LOG4CPLUS_TEXT("utils"));
    LOG4CPLUS_TRACE(logger, __PRETTY_FUNCTION__ );

	killProcess(0);
}
