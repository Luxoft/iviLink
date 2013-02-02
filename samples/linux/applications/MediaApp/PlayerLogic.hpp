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


#ifndef PLAYERLOGIC_HPP
#define PLAYERLOGIC_HPP

#include <list>
#include <iostream>

#include "Logger.hpp"
#include "IMediaControlClientProfileAPI.hpp"
#include "IMediaControlServerProfileAPI.hpp"
#include "IMediaSourceClientProfileAPI.hpp"
#include "IMediaSourceServerProfileAPI.hpp"
#include "IMediaStreamingClientProfileAPI.hpp"
#include "IMediaStreamingServerProfileAPI.hpp"

#include "CMediaSourceClientProfileProxy.hpp"
#include "CMediaSourceServerProfileProxy.hpp"
#include "CMediaControlClientProfileProxy.hpp"
#include "CMediaControlServerProfileProxy.hpp"
#include "CMediaStreamingClientProfileProxy.hpp"
#include "CMediaStreamingServerProfileProxy.hpp"

#include "MediaAppCommon.hpp"
#include "CMediaState.hpp"
#include "IGuiController.hpp"
#include "IMediaSource.hpp"
#include "IPlayer.hpp"
#include "IServer.hpp"

#ifndef ANDROID
#include <QObject>
#endif //ANDROID

#ifndef ANDROID
class PlayerLogic: public QObject,
        public IMediaControlClientProfile_API::Callbacks
#else
        class PlayerLogic: public IMediaControlClientProfile_API::Callbacks
#endif //ANDROID
        ,
        public IMediaControlServerProfile_API::Callbacks,
        public IMediaSourceClientProfile_API::Callbacks,
        public IMediaSourceServerProfile_API::Callbacks,
        public IMediaStreamingClientProfile_API::Callbacks,
        public IMediaStreamingServerProfile_API::Callbacks,
        public IPlayerCallback
{
#ifndef ANDROID
    Q_OBJECT
#endif //ANDROID
public:
    PlayerLogic(iviLink::Android::AppInfo info, IGuiController * controller, IMediaSource* source,
            IPlayer * player, IServer * server);

    virtual ~PlayerLogic();
    void connection();

    //from controlServer
    virtual void onServerToggle(bool needLock); //just receive
    virtual void onServerSync(bool needLock); //just receive
    virtual void onServerUnsync(bool needLock); //just receive
    virtual void onServerPlay(std::string const& trackName, std::string const& trackUid,
            std::string const& source, std::string const& avform, bool needLock); //start new broadcast and send the msg to app
    virtual void onServerResume(bool needLock);
    virtual void onServerPause(bool needLock); //pause broadcast and send the msg to app
    virtual void onServerStop(bool needLock); //send the msg to appx
    virtual std::string onMediaPathRequest(bool needLock);
    virtual void startVlcServer(int pipeReadDescriptor, bool needLock);

    //from  conrtolClient
    virtual void onClientChangeHighlightedTrack(int newPosition, bool needLock); // change highlited track (current position of cursor) 
    virtual void onClientToggle(bool needLock); //just receive
    virtual void onClientSync(bool needLock); //just receive
    virtual void onClientUnsync(bool needLock); //just receive
    virtual void onClientPlay(std::string const& trackName, std::string const& trackUid,
            std::string const& source, std::string const& avform, bool needLock); //start new broadcast and send the msg to app
    virtual void onClientResume(bool needLock);
    virtual void onClientPause(bool needLock); //pause broadcast and send the msg to app
    virtual void onClientStop(bool needLock); //send the msg to appx

    //from sourceClient
    virtual void onRecallTrackList(bool needLock);
    virtual void onTrackListReceived(std::string const& trackList, bool needLock);
    virtual void onTrackListChanged(bool needLock);

    //from sourceServer
    virtual void onRequestTrackList(bool needLock);

    //from streamingClient
    virtual void onStreamingClientAccepted(bool needLock);
    virtual void onStreamingClientUnaccepted(bool needLock);

    //from streamingServer
    virtual void onStreamingServerAccepted(bool needLock);
    virtual void onStreamingServerUnaccepted(bool needLock);

    virtual void handleError(BaseError const & error)
    {
        LOG4CPLUS_INFO(msLogger, static_cast<std::string>(error));
    }

    void initByUser();
    void initByLauncher();


public:
    // unsafe call - but on ubuntu is is invoked safely
    virtual void onPlaybackStarted();
    // safe call - for Android
    virtual void onPlaybackStartedSafe();

#ifndef ANDROID
public slots:
#else
public:
#endif //ANDROID
    void localPlay();
    void localStop();
    void localPause();
    void localNext();
    void localPrev();
    void localVolumeUp();
    void localVolumeDown();
    void localToggle();
    void localSync();
    void localExit();
    void localPosClicked(int newpos);
    void updateListView();
    void obtainPlayerState();

private:
    void remoteSyncUnsafe();
    void remoteUnsyncUnsafe();
    void remoteToggleUnsafe();

    void remotePlayUnsafe(std::string const& trackName, std::string const& trackUid,
            std::string const& source, std::string const& avform);
    void remoteResumeUnsafe();
    void remotePauseUnsafe();
    void remoteStopUnsafe();

    void localPlayUnsafe();
    void localPauseUnsafe();
    void localStopUnsafe();
    void localNextUnsafe();

    void playPositionUnsafe(int newpos);

private:

    IPlayer* mPlayer;
    CMediaState* mMediaState;
    CMediaSourceClientProfileProxy* mSourceClientProxy;
    CMediaSourceServerProfileProxy* mSourceServerProxy;
    CMediaControlClientProfileProxy* mControlClientProxy;
    CMediaControlServerProfileProxy* mControlServerProxy;
    CMediaStreamingClientProfileProxy* mStreamingClientProxy;
    CMediaStreamingServerProfileProxy* mStreamingServerProxy;

    IGuiController* mGuiController;           //updates Gui on external requestes
    IMediaSource* mMediaSource;
    IServer* mLocalServer;

    Direction mDirection;
    bool mIsInitByUser;           

    static Logger msLogger;

    iviLink::Android::AppInfo mAppInfo;
};

#endif // CPLAYERLOGIC_H
