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
 

#include <iostream>
#include <cassert>
#include <cstdlib>
#include <sstream>

#include "CPlayerLogic.h"
#include "Exit.hpp"

Logger CPlayerLogic::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("samples.Applications.Media.CPlayerLogic"));

#ifndef ANDROID
CPlayerLogic::CPlayerLogic( IGuiController * controller,
                            IMediaSource * source,
                            IPlayer * player,
                            IServer * server)
:
#else
CPlayerLogic::CPlayerLogic( iviLink::Android::AppInfo info,
                            IGuiController * controller,
                            IMediaSource * source,
                            IPlayer * player,
                            IServer * server)
:
    mAppInfo(info),
#endif //ANDROID
    mGuiController(controller),
    mMediaSource(source),
    mPlayer(player),
    mLocalServer(server),
    mIsInitByUser(false),
    mCurrPos(0),
    mMutex(new CRWMutex()),
    mMediaState(new CMediaState())
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    assert(mGuiController);
    assert(mMediaSource);
    assert(mPlayer);
    assert(mLocalServer);
#ifndef ANDROID
    mControlServerProxy = new CMediaControlServerProfileProxy(iviLink::Service::Uid("MediaService"));
    mControlClientProxy = new CMediaControlClientProfileProxy(iviLink::Service::Uid("MediaService"));
    mSourceServerProxy = new CMediaSourceServerProfileProxy(iviLink::Service::Uid("MediaService"));
    mSourceClientProxy = new CMediaSourceClientProfileProxy(iviLink::Service::Uid("MediaService"));
    mStreamingServerProxy = new CMediaStreamingServerProfileProxy(iviLink::Service::Uid("MediaService"));
    mStreamingClientProxy = new CMediaStreamingClientProfileProxy(iviLink::Service::Uid("MediaService"));
#else
    mControlServerProxy = new CMediaControlServerProfileProxy(iviLink::Service::Uid("MediaService"), mAppInfo);
    mControlClientProxy = new CMediaControlClientProfileProxy(iviLink::Service::Uid("MediaService"), mAppInfo);
    mSourceServerProxy = new CMediaSourceServerProfileProxy(iviLink::Service::Uid("MediaService"), mAppInfo);
    mSourceClientProxy = new CMediaSourceClientProfileProxy(iviLink::Service::Uid("MediaService"), mAppInfo);
    mStreamingServerProxy = new CMediaStreamingServerProfileProxy(iviLink::Service::Uid("MediaService"), mAppInfo);
    mStreamingClientProxy = new CMediaStreamingClientProfileProxy(iviLink::Service::Uid("MediaService"), mAppInfo);
#endif //ANDROID

    mMediaSource->obtainTrackList();   
    updateListView();
    if (mMediaState->getStreamState() == BOTH)
    {
	    mGuiController->setSync();
        mGuiController->deactivateToggle();
    }
    else if (mMediaState->getStreamState() == OTHERSIDE)
    {
	    mGuiController->setUnsync();
	    mGuiController->activateToggle();
    }
    else if (mMediaState->getStreamState() == MYSIDE)
    {
	    mGuiController->setUnsync();
	    mGuiController->highlightToggle();
    }
    mGuiController->playlistPosChanged(mCurrPos);
}

CPlayerLogic::~CPlayerLogic()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    if (mPlayer)
    	delete mPlayer;
    if (mLocalServer)
    	delete mLocalServer;
    if (mMediaSource)
    	delete mMediaSource;
    if (mMutex)
   	 delete mMutex;
    if (mGuiController)
    	delete mGuiController;
    if (mMediaState)
    	delete mMediaState;
    
}

void CPlayerLogic::connection()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    mStreamingServerProxy->sendStreamingInfo();
}

void CPlayerLogic::onServerToggle()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    if (mMediaState->getConnectionState() == CONNECTED)
    {
        if (mMediaState->getStreamState() == MYSIDE)
        {
            mGuiController->activateToggle();
            mMediaState->setStreamState(OTHERSIDE);

            if (mMediaState->getPlayBackState() == PLAY)
            {          
                mPlayer->pause();
            }
        }
        else if (mMediaState->getStreamState() == OTHERSIDE)
        {
            mMediaState->setStreamState(MYSIDE);
            mGuiController->highlightToggle();
            if (mMediaState->getPlayBackState() == PLAY)
            {
                if (mPlayer->play(mMediaState->getSource(), mMediaState->getAVFormat()) == ERROR_PLAY)
                {
                    return;
                }
            }

        }
    }
}

