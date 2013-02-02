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


#include <iostream>
#include <cassert>
#include <cstdlib>
#include <sstream>

#include "PlayerLogic.hpp"
#include "Exit.hpp"

Logger PlayerLogic::msLogger = Logger::getInstance(
        LOG4CPLUS_TEXT("samples.Applications.Media.CPlayerLogic"));

PlayerLogic::PlayerLogic(iviLink::Android::AppInfo info, IGuiController * controller,
        IMediaSource * source, IPlayer * player, IServer * server)
: 
    mAppInfo(info), 
    mGuiController(controller), 
    mMediaSource(source), 
    mPlayer(player), 
    mLocalServer(server), 
    mIsInitByUser(false), 
    mMediaState(new CMediaState())
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    assert(mGuiController);
    assert(mMediaSource);
    assert(mPlayer);
    assert(mLocalServer);
    mControlServerProxy = new CMediaControlServerProfileProxy(iviLink::Service::Uid("MediaService"),
            mAppInfo);
    mControlClientProxy = new CMediaControlClientProfileProxy(iviLink::Service::Uid("MediaService"),
            mAppInfo);
    mSourceServerProxy = new CMediaSourceServerProfileProxy(iviLink::Service::Uid("MediaService"),
            mAppInfo);
    mSourceClientProxy = new CMediaSourceClientProfileProxy(iviLink::Service::Uid("MediaService"),
            mAppInfo);
    mStreamingServerProxy = new CMediaStreamingServerProfileProxy(
            iviLink::Service::Uid("MediaService"), mAppInfo);
    mStreamingClientProxy = new CMediaStreamingClientProfileProxy(
            iviLink::Service::Uid("MediaService"), mAppInfo);

    mMediaSource->obtainTrackList();
    updateListView();
    

    mGuiController->setSync();
    mGuiController->deactivateToggle();

    {
        StateLocker lock(mMediaState, true);
        mGuiController->playlistPosChanged(mMediaState->getCurrentPosition());
    }
    
    mPlayer->registerPlaybackStartedCallback(this);
}

PlayerLogic::~PlayerLogic()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    if (mPlayer)
        delete mPlayer;
    if (mLocalServer)
        delete mLocalServer;
    if (mMediaSource)
        delete mMediaSource;
    if (mGuiController)
        delete mGuiController;
    if (mMediaState)
        delete mMediaState;

}

void PlayerLogic::connection()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    mStreamingServerProxy->sendStreamingInfo();
}

void PlayerLogic::onServerToggle(bool needLock)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    StateLocker lock(mMediaState, needLock);
    remoteToggleUnsafe();
}

void PlayerLogic::onServerSync(bool needLock)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    StateLocker lock(mMediaState, needLock);
    remoteSyncUnsafe();
}

void PlayerLogic::onServerUnsync(bool needLock)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    StateLocker lock(mMediaState, needLock);
    remoteUnsyncUnsafe();
}

void PlayerLogic::onServerPlay(std::string const& trackName, std::string const& trackUid,
        std::string const& source, std::string const& avform, bool needLock)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    StateLocker lock(mMediaState, needLock);
    if (mMediaState->getConnectionState() == CONNECTED)
    {
        mMediaState->setRoleState(SERVER);
        remotePlayUnsafe(trackName, trackUid, source, avform);
    }
}

void PlayerLogic::onServerResume(bool needLock)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    StateLocker lock(mMediaState, needLock);
    if (mMediaState->getConnectionState() == CONNECTED)
    {
        mMediaState->setRoleState(SERVER);
        remoteResumeUnsafe();
    }
}

void PlayerLogic::onServerPause(bool needLock)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    StateLocker lock(mMediaState, needLock);
    if (mMediaState->getConnectionState() == CONNECTED)
    {
        mMediaState->setRoleState(SERVER);
        remotePauseUnsafe();
    }
}

void PlayerLogic::onServerStop(bool needLock)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    StateLocker lock(mMediaState, needLock);
    if (mMediaState->getConnectionState() == CONNECTED)
    {
        mMediaState->setRoleState(SERVER);
        remoteStopUnsafe();
    }
}

