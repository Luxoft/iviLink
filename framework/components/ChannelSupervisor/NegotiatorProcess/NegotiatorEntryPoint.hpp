/* 
 * 
 * iviLINK SDK, version 1.1.2
 * http://www.ivilink.net
 * Cross Platform Application Communication Stack for In-Vehicle Applications
 * 
 * Copyright (C) 2012, Luxoft Professional Corp., member of IBS group
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
 * 
 */

#ifndef __NEGOTIATOR_ENTRY_POINT__
#define __NEGOTIATOR_ENTRY_POINT__

#include <cassert>
#include <getopt.h>
#include <iostream>
#include <string>
#include <sstream>

#include "Types.hpp"
#include "Negotiator.hpp"
#include "Logger.hpp"
#include "Components.hpp"
#include "WatchdogThread.hpp"

extern char const * gpCA_UNIX_SOCK_PATH;

namespace iviLink
{
namespace ChannelSupervisor
{
static struct option long_options[] =
    {
        { "cs_unix_addr", required_argument, 0, 'x' },
        { "ca_unix_addr", required_argument, 0, 'u' },
        { "cs_test_role", required_argument, 0, 'r' },
        { 0, 0, 0, 0 } };

char ca_sock_path[1024] = "";

int negotiatorEntryPoint(int argc = 0, char ** argv = NULL)
{
    Logger logger = Logger::getInstance(LOG4CPLUS_TEXT("NegotiatorProcess.main"));

    WatchdogThread watchdogThread(SystemController::CHANNEL_SUPERVISOR);
    watchdogThread.start();

#ifndef ANDROID
    PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT("log4cplus.properties"));

    LOG4CPLUS_INFO(logger, "Negotiator Process started...");

    int option_index = 0;
    int c;

    std::string cs_sock_addr;
    std::string testRole="";

    while (true)
    {
        c = getopt_long(argc, argv, "x:u:r:", long_options, &option_index);

        if (-1 == c)
            break;

        switch (c)
        {
        case 'x':
            cs_sock_addr = std::string(optarg);
            break;
        case 'u':
            strcpy(ca_sock_path, optarg);
            break;
        case 'r':
            testRole = std::string(optarg);
            break;
        default:
            exit(1);
            break;
        }
    }

    if (ca_sock_path[0] != 0)
    {
        gpCA_UNIX_SOCK_PATH = ca_sock_path;
    }
    Negotiator* negotiator = new Negotiator(cs_sock_addr);
    if (!testRole.empty()) // is used for testing purposes
    {
        negotiator->onReceiveRole("master" == testRole);
    }
#else
    std::string cs_sock_addr("");
    Negotiator* negotiator = new Negotiator(cs_sock_addr);
#endif //ANDROID

    negotiator->init();

    negotiator->waitShutdown();
    LOG4CPLUS_INFO(logger, "Negotiator Process ends...");
    delete negotiator;
    return 0;
}

}
}
#endif // __NEGOTIATOR_ENTRY_POINT__