void CPlayerLogic::onServerSync()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    if (mMediaState->getConnectionState() == CONNECTED)
    {
        //switch to mode BOTH
	    if (mMediaState->getStreamState() == OTHERSIDE)
        {
            if (mMediaState->getPlayBackState() == PLAY)
            {
                if (mPlayer->play(mMediaState->getSource(), mMediaState->getAVFormat()) == ERROR_PLAY)
                {
                    return;
                }
            }
        }
        mMediaState->setStreamState(BOTH);
        mGuiController->setSync();
        mGuiController->deactivateToggle();
    }
}

void CPlayerLogic::onServerUnsync()
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

void CPlayerLogic::onServerPlay(std::string  const& trackName, std::string  const& trackUid, std::string const& source, std::string const& avform)
{

    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    if (mMediaState->getConnectionState() == CONNECTED)
    {
        mMediaState->setRoleState(SERVER);
        remotePlay(trackName,trackUid, source, avform);
    }
}

void CPlayerLogic::onServerResume()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    if (mMediaState->getConnectionState() == CONNECTED)
    {
        mMediaState->setRoleState(SERVER);
        remoteResume();
    }
}

void CPlayerLogic::onServerPause()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    if (mMediaState->getConnectionState() == CONNECTED)
    {
        mMediaState->setRoleState(SERVER);
        remotePause();
    }
}

void CPlayerLogic::onServerStop()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    if (mMediaState->getConnectionState() == CONNECTED)
    {
        mMediaState->setRoleState(SERVER);
        remoteStop();
    }
}

std::string CPlayerLogic::onMediaPathRequest()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    return mMediaSource->getPathToFiles();
}

//from  conrtolClient
void CPlayerLogic::onClientToggle()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    if (mMediaState->getConnectionState() == CONNECTED)
    {
        onServerToggle();
    }
}

void CPlayerLogic::onClientSync()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    if (mMediaState->getConnectionState() == CONNECTED)
    {
        onServerSync();
    }
}

void CPlayerLogic::onClientUnsync()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    if (mMediaState->getConnectionState() == CONNECTED)
    {
        onServerUnsync();
    }
}

 void CPlayerLogic::onClientPlay(std::string  const& trackName, std::string  const& trackUid, std::string const& source, std::string const& avform)
{

    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    if (mMediaState->getConnectionState() == CONNECTED)
    {
        mMediaState->setRoleState(CLIENT);
        remotePlay(trackName,trackUid, source, avform);
    }
}

void CPlayerLogic::onClientResume()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    if (mMediaState->getConnectionState() == CONNECTED)
    {
        mMediaState->setRoleState(CLIENT);
        remoteResume();
    }
}

void CPlayerLogic::onClientPause()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    if (mMediaState->getConnectionState() == CONNECTED)
    {
        mMediaState->setRoleState(CLIENT);
        remotePause();
    }
}

void CPlayerLogic::onClientStop()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    if (mMediaState->getConnectionState() == CONNECTED)
    {
        mMediaState->setRoleState(CLIENT);
        remoteStop();
    }
}

//from sourceClient
void CPlayerLogic::onRecallTrackList()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    mMediaSource->recallRemoteTrackList();
    updateListView();
    mMediaState->setConnectionState(DISCONNECTED);
}

void CPlayerLogic::onTrackListReceived(std::string const& trackList)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    std::string rTL = trackList;
    mMediaSource->setRemoteTrackList(rTL);
    mDirection.incoming = true;
    LOG4CPLUS_INFO(msLogger, convertIntegerToString(static_cast<int>((connections)(mDirection.outgoing))));

    if(mDirection.outgoing)
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

void CPlayerLogic::onTrackListChanged()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    mSourceClientProxy->requestTrackList();
}

