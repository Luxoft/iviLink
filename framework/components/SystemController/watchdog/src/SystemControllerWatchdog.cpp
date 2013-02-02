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


#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <signal.h>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "SystemControllerWatchdog.hpp"

using namespace std;

namespace iviLink
{
namespace SystemController
{

Logger wlogger = Logger::getInstance(LOG4CPLUS_TEXT("SystemControllerWatchdog"));
CMutex SystemControllerWatchdog::singletonMutex;
SystemControllerWatchdog* SystemControllerWatchdog::instance = 0;

SystemControllerWatchdog::SystemControllerWatchdog() 
{
    maxFailCount = MAX_FAIL_COUNT;
}

void SystemControllerWatchdog::addComponent(Component* componentStatus)
{
    MutexLocker lock(componentsStatusMapMutex);
    componentStatusMap.insert(pair<int, Component*> (componentStatus->component, componentStatus));
}

void SystemControllerWatchdog::removeComponent(const int key)
{
    MutexLocker lock(componentsStatusMapMutex);
    tComponentStatusMap::iterator cur = componentStatusMap.find(key);
    if (cur != componentStatusMap.end())
    {
        delete (*cur).second;
        componentStatusMap.erase(cur);
    }
}

SystemControllerWatchdog * SystemControllerWatchdog::getInstance()
{
    MutexLocker lock(singletonMutex);
    if (instance == 0) 
    {
        instance = new SystemControllerWatchdog();
        LOG4CPLUS_INFO(wlogger, "SystemControllerWatchdog created");
    }
    return instance;
}

bool SystemControllerWatchdog::waitLoop()
{
    MutexLocker lock(componentsStatusMapMutex);
    updateAllComponentsStatus();
    return (getNumberOfFailComponent() == 0);
}

void SystemControllerWatchdog::updateComponentStatus(const Components component)
{    
    tComponentStatusMap::iterator cur = componentStatusMap.find((int)component);
    if (cur != componentStatusMap.end())
    {
        if (!isComponentResponsive(component))
        {
            (*cur).second->failCount += 1;
            if ((*cur).second->failCount >= maxFailCount)
            {
                LOG4CPLUS_INFO(wlogger, "Component " + (std::string)ComponentsNames[component] + " is DOWN");
                (*cur).second->status = IS_DOWN;
            }
        }
        else
        {
            (*cur).second->resetCounters();
        }
    }
    else
    {
        LOG4CPLUS_ERROR(wlogger, "no such component as " + (std::string)ComponentsNames[component]);
    }
}

void SystemControllerWatchdog::updateAllComponentsStatus()
{
    tComponentStatusMap::iterator cur;
    for (cur = componentStatusMap.begin(); cur != componentStatusMap.end();  cur++)
    {
        updateComponentStatus((*cur).second->component);
    }
}

bool SystemControllerWatchdog::isComponentResponsive(const Components component) const
{
    UInt8 message = 0;

    if ((recvAnswer(const_cast <Components &> (component), message) > 0) && (message != 0))
    {
        LOG4CPLUS_INFO(wlogger, "Component " + (std::string)ComponentsNames[component] + " is responsive");
        return true;
    }
    LOG4CPLUS_WARN(wlogger, "Component " + (std::string)ComponentsNames[component] + " is NOT responsive");
    return false;
}


int SystemControllerWatchdog::recvAnswer(const Components component, UInt8 &message) const
{
    message = 0;
    tComponentStatusMap::const_iterator cur = componentStatusMap.find((int)component);
    if (cur == componentStatusMap.end()) return 0;
    int bytesRead = (*cur).second->socket.receiveMessage(message);
    if ( bytesRead <= 0)
    {
        LOG4CPLUS_ERROR(wlogger, "Nothing received from " + (std::string)ComponentsNames[component]);
    }
    return bytesRead;
}

int SystemControllerWatchdog::getNumberOfFailComponent() const
{
    int count = 0;
    tComponentStatusMap::const_iterator cur;
    for (cur = componentStatusMap.begin(); cur != componentStatusMap.end(); cur++)
    {
        if (((*cur).second->status == IS_DOWN))
        {
            LOG4CPLUS_INFO(wlogger, "Failed component: " + (std::string)ComponentsNames[(*cur).second->component]);
            count++;
        }
    }
    if (count > 0)
    {
        LOG4CPLUS_INFO(wlogger, "Failed NUMBER: " + convertIntegerToString(count));
    }
    return count;
}

SystemControllerWatchdog::~SystemControllerWatchdog()
{
}

}; /*namespace SystemController*/
}; // namespace iviLink

