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
#ifndef C_ANDROID_MEDIA_SOUCE_IMPL_HPP
#define C_ANDROID_MEDIA_SOUCE_IMPL_HPP

#include <jni.h>

#include "MediaSource.hpp"

#include "Logger.hpp"

class AndroidMediaSource: public Playlist
{
public:
    AndroidMediaSource(JavaVM * pJM, jobject callbacks, const std::string& pathToFiles);
    virtual ~AndroidMediaSource()
    {
    }

    std::list<std::string> getFolderBlacklist() const;

    virtual void startVlcServer(const int pipefds);
private:
    JavaVM * mpJM;
    jobject mjCallbacks;
    Logger mLogger;

};

#endif //C_ANDROID_MEDIA_SOUCE_IMPL_HPP