//from sourceServer
void CPlayerLogic::onRequestTrackList()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    std::string msg;
    mMediaSource->getLocalTrackList(msg);
    mSourceServerProxy->sendTrackList(msg);
}

//from streamingClient
 void CPlayerLogic::onStreamingClientAccepted()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
}

 void CPlayerLogic::onStreamingClientUnaccepted()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    mMediaSource->recallRemoteTrackList();
    mSourceServerProxy->recallTrackList();
    mDirection.incoming = false;
    mMediaState->setConnectionState(DISCONNECTED);
}

//from streamingServer
void CPlayerLogic::onStreamingServerAccepted()      //we have received from the client formats we are able to support, therefore server accepted
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    std::string list;
    mMediaSource->getLocalTrackList(list);
    mSourceServerProxy->sendTrackList(list);
    mDirection.outgoing = true;
    
    if(mDirection.incoming)
    {
        mMediaState->setConnectionState(CONNECTED);
        mPlayer->stop();
        mGuiController->showPlayButton();
        mCurrPos = 0;
        mGuiController->playlistPosChanged(mCurrPos);
        updateListView();
    }
    else
    {
        mMediaState->setConnectionState(DISCONNECTED);        
    }
}

void CPlayerLogic::onStreamingServerUnaccepted()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    mSourceServerProxy->recallTrackList();
    mMediaSource->recallRemoteTrackList();
    mDirection.outgoing = false;
    mMediaState->setConnectionState(DISCONNECTED);
}

void CPlayerLogic::remotePlay(std::string const& trackName, std::string const& trackUid, std::string const& source, std::string const& avform)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    LOG4CPLUS_INFO(msLogger,  "source: " + source + "avform: "  + avform + "trackUid: " + trackUid);
    if (mMediaState->getStreamState() == MYSIDE || mMediaState->getStreamState() == BOTH)
    {
        mPlayer->play(source, avform);
    }

    mMediaState->setSource(source);
    mMediaState->setAVFormat(avform);
    mMediaState->setCurrTrack(trackUid);
    mMediaState->setPlayBackState(PLAY);
    mCurrPos = mMediaSource->getPosition(trackUid);
    std::string s;
    std::stringstream out;
    out << mCurrPos;
    s = out.str();

    mGuiController->playlistPosChanged(mCurrPos);
    mGuiController->showPauseButton();
    mGuiController->showPlayIcon();
}

void CPlayerLogic::remoteStop()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    mPlayer->stop();
    mMediaState->setPlayBackState(STOP);
    mMutex->lockWrite();
    mCurrPos = 0;
    mMutex->unlockWrite();
    mGuiController->playlistPosChanged(mCurrPos);
    mGuiController->showPlayButton();
    mGuiController->hideIcon();
}

void CPlayerLogic::remotePause()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    if (mMediaState->getPlayBackState() == PLAY)
    {
        mPlayer->pause();
    }
    mMediaState->setPlayBackState(PAUSE);
    mGuiController->showPlayButton();
    mGuiController->showPauseIcon();
}

void CPlayerLogic::remoteResume()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    if (mMediaState->getStreamState() == MYSIDE || mMediaState->getStreamState() == BOTH)
    {
        if (mPlayer->resume() == ERROR_PLAY)
        {
            LOG4CPLUS_WARN(msLogger, "mPlayer->resume() == ERROR_PLAY");
        }       
    }
    mMediaState->setPlayBackState(PLAY);
    mGuiController->showPauseButton();
    mGuiController->showPlayIcon();
}

void CPlayerLogic::localStop()
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
   if (!mMediaSource->getTrackList().empty())
   {
	    if (mMediaState->getConnectionState() == CONNECTED)
	    {
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
		  mLocalServer->stop();
	    }
        mPlayer->stop();
        mMediaState->setPlayBackState(STOP);
	    mMutex->lockWrite();
	    mCurrPos = 0;
	    mMutex->unlockWrite();
	    mGuiController->playlistPosChanged(mCurrPos);
	    mGuiController->showPlayButton();
        mGuiController->hideIcon();
   }
   else 
   {
        LOG4CPLUS_WARN(msLogger, "trackList is empty!");
   }
}