std::string PlayerLogic::onMediaPathRequest(bool needLock)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    return mMediaSource->getPathToFiles();
}

//from  conrtolClient
void PlayerLogic::onClientToggle(bool needLock)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    StateLocker lock(mMediaState, needLock);
    remoteToggleUnsafe();
}

void PlayerLogic::onClientSync(bool needLock)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    StateLocker lock(mMediaState, needLock);
    remoteSyncUnsafe();
}

void PlayerLogic::onClientUnsync(bool needLock)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    StateLocker lock(mMediaState, needLock);
    remoteUnsyncUnsafe();
}

void PlayerLogic::onClientPlay(std::string const& trackName, std::string const& trackUid,
        std::string const& source, std::string const& avform, bool needLock)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    StateLocker lock(mMediaState, needLock);
    if (mMediaState->getConnectionState() == CONNECTED)
    {
        mMediaState->setRoleState(CLIENT);
        remotePlayUnsafe(trackName, trackUid, source, avform);
    }
}

void PlayerLogic::onClientResume(bool needLock)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    StateLocker lock(mMediaState, needLock);
    if (mMediaState->getConnectionState() == CONNECTED)
    {
        mMediaState->setRoleState(CLIENT);
        remoteResumeUnsafe();
    }
}

void PlayerLogic::onClientPause(bool needLock)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    StateLocker lock(mMediaState, needLock);
    if (mMediaState->getConnectionState() == CONNECTED)
    {
        mMediaState->setRoleState(CLIENT);
        remotePauseUnsafe();
    }
}

void PlayerLogic::onClientStop(bool needLock)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    StateLocker lock(mMediaState, needLock);
    if (mMediaState->getConnectionState() == CONNECTED)
    {
        mMediaState->setRoleState(CLIENT);
        remoteStopUnsafe();
    }
}

//from sourceClient
void PlayerLogic::onRecallTrackList(bool needLock)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    StateLocker lock(mMediaState, needLock);
    mMediaSource->recallRemoteTrackList();
    updateListView();
    mMediaState->setConnectionState(DISCONNECTED);
}

void PlayerLogic::onTrackListReceived(std::string const& trackList, bool needLock)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    StateLocker lock(mMediaState, needLock);
    mMediaSource->setRemoteTrackList(trackList);
    mDirection.incoming = true;
    LOG4CPLUS_INFO(msLogger,
            convertIntegerToString(static_cast<int>((connections) (mDirection.outgoing))));
    if (mDirection.outgoing)
    {
        mMediaState->setConnectionState(CONNECTED);
        mPlayer->stop();
        mMediaState->setPlayBackState(STOP);
        mGuiController->showPlayButton();
        updateListView();
    } 
    else
    {
        mMediaState->setConnectionState(DISCONNECTED);
    }
}

void PlayerLogic::onTrackListChanged(bool needLock)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    mSourceClientProxy->requestTrackList();
}

//from sourceServer
void PlayerLogic::onRequestTrackList(bool needLock)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    std::string msg;
    mMediaSource->getLocalTrackList(msg);
    mSourceServerProxy->sendTrackList(msg);
}

//from streamingClient
void PlayerLogic::onStreamingClientAccepted(bool needLock)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
}

void PlayerLogic::onStreamingClientUnaccepted(bool needLock)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    StateLocker lock(mMediaState, needLock);
    mMediaSource->recallRemoteTrackList();
    mSourceServerProxy->recallTrackList();
    mDirection.incoming = false;
    mMediaState->setConnectionState(DISCONNECTED);
}

