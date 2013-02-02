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
#include <cassert>

#include "com_luxoft_ivilink_samples_media_Bridge.h"
#include "Logger.hpp"
#include "AppInfo.hpp"
#include "StringConversion.hpp"
#include "JniPlayer.hpp"
#include "AndroidMediaSource.hpp"
#include "JniGuiController.hpp"
#include "LocalServerStub.hpp"
#include "CConnectionControl.h"
#include "PlayerLogic.hpp"

jobject gMediaSource = NULL;
jobject gMediaGui = NULL;
jobject gMediaPlayer = NULL;
JavaVM * gJM = NULL;
CConnectionControl * gConnectionControl = NULL;
PlayerLogic * gPlayerLogic = NULL;

Logger log = Logger::getInstance("iviLink.samples.Media.JniLayer");

JNIEXPORT void JNICALL Java_com_luxoft_ivilink_samples_media_Bridge_storeObjects
(JNIEnv * env, jobject, jobject gui, jobject player, jobject source)
{
    LOG4CPLUS_TRACE_METHOD(log, __PRETTY_FUNCTION__);
    env->GetJavaVM(&gJM);
    assert(gJM);
    gMediaGui = env->NewGlobalRef(gui);
    gMediaPlayer = env->NewGlobalRef(player);
    gMediaSource = env->NewGlobalRef(source);
}

JNIEXPORT void JNICALL Java_com_luxoft_ivilink_samples_media_Bridge_startApp
(JNIEnv * env, jobject, jstring jserviceRepo, jstring jlaunchInfo, jstring jinternalPath, jstring jmediaPath)
{
    LOG4CPLUS_TRACE_METHOD(log, __PRETTY_FUNCTION__);
    std::string serviceRepoPath = iviLink::Android::StringConversion::jToStd(jserviceRepo, env);
    std::string privateDirPath = iviLink::Android::StringConversion::jToStd(jinternalPath, env);
    std::string launchInfo = iviLink::Android::StringConversion::jToStd(jlaunchInfo, env);
    iviLink::Android::AppInfo info(serviceRepoPath, privateDirPath, launchInfo, NULL, NULL);
    IPlayer * player= new JniPlayer(gJM, gMediaPlayer);
    IMediaSource * source = new AndroidMediaSource(gJM, gMediaSource, iviLink::Android::StringConversion::jToStd(jmediaPath, env));
    IServer * server = new LocalServerStub();
    IGuiController * gui = new JniGuiController(gJM, gMediaGui);
    gConnectionControl = new CConnectionControl(gui, source, player,server, info);
    gConnectionControl->init();
    gPlayerLogic = gConnectionControl->getPlayerLogic();
    //the following code may be used to test vlc server's functionality
    /*int fds[2];
     pipe (fds);
     gPlayerLogic->startVlcServer(fds[0]);
     FILE * file = fdopen(fds[1], "w");
     std::string message = "play;#rtp{sdp=rtsp://:5544/test};/mnt/sdcard/iviLinkMedia/hatchet.mp3";
     fprintf(file, "%s\n", message.c_str());
     fflush(file);*/
}

JNIEXPORT void JNICALL Java_com_luxoft_ivilink_samples_media_Bridge_playClicked
(JNIEnv *, jobject)
{
    LOG4CPLUS_TRACE_METHOD(log, __PRETTY_FUNCTION__);
    if(gPlayerLogic) gPlayerLogic->localPlay();
}

JNIEXPORT void JNICALL Java_com_luxoft_ivilink_samples_media_Bridge_pauseClicked
(JNIEnv *, jobject)
{
    LOG4CPLUS_TRACE_METHOD(log, __PRETTY_FUNCTION__);
    if(gPlayerLogic) gPlayerLogic->localPause();
}

JNIEXPORT void JNICALL Java_com_luxoft_ivilink_samples_media_Bridge_stopClicked
(JNIEnv *, jobject)
{
    LOG4CPLUS_TRACE_METHOD(log, __PRETTY_FUNCTION__);
    if(gPlayerLogic) gPlayerLogic->localStop();
}

JNIEXPORT void JNICALL Java_com_luxoft_ivilink_samples_media_Bridge_nextClicked
(JNIEnv *, jobject)
{
    LOG4CPLUS_TRACE_METHOD(log, __PRETTY_FUNCTION__);
    if(gPlayerLogic) gPlayerLogic->localNext();
}

JNIEXPORT void JNICALL Java_com_luxoft_ivilink_samples_media_Bridge_prevClicked
(JNIEnv *, jobject)
{
    LOG4CPLUS_TRACE_METHOD(log, __PRETTY_FUNCTION__);
    if(gPlayerLogic) gPlayerLogic->localPrev();
}

JNIEXPORT void JNICALL Java_com_luxoft_ivilink_samples_media_Bridge_posClicked
(JNIEnv *, jobject, jint position, jint oldPosition)
{
    LOG4CPLUS_TRACE_METHOD(log, __PRETTY_FUNCTION__);
    if(gPlayerLogic) gPlayerLogic->localPosClicked((int)position);
}

JNIEXPORT void JNICALL Java_com_luxoft_ivilink_samples_media_Bridge_completed
(JNIEnv *, jobject)
{
    LOG4CPLUS_TRACE_METHOD(log, __PRETTY_FUNCTION__);
    if(gPlayerLogic) gPlayerLogic->obtainPlayerState();
}

JNIEXPORT void JNICALL Java_com_luxoft_ivilink_samples_media_Bridge_syncClicked
(JNIEnv *, jobject)
{
    LOG4CPLUS_TRACE_METHOD(log, __PRETTY_FUNCTION__);
    if(gPlayerLogic) gPlayerLogic->localSync();
}

JNIEXPORT void JNICALL Java_com_luxoft_ivilink_samples_media_Bridge_toggleClicked
(JNIEnv *, jobject)
{
    LOG4CPLUS_TRACE_METHOD(log, __PRETTY_FUNCTION__);
    if(gPlayerLogic) gPlayerLogic->localToggle();
}

JNIEXPORT void JNICALL Java_com_luxoft_ivilink_samples_media_Bridge_playbackStarted
(JNIEnv *, jobject)
{
    LOG4CPLUS_TRACE_METHOD(log, __PRETTY_FUNCTION__);
    if(gPlayerLogic) gPlayerLogic->onPlaybackStartedSafe();
}