void CPlayerLogic::localPause()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    if (!mMediaSource->getTrackList().empty())
    {
        mPlayer->pause();
	    mMediaState->setPlayBackState(PAUSE);
	    mGuiController->showPlayButton();
	    if (mMediaState->getConnectionState() == CONNECTED)
	    {
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
		    mLocalServer->pause();
	    }
        mGuiController->showPauseIcon();
   }
   else 
   {
        LOG4CPLUS_WARN(msLogger, "trackList is empty!");
   }
}

void CPlayerLogic::localPosClicked(int newpos)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    if (newpos == mCurrPos)
    {      
         if (mMediaState->getPlayBackState() == PLAY)
         {
	        localPause();
         }
         else if (mMediaState->getPlayBackState() == PAUSE)
         {
             localPlay();
         }
         else if (mMediaState->getPlayBackState() == STOP)
         {
            playPos(0);
         }
    }
    else
    {
	   playPos(newpos);
    }
    mMutex->lockWrite();
    mCurrPos = newpos;
    mMutex->unlockWrite(); 
    mGuiController->playlistPosChanged(mCurrPos);
}

void CPlayerLogic::localVolumeUp()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    mPlayer->volumeUp();
}

void CPlayerLogic::localVolumeDown()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    mPlayer->volumeDown();
}

void CPlayerLogic::localToggle()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    if (mMediaState->getConnectionState() == CONNECTED)
    {
        if (mMediaState->getStreamState() == MYSIDE)
        {
            if (mMediaState->getPlayBackState() == PLAY)
            {
                mPlayer->pause();
            }
            mMediaState->setStreamState(OTHERSIDE);
            mGuiController->activateToggle();
        }
        else if (mMediaState->getStreamState() == OTHERSIDE)
        {
            if (mMediaState->getPlayBackState() == PLAY)
            {
                mPlayer->play(mMediaState->getSource(), mMediaState->getAVFormat());
            }
        
            mMediaState->setStreamState(MYSIDE);
            mGuiController->highlightToggle();
        }
        mControlClientProxy->toggle();
    }
}

void CPlayerLogic::onClientChangeHighlightedTrack(int newPosition)
{
        LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
        mCurrPos = newPosition;
        mGuiController->playlistPosChanged(mCurrPos);
}

void CPlayerLogic::localSync()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
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
            if (mMediaState->getPlayBackState() == PLAY)
            {
                mPlayer->play(mMediaState->getSource(), mMediaState->getAVFormat());
            }
            mControlClientProxy->sync();
            mMediaState->setStreamState(BOTH);
            mGuiController->setSync();
            mGuiController->deactivateToggle();
        }
        else if (mMediaState->getStreamState() == BOTH)
        {
            if (mMediaState->getPlayBackState() == PLAY)
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

void CPlayerLogic::localNext()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    if (!mMediaSource->getTrackList().empty())
    {
	    std::string track;
	    std::string form;
        std::string trackuid;
	    std::string currTrackUid = mMediaState->getCurrTrack();
	    bool isHolder =  mMediaSource->getNextTrack(track, form, trackuid, mCurrPos);

	    mMediaState->setCurrTrack(trackuid);
	    mMutex->lockWrite();
	    if (mCurrPos == (mMediaSource->getTrackList().size() - 1))
	    {
		   mCurrPos = 0;
	    }
	    else ++mCurrPos;
	    mGuiController->playlistPosChanged(mCurrPos);
	    mMutex->unlockWrite();    
	    if (mMediaState->getConnectionState() == CONNECTED && mMediaState->getPlayBackState() != STOP)
	    {
		    mPlayer->stop();

		    if(isHolder == false)
		    {

		        mControlClientProxy->play(track,trackuid, form);
		        if (mMediaState->getRoleState() == SERVER)
		        {
		            mControlServerProxy->serverStop();
		        }
		        mMediaState->setRoleState(CLIENT);
		    }
		    else if (isHolder == true)
		    {

		        mControlServerProxy->play(track, trackuid, form);
		        if (mMediaState->getRoleState() == CLIENT)
		        {
		            mControlClientProxy->serverStop();
		        }
		        mMediaState->setRoleState(SERVER);
		    }
	    }
        else if (mMediaState->getConnectionState() == CONNECTED && mMediaState->getPlayBackState() == STOP)
	    {
            mControlServerProxy->changeHighlightedTrack(mCurrPos);
        }
	    else if (mMediaState->getConnectionState() == DISCONNECTED && mMediaState->getPlayBackState() != STOP)
	    {
             mLocalServer->stop();
		     mLocalServer->play(mMediaSource->getPathToFiles() + track, form);
		     mPlayer->play(mLocalServer->getSource(), form);
		     mMediaState->setPlayBackState(PLAY);
		     mGuiController->showPauseButton();
             mGuiController->showPlayIcon();
	    }
   }
   else 
   {
        LOG4CPLUS_WARN(msLogger, "trackList is empty!");
   }
}

