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


#ifndef SYSTEMCONTROLLERWATCHDOG_HPP
#define SYSTEMCONTROLLERWATCHDOG_HPP

#include <map>
#include <string>
#include <cstring>

#ifndef ANDROID
#else
#include <jni.h>
#endif //ANDROID

#include "SystemControllerWatchdogCommon.hpp"
#include "CMutex.hpp"

#ifndef PREFIX
#define PREFIX ./
#endif // PREFIX

namespace iviLink
{
namespace SystemController
{

class SystemControllerWatchdog
{
private:
    int maxFailCount;   // If the number of falls over this counter system should be restarted 
    
    /*map save information about all running component*/
    tComponentStatusMap componentStatusMap; 
    CMutex componentsStatusMapMutex;
    
    static SystemControllerWatchdog* instance;
    static CMutex singletonMutex;

/*Methods*/
public:

    static SystemControllerWatchdog* getInstance();
    
    /*
    * Check components status
    * return 
    *   true - all started components are alive
    *   faulse - one or more component is down
    */
    bool waitLoop();

    /**
    * Add to map component and it's status
    * This function has no return parameters
    */
    void addComponent(Component* componentStatus);

    /**
    * Remove from map component by @parem key
    * This function has no return parameters
    */
    void removeComponent(const int key);

private:

    /**
    * @param component - is this component respond on WD request
    * return 
    *   true, if component respond
    *   faulse - if not
    */
    bool isComponentResponsive(const Components component) const;

    /**
    * This function return number of components which status IS_DOWN
    */
    int getNumberOfFailComponent() const;

    void updateAllComponentsStatus();

    void updateComponentStatus(const Components component);

protected:
    SystemControllerWatchdog();

    virtual ~SystemControllerWatchdog();

    int recvAnswer(const Components component, UInt8 &message) const;

};

}   /* end of SystemController namespace*/
} // namespace iviLink

#endif /* SYSTEMCONTROLLERWATCHDOG_HPP */
