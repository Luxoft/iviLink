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
#include "CButtonProcessor.h"


static const char* media_app = "../applications/MediaApp";
static const char* seat_app = "../applications/Seat_App";
static const char* climate_app = "../applications/Climate_App";
static const char* restart = "../applications/restart.sh";
static const char* restart_n = "../applications/restart-n.sh";
static const char* shutdown = "../applications/shutdown.sh";
static const char* unpair = "../applications/unpair.sh";

Logger CButtonProcessor::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("samples.Applications.launch.CButtonProcessor"));

CButtonProcessor::CButtonProcessor(bool isAuthEnabled) 
                        : mIsAuthEnabled(isAuthEnabled)
{
	LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
}

void CButtonProcessor::mediaPressed()
{
 	LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
	launchApp(media_app);
}
void CButtonProcessor::seatPressed()
{
 	LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
	launchApp(seat_app);
}
void CButtonProcessor::climatePressed()
{
 	LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
	launchApp(climate_app);
}

void CButtonProcessor::resetPressed()
{
	LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    if (mIsAuthEnabled)
    {
        if (system(restart) )
            LOG4CPLUS_INFO(msLogger, "iviLink restarted");
    }
    else
    {
        if (system(restart_n) )
            LOG4CPLUS_INFO(msLogger, "iviLink restarted");  
    }

}

void CButtonProcessor::shutdownPressed()
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);

   if(system(shutdown))
   {
      LOG4CPLUS_INFO(msLogger, "iviLink shutdown");
   }
}

void CButtonProcessor::unpairPressed()
{
 	LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
	if (system(unpair))
		 LOG4CPLUS_INFO(msLogger, "iviLink unpaired");
	if (system(restart))
		 LOG4CPLUS_INFO(msLogger, "iviLink restarted");
}

void CButtonProcessor::launchApp(std::string path)
{
	LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    pid_t pid = fork();

        switch (pid) {
            case -1:
            {
                LOG4CPLUS_ERROR(msLogger, "after fork pid = -1");
                break;
            }
            case 0:
            {
                LOG4CPLUS_INFO(msLogger, "child");
                if (execl(path.c_str(), path.c_str(), NULL))
                {
                     LOG4CPLUS_FATAL(msLogger, "execl failed");
                     killProcess(1);
                }
                killProcess(1);
                break;
            }
            default:
            {
 				LOG4CPLUS_INFO(msLogger, "parent");
            }
        }
}
