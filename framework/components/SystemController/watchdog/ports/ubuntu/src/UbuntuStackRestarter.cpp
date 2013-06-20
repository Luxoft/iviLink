#include <string>
#include <sys/types.h>
#include <stdlib.h>

#include "UbuntuStackRestarter.hpp"

namespace iviLink
{

namespace SystemController
{

Logger UbuntuStackRestarter::mLogger = Logger::getInstance("SystemControllerWatchdog.Restarter");

void UbuntuStackRestarter::restartIviLinkFromWatchdog()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    pid_t pid = fork();
    assert(pid == 0);
    if (pid == 0)
    {
        std::string systemControllerName(SYSTEM_CONTROLLER_PROCESS_NAME);
        systemControllerName = "./" + systemControllerName;//TODO: shouldn't SystemController be restarted not from current dir?
        execl(systemControllerName.c_str(), systemControllerName.c_str(), "-r", "-1", NULL);
        exit(1);
    }
}

}
}