//from streamingServer
void PlayerLogic::onStreamingServerAccepted(bool needLock) //we have received from the client formats we are able to support, therefore server accepted
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    StateLocker lock(mMediaState, needLock);
    std::string list;
    mMediaSource->getLocalTrackList(list);
    mSourceServerProxy->sendTrackList(list);
    mDirection.outgoing = true;

    if (mDirection.incoming)
    {
        mMediaState->setConnectionState(CONNECTED);
        mPlayer->stop();
        mGuiController->showPlayButton();
        mMediaState->setCurrentPosition(0);
        mGuiController->playlistPosChanged(mMediaState->getCurrentPosition());
        updateListView();
    } 
    else
    {
        mMediaState->setConnectionState(DISCONNECTED);
    }
}

void PlayerLogic::onStreamingServerUnaccepted(bool needLock)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    StateLocker lock(mMediaState, needLock);
    mSourceServerProxy->recallTrackList();
    mMediaSource->recallRemoteTrackList();
    mDirection.outgoing = false;
    mMediaState->setConnectionState(DISCONNECTED);
}

void PlayerLogic::localStop()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    StateLocker lock(mMediaState, true);
    if (!mMediaSource->getTrackList().empty())
    {
        if (mMediaState->getConnectionState() == CONNECTED)
        {
            if (mMediaState->getStreamState() != OTHERSIDE)
            {
                mPlayer->stop();
            }

            if (mMediaState->getRoleState() == SERVER)
            {
                mControlServerProxy->stopStreaming();
            } 
            else if (mMediaState->getRoleState() == CLIENT)
            {
                mControlClientProxy->stop();
            }
        } 
        else if (mMediaState->getConnectionState() == DISCONNECTED)
        {
            mPlayer->stop();
            mLocalServer->stop();
        }
        mMediaState->setPlayBackState(STOP);
        mMediaState->setCurrentPosition(0);
        mGuiController->playlistPosChanged(0);
        mGuiController->showPlayButton();
        mGuiController->hideIcon();
    } 
    else
    {
        LOG4CPLUS_WARN(msLogger, "trackList is empty!");
    }
}

void PlayerLogic::localPause()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    StateLocker lock(mMediaState, true);
    localPauseUnsafe();
}

void PlayerLogic::localPosClicked(int newpos)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    StateLocker lock(mMediaState, true);
    if (newpos == mMediaState->getCurrentPosition())
    {
        if (mMediaState->getPlayBackState() == PLAY)
        {
            localPauseUnsafe();
        } 
        else if (mMediaState->getPlayBackState() == PAUSE)
        {
            localPlayUnsafe();
        } 
        else if (mMediaState->getPlayBackState() == STOP)
        {
            playPositionUnsafe(0);
        } 
    } 
    else
    {
        playPositionUnsafe(newpos);
    }
    mMediaState->setCurrentPosition(newpos);
    mGuiController->playlistPosChanged(newpos);
}

void PlayerLogic::localVolumeUp()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    mPlayer->volumeUp();
}

void PlayerLogic::localVolumeDown()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    mPlayer->volumeDown();
}

void PlayerLogic::localToggle()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    StateLocker lock(mMediaState, true);
    remoteToggleUnsafe();
    if(mMediaState->getConnectionState() == CONNECTED)
    {
        mControlClientProxy->toggle();        
    }
}

void PlayerLogic::onClientChangeHighlightedTrack(int newPosition, bool needLock)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    StateLocker lock(mMediaState, needLock);
    mMediaState->setCurrentPosition(newPosition);
    mGuiController->playlistPosChanged(newPosition);
}

void PlayerLogic::localSync()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    StateLocker lock(mMediaState, true);
    if (mMediaState->getConnectionState() == CONNECTED)
    {
        if (mMediaState->getStreamState() == MYSIDE)
        {
            mControlClientProxy->sync();
            mMediaState->setStreamState(BOTH);
            mGuiController->setSync();
            mGuiController->deactivateToggle();
        } 
        else if (mMediaState->getStreamState() == OTHERSIDE)
        {
            if (mMediaState->getPlayBackState() != STOP)
            {
                mPlayer->play(mMediaState->getSource(), mMediaState->getAVFormat());
                // if media is paused the player will be paused from the onPlaybackStarted callback
            }
            mControlClientProxy->sync();
            mMediaState->setStreamState(BOTH);
            mGuiController->setSync();
            mGuiController->deactivateToggle();
        } 
        else if (mMediaState->getStreamState() == BOTH)
        {
            if (mMediaState->getPlayBackState() != STOP)
            {
                mPlayer->stop();
            }
            mControlClientProxy->unsync();
            mMediaState->setStreamState(OTHERSIDE);
            mGuiController->setUnsync();
            mGuiController->activateToggle();
        }
    }
}

