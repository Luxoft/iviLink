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
#ifndef ANDROID_LAUNCHER_HPP
#define ANDROID_LAUNCHER_HPP

#include <jni.h>

#include "ILauncher.hpp"
#include "Logger.hpp"
#include "CMutex.hpp"

namespace iviLink
{
namespace AMP
{

class AndroidLauncher : public ILauncher
{
public:
    AndroidLauncher(JavaVM * jm, jclass launcher);

    virtual void init(ILauncherHandler * handler);

    virtual void launchApplication(const std::string & launchInfo);
    virtual void deinit();
    virtual ~AndroidLauncher() {}

private:
    static Logger mLogger;
    JavaVM * mJavaVM;
    jclass mLaunchPerformer;
    jmethodID mLaunchMethod;
    ILauncherHandler * mHandler;
    CMutex mInitedMutex;
};

}
}

#endif //ANDROID_LAUNCHER_HPP
