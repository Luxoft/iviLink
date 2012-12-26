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
 

#ifndef CPLAYERLOGIC_H
#define CPLAYERLOGIC_H

#include <list>
#include <iostream>

#include "Logger.hpp"
#include "CRWMutex.hpp"
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

#include "common.hpp"
#include "CMediaState.hpp"
#include "IGuiController.hpp"
#include "IMediaSource.hpp"
#include "IPlayer.hpp"
#include "IServer.hpp"


#ifndef ANDROID
#include <QObject>
#endif //ANDROID

#ifndef ANDROID
class CPlayerLogic: public QObject
        , public IMediaControlClientProfile_API::Callbacks
#else
class CPlayerLogic: public IMediaControlClientProfile_API::Callbacks
#endif //ANDROID
        , public IMediaControlServerProfile_API::Callbacks
        , public IMediaSourceClientProfile_API::Callbacks
        , public IMediaSourceServerProfile_API::Callbacks
        , public IMediaStreamingClientProfile_API::Callbacks
        , public IMediaStreamingServerProfile_API::Callbacks
{
#ifndef ANDROID
	Q_OBJECT
#endif //ANDROID
public:
#ifndef ANDROID
    CPlayerLogic( IGuiController * controller,
                  IMediaSource* source,
                  IPlayer * player,
                  IServer * server);
#else
    CPlayerLogic( iviLink::Android::AppInfo info, 
                  IGuiController * controller,
                  IMediaSource* source,
                  IPlayer * player,
                  IServer * server);
#endif //ANDROID

    virtual ~CPlayerLogic();
    void connection();

    //from controlServer
    virtual void onServerToggle(); //just receive
    virtual void onServerSync(); //just receive
    virtual void onServerUnsync(); //just receive
    virtual void onServerPlay(std::string  const& trackName,std::string  const& trackUid, std::string const& source, std::string const& avform); //start new broadcast and send the msg to app
    virtual void onServerResume();
    virtual void onServerPause(); //pause broadcast and send the msg to app
    virtual void onServerStop(); //send the msg to appx
    virtual std::string onMediaPathRequest();
    virtual void startVlcServer(int pipeReadDescriptor);

    //from  conrtolClient
    virtual void onClientChangeHighlightedTrack(int newPosition); // change highlited track (current position of cursor) 
    virtual void onClientToggle(); //just receive
    virtual void onClientSync(); //just receive
    virtual void onClientUnsync(); //just receive
    virtual void onClientPlay(std::string  const& trackName,std::string  const& trackUid, std::string const& source, std::string const& avform); //start new broadcast and send the msg to app
    virtual void onClientResume();
    virtual void onClientPause(); //pause broadcast and send the msg to app
    virtual void onClientStop(); //send the msg to appx

    //from sourceClient
    virtual void onRecallTrackList();
    virtual void onTrackListReceived(std::string const& trackList);
    virtual void onTrackListChanged();

    //from sourceServer
    virtual void onRequestTrackList();

    //from streamingClient
    virtual void onStreamingClientAccepted();
    virtual void onStreamingClientUnaccepted();

    //from streamingServer
    virtual void onStreamingServerAccepted();
    virtual void onStreamingServerUnaccepted();

    virtual void handleError(CError const & error)
    {
        LOG4CPLUS_INFO(msLogger,static_cast<std::string>(error));
    }

    void initByUser();
    void initByLauncher();

    void remotePlay(std::string  const& trackName,std::string  const& trackUid, std::string const& source, std::string const& avform);
    void remotePause();
    void remoteStop();
    void remoteResume();
    void playPos(int newpos);

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

    IPlayer*                                  mPlayer;
    CMediaState*                              mMediaState;
    CMediaSourceClientProfileProxy*           mSourceClientProxy;
    CMediaSourceServerProfileProxy*           mSourceServerProxy;
    CMediaControlClientProfileProxy*          mControlClientProxy;
    CMediaControlServerProfileProxy*          mControlServerProxy;
    CMediaStreamingClientProfileProxy*        mStreamingClientProxy;
    CMediaStreamingServerProfileProxy*        mStreamingServerProxy;

    IGuiController*                           mGuiController;           //updates Gui on external requestes
    IMediaSource*                             mMediaSource;
    IServer*                                  mLocalServer;

    
    Direction                                 mDirection;
    bool                                      mIsInitByUser;            //
    int                                       mCurrPos;                 //current track pos in the map
    CRWMutex*                                 mMutex;

    static Logger                             msLogger;                 //Logger

#ifndef ANDROID
#else
    iviLink::Android::AppInfo                 mAppInfo;
#endif //ANDROID
};

#endif // CPLAYERLOGIC_H