void PlayerLogic::localNext()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    StateLocker lock(mMediaState, true);
    localNextUnsafe();
}

void PlayerLogic::localPrev()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    StateLocker lock(mMediaState, true);
    if (!mMediaSource->getTrackList().empty())
    {
        std::string track;
        std::string form;
        std::string trackuid;
        std::string currTrackUid = mMediaState->getCurrTrack();
        int currentPosition = mMediaState->getCurrentPosition();
        bool isHolder = mMediaSource->getPrevTrack(track, form, trackuid, currentPosition);
        mMediaState->setCurrTrack(trackuid);
        if (currentPosition == 0)
        {
            mMediaState->setCurrentPosition(mMediaSource->getTrackList().size() - 1);
        } 
        else
        {
            mMediaState->setCurrentPosition(currentPosition - 1);
        }
        mGuiController->playlistPosChanged(mMediaState->getCurrentPosition());
        if (mMediaState->getConnectionState() == CONNECTED
                && mMediaState->getPlayBackState() != STOP)
        {
            mPlayer->stop();
            if (!isHolder)
            {
                mControlClientProxy->play(track, trackuid, form);
                if (mMediaState->getRoleState() == SERVER)
                {
                    mControlServerProxy->serverStop();
                }
                mMediaState->setRoleState(CLIENT);
            } 
            else
            {
                mControlServerProxy->play(track, trackuid, form, false);
                if (mMediaState->getRoleState() == CLIENT)
                {
                    mControlClientProxy->serverStop();
                }
                mMediaState->setRoleState(SERVER);
            }
        } 
        else if (mMediaState->getConnectionState() == CONNECTED
                    && mMediaState->getPlayBackState() == STOP)
        {
            mControlServerProxy->changeHighlightedTrack(mMediaState->getCurrentPosition());
        } 
        else if (mMediaState->getConnectionState() == DISCONNECTED
                        && mMediaState->getPlayBackState() != STOP) 
        {
            mLocalServer->stop();
            mLocalServer->play(mMediaSource->getPathToFiles() + track, form);
            mPlayer->play(mLocalServer->getSource(), form);
            mMediaState->setPlayBackState(PLAY);
            mGuiController->showPauseButton();
            mGuiController->showPlayIcon();
        }
        // TODO disconnected && stopped state
    } 
    else
    {
        LOG4CPLUS_WARN(msLogger, "trackList is empty!");
    }
}

void PlayerLogic::localPlay()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    StateLocker lock(mMediaState, true);
    localPlayUnsafe();
}

void PlayerLogic::localExit()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    localStop();
    killProcess();
}

void PlayerLogic::updateListView()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    commonTrackListMap trackList = mMediaSource->getTrackList();
    commonTrackListMap::iterator iter = trackList.begin();
    commonTrackListMap::iterator endIter = trackList.end();
    mGuiController->playlistClear();
    while (iter != endIter)
    {
        LOG4CPLUS_TRACE(msLogger, "track: " + iter->first);
        mGuiController->playlistPopulate(iter->second.trackName, iter->second.owner,
                mMediaSource->isAudio(iter->second.trackName));
        ++iter;
    }
}

void PlayerLogic::initByUser()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    LOG4CPLUS_TRACE(msLogger, "mIsInitByUser = true");
    mIsInitByUser = true;
}

void PlayerLogic::initByLauncher()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    LOG4CPLUS_TRACE(msLogger, "mIsInitByUser = false");
    mIsInitByUser = false;
}

