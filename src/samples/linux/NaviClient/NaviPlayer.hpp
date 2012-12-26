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
 
#ifndef NAVIPLAYER_HPP
#define NAVIPLAYER_HPP

#include <QObject> 
#include <QPushButton> 
#include <QApplication>
#include <phonon/MediaObject>
#include <phonon/MediaSource>
#include <phonon/AudioOutput>

#include <deque>
#include <string>

//CApp header, must be included for interaction with apllication manager
#include "CApp.hpp"
#include "Logger.hpp"
#include "CMutex.hpp"
//basic sample api profile header, must be included for implementing callbacks
#include "INaviReceiverProfile.hpp"

#define SERVICE "NavigationVoiceService"


class NaviPlayer :  public QObject,
                    public iviLink::CApp,
                    public INaviReceiverProfile_API::Callbacks
{
    Q_OBJECT

public:
    NaviPlayer(QPushButton *parent, std::string path="../navi");
    void init();
   
    // from profile
    void playTracks(std::deque<std::string> tracks);
    // from CApp
   virtual void onInitDone(iviLink::ELaunchInfo launcher);

   /**
    * Callback is called before loading incoming service
    * @param service is UID of service
    */
   virtual void onIncomingServiceBeforeLoading(const iviLink::Service::Uid &service);

   /**
    * Callback is called after loading of incoming service
    */
   virtual void onIncomingServiceAfterLoading(const iviLink::Service::Uid &service);

   /**
    * Callback is called in case of and service session
    * @param service UID is uid of service
    */
   virtual void onServiceDropped(const iviLink::Service::Uid &service);

   virtual void handleError(CError const & error)
   {
      LOG4CPLUS_INFO(msLogger,static_cast<std::string>(error));
   }

public slots:
  void exitClicked();

private slots:
    void stateChanged(Phonon::State newState,Phonon::State oldState);

private:
    Phonon::MediaObject * mMedia;
    Phonon::AudioOutput * mAudio;
    std::string mPath;
    Phonon::State mState;
    std::deque<std::string> playingQueue;
    CMutex playingQueueMutex;
    static Logger msLogger;
    QPushButton * mpParent;

};
#endif // NAVIPLAYER_H