void CPlayerLogic::localPrev()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    if (!mMediaSource->getTrackList().empty())
    {
	    std::string track;
	    std::string form;
        std::string trackuid;
	    std::string currTrackUid = mMediaState->getCurrTrack();
	    bool isHolder =  mMediaSource->getPrevTrack(track, form,trackuid, mCurrPos);
	    mMediaState->setCurrTrack(trackuid);
	    mMutex->lockWrite();
	    if (mCurrPos == 0)
	    {
	 	  mCurrPos = (mMediaSource->getTrackList().size() - 1);
	    }
	    else --mCurrPos;
	    mGuiController->playlistPosChanged(mCurrPos);
	    mMutex->unlockWrite();
	    if (mMediaState->getConnectionState() == CONNECTED && mMediaState->getPlayBackState() != STOP)
	    {
		    mPlayer->stop();
		    if(isHolder == false)
		    {
		        mControlClientProxy->play(track,trackuid, form);
		        if (mMediaState->getRoleState() == SERVER)
		        {
		            mControlServerProxy->serverStop();
		        }
		        mMediaState->setRoleState(CLIENT);
		    }
		    else if (isHolder == true)
		    {
		        mControlServerProxy->play(track,trackuid, form);
		        if (mMediaState->getRoleState() == CLIENT)
		        {
		            mControlClientProxy->serverStop();
		        }
		        mMediaState->setRoleState(SERVER);
		    }
	    }
        else if (mMediaState->getConnectionState() == CONNECTED && mMediaState->getPlayBackState() == STOP)
	    {
            mControlServerProxy->changeHighlightedTrack(mCurrPos);
        }
	    else if (mMediaState->getConnectionState() == DISCONNECTED && mMediaState->getPlayBackState() != STOP)
	    {
            mLocalServer->stop();
		    mLocalServer->play(mMediaSource->getPathToFiles() + track, form);
		    mPlayer->play(mLocalServer->getSource(), form);
		    mMediaState->setPlayBackState(PLAY);
		    mGuiController->showPauseButton();
            mGuiController->showPlayIcon();
	    }
   }
   else 
   {
        LOG4CPLUS_WARN(msLogger, "trackList is empty!");
   }
}