void PlayerLogic::startVlcServer(int pipeReadDescriptor, bool needLock)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    mMediaSource->startVlcServer(pipeReadDescriptor);
}

// is unsafe - for ubuntu only
void PlayerLogic::onPlaybackStarted()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    if(mMediaState->getPlayBackState() == PAUSE) 
    {
        mPlayer->pause();
    }
    if(mMediaState->getPlayBackState() == STOP)
    {
        mPlayer->stop();
    }
}

// for android - as it invoked on a spawned thread
void PlayerLogic::onPlaybackStartedSafe()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    StateLocker lock(mMediaState, true);
    onPlaybackStarted();
}

// ubuntu-special
void PlayerLogic::obtainPlayerState()
{
    mGuiController->playlistPosChanged(mMediaState->getCurrentPosition());
    //there is the freaky expression, but it seems it works properly
    if (mPlayer->isTrackEnded()
            && (mMediaState->getRoleState() == SERVER
                    || (mMediaState->getRoleState() == CLIENT
                            && mMediaState->getStreamState() == MYSIDE)))
    {
        LOG4CPLUS_INFO(msLogger, "I am server and track's ended");
        localNext();
        return;
    }
    if (mPlayer->isTrackInErrorState() && mMediaState->getPlayBackState() == PLAY)
    {
        LOG4CPLUS_INFO(msLogger,
                "obtainPlayerState:: mPlayer->isTrackInErrorState() && mMediaState->getPlayBackState() == PLAY");
        if (mMediaState->getConnectionState() == CONNECTED)
        {
            LOG4CPLUS_INFO(msLogger,
                    "obtainPlayerState:: mPlayer->isTrackInErrorState() && mMediaState->getPlayBackState() == PLAY :: CONNECTED");
            mPlayer->play(mMediaState->getSource(), mMediaState->getAVFormat());
        } 
        else
        {
            mPlayer->play(mLocalServer->getSource(), mMediaState->getAVFormat());
        }
    }
}

// unprotected methods
void PlayerLogic::remoteSyncUnsafe()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    if (mMediaState->getConnectionState() == CONNECTED)
    {
        //switch to mode BOTH
        if (mMediaState->getStreamState() == OTHERSIDE)
        {
            if (mMediaState->getPlayBackState() != STOP)
            {
                mPlayer->play(mMediaState->getSource(), mMediaState->getAVFormat());
                // if it should be paused we will pause it from the callback onPlaybackStarted
            }
        }
        mMediaState->setStreamState(BOTH);
        mGuiController->setSync();
        mGuiController->deactivateToggle();
    }
}

void PlayerLogic::remoteUnsyncUnsafe()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    if (mMediaState->getConnectionState() == CONNECTED)
    {
        //swith to mode myside
        mMediaState->setStreamState(MYSIDE);
        mGuiController->setUnsync();
        mGuiController->highlightToggle();
    }
}

void PlayerLogic::remoteToggleUnsafe()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    if (mMediaState->getConnectionState() == CONNECTED)
    {
        if (mMediaState->getStreamState() == MYSIDE)
        {
            mGuiController->activateToggle();
            mMediaState->setStreamState(OTHERSIDE);

            if (mMediaState->getPlayBackState() != STOP)
            {
                mPlayer->stop();
            }
        } 
        else if (mMediaState->getStreamState() == OTHERSIDE)
        {
            mMediaState->setStreamState(MYSIDE);
            mGuiController->highlightToggle();
            if (mMediaState->getPlayBackState() != STOP)
            {
                mPlayer->play(mMediaState->getSource(), mMediaState->getAVFormat());
                // if it should be paused we will pause it from the callback onPlaybackStarted
            }
        }
    }
}

