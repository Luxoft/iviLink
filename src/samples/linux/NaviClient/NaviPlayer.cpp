/* 
 * iviLINK SDK, version 1.1.19
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
 */ 
 
#include <unistd.h>
#include <signal.h>

#include "NaviPlayer.hpp"

Logger NaviPlayer::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("samples.Applications.NaviPlayer"));

NaviPlayer::NaviPlayer(QPushButton *parent, std::string path) :
   CApp(iviLink::Service::Uid(SERVICE)),
   mpParent(parent)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    mPath = path+std::string("/");
    mpParent->setEnabled(false);
}

void NaviPlayer::init()
{
   initInIVILink();

   mMedia = new Phonon::MediaObject(mpParent);
   mAudio = new Phonon::AudioOutput(Phonon::MusicCategory, mpParent);
   Phonon::createPath(mMedia, mAudio);
   mAudio->setVolume(1);
   connect(mMedia, SIGNAL(stateChanged(Phonon::State,Phonon::State)), this, SLOT(stateChanged(Phonon::State,Phonon::State)),Qt::QueuedConnection);
   mState=Phonon::StoppedState;
}

void NaviPlayer::playTracks(std::deque<std::string> tracks)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    std::string mediaPath;
    mediaPath = mPath+tracks.front()+std::string(".mp3");
    tracks.pop_front();
    MutexLocker lock(playingQueueMutex);
    if(mState!=Phonon::PlayingState){
        LOG4CPLUS_INFO(msLogger, "media state: not playing, setting current source");
        mMedia->setCurrentSource(QString(mediaPath.c_str()));
        mMedia->play();
    }else{
        LOG4CPLUS_INFO(msLogger, "media state: playing, enqueuing");
        playingQueue.push_back(mediaPath);
    }
    while(!tracks.empty())
    {
        mediaPath = mPath+tracks.front()+".mp3";
        tracks.pop_front();
        playingQueue.push_back(mediaPath);
    }
}

void NaviPlayer::stateChanged(Phonon::State newState, Phonon::State oldState)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    MutexLocker lock(playingQueueMutex);
    if(newState == Phonon::PlayingState) {
        LOG4CPLUS_INFO(msLogger, "media state: playing");
        mState=newState;
    } else if(newState == Phonon::StoppedState || newState==Phonon::PausedState) {
        LOG4CPLUS_INFO(msLogger, "media state: stopped or paused");
        mState = newState;
        if(playingQueue.empty()) {
           LOG4CPLUS_INFO(msLogger, "played everything in the queue!");
        } else{
            mMedia->setCurrentSource(QString(playingQueue.front().c_str()));
            playingQueue.pop_front();
            mMedia->play();
        }
    } else if(newState==Phonon::ErrorState){
           LOG4CPLUS_ERROR(msLogger, "Phonon::ErrorState");
    }
}

void NaviPlayer::onInitDone(iviLink::ELaunchInfo launcher)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
   if (iviLink::LAUNCHED_BY_USER == launcher)
   {
      LOG4CPLUS_INFO(msLogger, "started by user");
      LOG4CPLUS_INFO(msLogger, "Registering profile callbacks");
      registerProfileCallbacks(iviLink::Profile::ApiUid("NaviReceiverProfile_PAPI_UID"),this);
      mpParent->setText("Loading...");
      if (loadService(iviLink::Service::Uid(SERVICE)))
      {
         LOG4CPLUS_INFO(msLogger, "service started successfully!");
         mpParent->setText("Click to exit");
         mpParent->setEnabled(true);
      }
      else
      {
         LOG4CPLUS_ERROR(msLogger, "service NOT started");
         kill(getpid(), SIGKILL);
      }
   }
   else
   {
      LOG4CPLUS_INFO(msLogger, "started by iviLink");
   }
}

void NaviPlayer::onIncomingServiceBeforeLoading(const iviLink::Service::Uid &service)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
   LOG4CPLUS_INFO(msLogger, "Registering profile callbacks");
   registerProfileCallbacks(iviLink::Profile::ApiUid("NaviReceiverProfile_PAPI_UID"),this);
   mpParent->setText("Loading...");
}

void NaviPlayer::onIncomingServiceAfterLoading(const iviLink::Service::Uid &service)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
   mpParent->setText("Click to exit");
   mpParent->setEnabled(true);
}

void NaviPlayer::onServiceDropped(const iviLink::Service::Uid &service)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
   LOG4CPLUS_WARN(msLogger, "Other side has died!");
   kill(getpid(), SIGKILL);
}


void NaviPlayer::exitClicked()
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
   kill(getpid(), SIGKILL);
}
