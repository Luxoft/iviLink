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


#ifndef __ANDROID_APP_INFO_HPP
#define __ANDROID_APP_INFO_HPP

#ifndef ANDROID
#else
#include <string>
#include <jni.h>
#endif //ANDROID

namespace iviLink
{
namespace Android
{
// includes all android-specific information that must be passed from app to sdk
struct AppInfo
{
    bool isValid;

    #ifndef ANDROID

    AppInfo() : isValid(false)
    {
    }

    #else
    // When native CApp is used directly with native profiles, only std::string parameters should be set.
    std::string serviceRepoPath; // path to where ServiceRepository on the device is
    std::string privateDirPath;  // path to application's private directory
    std::string launchInfo;      // application's launch info (see AndroidCommonLib: Utils class)
    JavaVM * javaVM;             // pointer to application's java VM
    jclass profileCreatorClass;  // is used for Java-only apps

    AppInfo()
        : serviceRepoPath("")
        , privateDirPath("")
        , launchInfo("")
        , javaVM(NULL)
        , profileCreatorClass(NULL)
        , isValid(false)
    {
    }

    AppInfo(const AppInfo& arg)
    {
        isValid = arg.isValid;
        serviceRepoPath = arg.serviceRepoPath;
        privateDirPath = arg.privateDirPath;
        launchInfo = arg.launchInfo;
        javaVM = arg.javaVM;
        profileCreatorClass = arg.profileCreatorClass;
    }

    AppInfo(const std::string& servicePath, const std::string& privatePath, const std::string& launch, JavaVM* jm, jclass claz)
    {
        isValid = true;
        serviceRepoPath = servicePath;
        privateDirPath = privatePath;
        launchInfo = launch;
        javaVM = jm;
        profileCreatorClass = claz;
    }

    AppInfo(const std::string& servicePath, const std::string& privatePath, const std::string& launch)
    {
        isValid = true;
        serviceRepoPath = servicePath;
        privateDirPath = privatePath;
        launchInfo = launch;
        javaVM = NULL;
        profileCreatorClass = NULL;
    }

    #endif //ANDROID

};

} //namespace Android
} // namespace iviLink


#endif //__ANDROID_APP_INFO_HPP