void PlayerLogic::remotePlayUnsafe(std::string const& trackName, std::string const& trackUid,
        std::string const& source, std::string const& avform)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    LOG4CPLUS_INFO(msLogger, "source: " + source + "avform: " + avform + "trackUid: " + trackUid);
    if (mMediaState->getStreamState() != OTHERSIDE)
    {
        mPlayer->play(source, avform);
    }

    mMediaState->setSource(source);
    mMediaState->setAVFormat(avform);
    mMediaState->setCurrTrack(trackUid);
    mMediaState->setPlayBackState(PLAY);
    mMediaState->setCurrentPosition(mMediaSource->getPosition(trackUid));

    mGuiController->playlistPosChanged(mMediaState->getCurrentPosition());
    mGuiController->showPauseButton();
    mGuiController->showPlayIcon();
}

void PlayerLogic::remoteResumeUnsafe()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    if (mMediaState->getStreamState() != OTHERSIDE)
    {
        if(mMediaState->getPlayBackState() == PAUSE)
        {
            mPlayer->resume();
        }
    }
    mMediaState->setPlayBackState(PLAY);
    mGuiController->showPauseButton();
    mGuiController->showPlayIcon();
}

void PlayerLogic::remotePauseUnsafe()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    if (mMediaState->getStreamState() != OTHERSIDE)
    {
        if (mMediaState->getPlayBackState() == PLAY)
        {
            mPlayer->pause();
        }
    }
    mMediaState->setPlayBackState(PAUSE);
    mGuiController->showPlayButton();
    mGuiController->showPauseIcon();
}

void PlayerLogic::remoteStopUnsafe()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    if (mMediaState->getStreamState() != OTHERSIDE)
    {
        mPlayer->stop();
    }
    mMediaState->setPlayBackState(STOP);
    mMediaState->setCurrentPosition(0);

    mGuiController->playlistPosChanged(0);
    mGuiController->showPlayButton();
    mGuiController->hideIcon();
}

void PlayerLogic::localPlayUnsafe()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    if (!mMediaSource->getTrackList().empty())
    {
        std::string track;
        std::string form;
        std::string trackuid;
        bool isHolder = mMediaSource->getInfoAboutTrack(mMediaState->getCurrentPosition(), track, form, trackuid);
        if (mMediaState->getConnectionState() == CONNECTED)
        {
            if (mMediaState->getPlayBackState() == PAUSE)
            {
                if (mMediaState->getRoleState() == SERVER)
                {
                    mControlServerProxy->resume(false);
                } 
                else if (mMediaState->getRoleState() == CLIENT)
                {
                    mControlClientProxy->resume();
                }
            } 
            else if (mMediaState->getPlayBackState() == STOP)
            {
                mMediaState->setCurrTrack(trackuid);
                if (isHolder == false)
                {
                    mControlClientProxy->play(track, trackuid, form);
                    mMediaState->setRoleState(CLIENT);
                } 
                else
                {
                    mControlServerProxy->play(track, trackuid, form, false);
                    mMediaState->setRoleState(SERVER);
                }
            }
        } 
        else
        {
            if (mMediaState->getConnectionState() == DISCONNECTED)
            {
                if (mMediaState->getPlayBackState() == PAUSE)
                {
                    mLocalServer->resume();
                    mPlayer->resume();
                } 
                else if (mMediaState->getPlayBackState() == STOP)
                {
                    mMediaState->setCurrTrack(trackuid);
                    mLocalServer->stop();
                    mLocalServer->play(mMediaSource->getPathToFiles() + track, form);
                    mPlayer->play(mLocalServer->getSource(), form);
                }
                mMediaState->setPlayBackState(PLAY);
                mGuiController->showPauseButton();
                mGuiController->showPlayIcon();
            }
        }
    } 
    else
    {
        LOG4CPLUS_WARN(msLogger, "trackList is empty!");
    }
}