void CPlayerLogic::localPlay()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    if (!mMediaSource->getTrackList().empty())
    {
	    std::string track;
	    std::string form;
        std::string trackuid;
        bool isHolder = mMediaSource->getInfoAboutTrack(mCurrPos,track, form,trackuid);
	    if (mMediaState->getConnectionState() == CONNECTED)
	    {
		    if(mMediaState->getPlayBackState() == PAUSE)
		    {
		        if (mMediaState->getRoleState() == SERVER)
		        {
		            mControlServerProxy->resume();
		        }
		        else if (mMediaState->getRoleState() == CLIENT)
		        {
		            mControlClientProxy->resume();
		        }
		    }
		    else if(mMediaState->getPlayBackState() == STOP)
		    {	  
		        mMediaState->setCurrTrack(trackuid);
		        if (isHolder == false)
		        {
		            mControlClientProxy->play(track,trackuid, form);
		            mMediaState->setRoleState(CLIENT);
		        }
		        else if (isHolder == true)
		        {
		            mControlServerProxy->play(track,trackuid, form);
		            mMediaState->setRoleState(SERVER);
		        }
		    }
	    }
	    else if (mMediaState->getConnectionState() == DISCONNECTED)
	    {
            if(mMediaState->getPlayBackState() == PAUSE)
		    {
		        mLocalServer->resume();
		        if (mPlayer->resume() == ERROR_PLAY)
			    {
				    return;
			    }
		    }
		    else if(mMediaState->getPlayBackState() == STOP)
		    {
		        mMediaState->setCurrTrack(trackuid);
                mLocalServer->stop();
		        mLocalServer->play(mMediaSource->getPathToFiles() + track, form);
		        if (mPlayer->play(mLocalServer->getSource(), form) == ERROR_PLAY)
			    {
				    return;
			    }
		    }
		    mMediaState->setPlayBackState(PLAY);
		    mGuiController->showPauseButton();
            mGuiController->showPlayIcon();
	    }
   }
   else 
   {
        LOG4CPLUS_WARN(msLogger, "trackList is empty!");
   }
}

void CPlayerLogic::localExit()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    localStop();
    killProcess();
}

void CPlayerLogic::playPos(int newpos)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    std::string track;
    std::string form;
    std::string trackuid;
    bool isHolder =  mMediaSource->getInfoAboutTrack(newpos, track, form, trackuid);
    if (mMediaState->getConnectionState() == CONNECTED)
    {
        mPlayer->stop();
        if(isHolder == false)
        {
            mControlClientProxy->play(track,trackuid, form);
            if (mMediaState->getRoleState() == SERVER)
            {
                mControlServerProxy->serverStop();
            }
            mMediaState->setRoleState(CLIENT);
        }
        else if (isHolder == true)
        {
            mControlServerProxy->play(track,trackuid, form);
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


void CPlayerLogic::updateListView()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    commonTrackListMap trackList = mMediaSource->getTrackList();
    commonTrackListMap::iterator iter = trackList.begin();
    commonTrackListMap::iterator endIter = trackList.end();
    mGuiController->audioListClear();
    while(iter != endIter) 
    {
        LOG4CPLUS_TRACE(msLogger, "track: " + iter->first);
        mGuiController->audioListPopulate(iter->second.trackName, iter->second.owner);
        ++iter;
    }
}

void CPlayerLogic::initByUser()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    LOG4CPLUS_TRACE(msLogger, "mIsInitByUser = true");
    mIsInitByUser = true;
}

void CPlayerLogic::initByLauncher()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    LOG4CPLUS_TRACE(msLogger, "mIsInitByUser = false");
    mIsInitByUser = false;
}

void CPlayerLogic::startVlcServer(int pipeReadDescriptor) 
{

    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
#ifndef ANDROID
#else
    mMediaSource->startVlcServer(pipeReadDescriptor);
#endif //ANDROID
}

void  CPlayerLogic::obtainPlayerState()
{
    mGuiController->playlistPosChanged(mCurrPos);
        //there is the freaky expression, but it seems it works properly
	if (mPlayer->isTrackEnded() && (mMediaState->getRoleState() == SERVER ||  ( mMediaState->getRoleState() == CLIENT && mMediaState->getStreamState() == MYSIDE)))
	{
		LOG4CPLUS_INFO(msLogger, "I am server and track's ended");
		localNext();
		return;
	}
    if (mPlayer->isTrackInErrorState() && mMediaState->getPlayBackState() == PLAY)
	{
        	LOG4CPLUS_INFO(msLogger, "obtainPlayerState:: mPlayer->isTrackInErrorState() && mMediaState->getPlayBackState() == PLAY");
		if (mMediaState->getConnectionState() == CONNECTED)
		{
            		LOG4CPLUS_INFO(msLogger, "obtainPlayerState:: mPlayer->isTrackInErrorState() && mMediaState->getPlayBackState() == PLAY :: CONNECTED");
            		mPlayer->play(mMediaState->getSource(), mMediaState->getAVFormat());
		}
		else
		{
		    mPlayer->play(mLocalServer->getSource(), mMediaState->getAVFormat());
		}
	}   
}
