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


#ifndef MEDIA_APP_COMMON_HPP
#define MEDIA_APP_COMMON_HPP

#include <string>
#include <stdio.h>
#include <map>

#define ERROR_PLAY                  (-1)
#define AUDIO_EXTENSION             ".m4a"
#define VIDEO_EXTENSION             ".mp4"
#define VIDEO                       "video"
#define AUDIO                       "audio"
#define VCODE                       "#rtp{sdp=rtsp://:"
#define ACODE                       "#rtp{sdp=rtsp://:"
#define MEDIA_NAME                  "local_media"

//
static const char * const supported_audio_extensions[] = {"*.aac", "*.ac3", "*.m4a", "*.mp3", "*.wav"};

static const char * const supported_video_extensions[] =
{ "*.3gp", "*.avi", "*.divx", "*.mkv", "*.mov", "*.mp2", "*.mp4", "*.mpeg", "*.mpg" };

enum windowPosition
{
    XLEFTPOS = 0, XRIGHTPOS = 800, YUPPOS = 0, YDOWNPOS = 600,
};

enum videoControlWindowPosition
{
    VID_XLEFTPOS = 0, VID_XRIGHTPOS = 550, VID_YUPPOS = 0, VID_YDOWNPOS = 106,
};

enum volume
{
    VOLUME_STEP = 10, VOLUME_MAX = 191, VOLUME_MIN = 9, VOLUME_INIT = 50,
};

enum playBack
{
    PLAY = 0, PAUSE, STOP,
};

//defines who streaming and doesn't cares about sink side
enum role
{
    CLIENT = 0, SERVER,
};

enum connections
{
    CONNECTED = 1, DISCONNECTED = 0,
};

//stream defines where we are sink aoudio or video stream
enum stream
{
    MYSIDE = 0, OTHERSIDE, BOTH,
};

//
struct Direction
{
    bool outgoing; //true - we have sent tracklists and we sent that streaming accepted
    bool incoming;

    Direction()
            : outgoing(false), incoming(false)
    {
    }
};

struct trackInfo
{
    std::string trackName;
    bool owner;
};
typedef std::map<std::string, trackInfo> commonTrackListMap;
typedef std::map<std::string, std::string> uidNameMap;

#define UBUNTU_LOCAL_DIRECTORY "media/"
#endif // COMMON_HPP