void PlayerLogic::localPauseUnsafe()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    if (!mMediaSource->getTrackList().empty())
    {
        mMediaState->setPlayBackState(PAUSE);
        mGuiController->showPlayButton();
        if (mMediaState->getConnectionState() == CONNECTED)
        {
            if (mMediaState->getStreamState() != OTHERSIDE)
            {
                mPlayer->pause();
            }

            if (mMediaState->getRoleState() == SERVER)
            {
                mControlServerProxy->pause();
            } 
            else if (mMediaState->getRoleState() == CLIENT)
            {
                mControlClientProxy->pause();
            }
        }
        else if (mMediaState->getConnectionState() == DISCONNECTED)
        {
            mPlayer->pause();
            mLocalServer->pause();
        }
        mGuiController->showPauseIcon();
    } 
    else
    {
        LOG4CPLUS_WARN(msLogger, "trackList is empty!");
    }
}

void PlayerLogic::playPositionUnsafe(int newpos)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    std::string track;
    std::string form;
    std::string trackuid;
    bool isHolder = mMediaSource->getInfoAboutTrack(newpos, track, form, trackuid);
    if (mMediaState->getConnectionState() == CONNECTED)
    {
        mPlayer->stop();
        if (isHolder == false)
        {
            mControlClientProxy->play(track, trackuid, form);
            if (mMediaState->getRoleState() == SERVER)
            {
                mControlServerProxy->serverStop();
            }
            mMediaState->setRoleState(CLIENT);
        } 
        else
        {
            mControlServerProxy->play(track, trackuid, form, false);
            if (mMediaState->getRoleState() == CLIENT)
            {
                mControlClientProxy->serverStop();
            }
            mMediaState->setRoleState(SERVER);
        }
    } 
    else if (mMediaState->getConnectionState() == DISCONNECTED)
    {
        mMediaState->setCurrTrack(trackuid);
        mLocalServer->stop();
        mLocalServer->play(mMediaSource->getPathToFiles() + track, form);
        mPlayer->play(mLocalServer->getSource(), form);
        mMediaState->setPlayBackState(PLAY);
        mGuiController->showPauseButton();
        mGuiController->showPlayIcon();
    }
}

void PlayerLogic::localNextUnsafe()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    if (!mMediaSource->getTrackList().empty())
    {
        std::string track;
        std::string form;
        std::string trackuid;
        std::string currTrackUid = mMediaState->getCurrTrack();
        int currentPosition = mMediaState->getCurrentPosition();
        bool isHolder = mMediaSource->getNextTrack(track, form, trackuid, currentPosition);

        mMediaState->setCurrTrack(trackuid);
        if (currentPosition == (mMediaSource->getTrackList().size() - 1))
        {
            mMediaState->setCurrentPosition(0);
        } 
        else
        {
            mMediaState->setCurrentPosition(currentPosition + 1);
        }
        mGuiController->playlistPosChanged(mMediaState->getCurrentPosition());
        if (mMediaState->getConnectionState() == CONNECTED
                && mMediaState->getPlayBackState() != STOP)
        {
            mPlayer->stop();

            if (!isHolder)
            {
                mControlClientProxy->play(track, trackuid, form);
                if (mMediaState->getRoleState() == SERVER)
                {
                    mControlServerProxy->serverStop();
                }
                mMediaState->setRoleState(CLIENT);
            } 
            else
            {
                mControlServerProxy->play(track, trackuid, form, false);
                if (mMediaState->getRoleState() == CLIENT)
                {
                    mControlClientProxy->serverStop();
                }
                mMediaState->setRoleState(SERVER);
            }
        } 
        else if (mMediaState->getConnectionState() == CONNECTED
                    && mMediaState->getPlayBackState() == STOP)
        {
            mControlServerProxy->changeHighlightedTrack(mMediaState->getCurrentPosition());
        } 
        else if (mMediaState->getConnectionState() == DISCONNECTED
                        && mMediaState->getPlayBackState() != STOP)
        {
            mLocalServer->stop();
            mLocalServer->play(mMediaSource->getPathToFiles() + track, form);
            mPlayer->play(mLocalServer->getSource(), form);
            mMediaState->setPlayBackState(PLAY);
            mGuiController->showPauseButton();
            mGuiController->showPlayIcon();
        }
        // TODO (disconnected && stopped) case
    } 
    else
    {
        LOG4CPLUS_WARN(msLogger, "trackList is empty!");
    }
}

