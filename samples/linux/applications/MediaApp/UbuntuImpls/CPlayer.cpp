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
#include <QString>

#include "CPlayer.h"
#include "Exit.hpp"

Logger CPlayer::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("samples.Applications.Media.CPlayer"));

CPlayer::CPlayer()
: mVideoWidget(0),
  mMediaPlayer(NULL),
  mPlaybackStartedCallback(NULL)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT("log4cplus.properties"));
    mMutex = new CRWMutex();    
    const char * const argv[] = {
        "--no-video-title-show",
        "-vvv",
        "--packetizer-mpegvideo-sync-iframe"
    };
    mInst = libvlc_new(sizeof(argv) / sizeof(*argv), argv);
    assert(mInst);
    mVolume = VOLUME_INIT;
    hasPlayerInstance = false;
    //for process show and hide in qt-threads
    if (connect(this, SIGNAL(playerPlaySig(QString, QString)), this, SLOT(playerPlaySlot(QString, QString)), Qt::QueuedConnection) == false)
    {
        LOG4CPLUS_INFO(msLogger, "qt - connect play failed!");
        killProcess(1);
    }
    if (connect(this, SIGNAL(playerStopSig()), this, SLOT(playerStopSlot()), Qt::QueuedConnection) == false)
    { 
        LOG4CPLUS_INFO(msLogger, "qt - connect  stop failed!");
        killProcess(1); 
    }
}

CPlayer::~CPlayer()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    stop();
    release();
    delete mMutex;
}

void CPlayer::registerPlaybackStartedCallback(IPlayerCallback * playerLogic)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    mPlaybackStartedCallback = playerLogic;
}

void CPlayer::setWidget(CVideoWidget* videoWidget)
{
    assert(videoWidget);
    if (videoWidget)
    	mVideoWidget = videoWidget;
}

void CPlayer::actuallyStop()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    if (hasPlayerInstance)
	{
        mVideoWidget->video_hide();
	    mMutex->lockWrite();
	    libvlc_media_player_stop(mMediaPlayer);
	    libvlc_media_player_release(mMediaPlayer);
	    mMutex->unlockWrite();
	    hasPlayerInstance = false;
	}
 
}

void CPlayer::pause()
{
 
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    if (hasPlayerInstance)
    {
	    mMutex->lockWrite();
	    libvlc_media_player_pause(mMediaPlayer);
	    mMutex->unlockWrite();
    }

}

int CPlayer::play(const std::string& source, const std::string& avform)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    emit playerPlaySig(QString::fromStdString(source), QString::fromStdString(avform));
    return 0;
}


void CPlayer::stop()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    emit playerStopSig();
}


int CPlayer::actuallyPlay(const std::string& source, const std::string& avform)
{
 
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    LOG4CPLUS_INFO(msLogger, "args: " + source + " " + avform);
    mVideoWidget->video_hide();
    usleep(2000);
    mMutex->lockWrite();
    LOG4CPLUS_INFO(msLogger, "source: " + source);
    mPath = libvlc_media_new_path(mInst, source.c_str());
    LOG4CPLUS_INFO(msLogger, "Player now!");
    if (hasPlayerInstance == false) 
    {
        mMediaPlayer = libvlc_media_player_new_from_media(mPath);
    }
    else 
    {
        libvlc_media_player_set_media(mMediaPlayer,mPath);
    }
    hasPlayerInstance = true;
    libvlc_media_release (mPath);
    int res = ERROR_PLAY;
    if (avform == VIDEO)
    {
        LOG4CPLUS_INFO(msLogger, "get video source");
        assert(mMediaPlayer);
        mVideoWidget->setMediaPlayer(mMediaPlayer);
        mVideoWidget->show();
	    res = libvlc_media_player_play(mMediaPlayer);
        mMutex->unlockWrite();
        if (res == ERROR_PLAY)
        {
            LOG4CPLUS_WARN(msLogger, "vlc player cannot start playing media");
        }
        else
        {
            if(mPlaybackStartedCallback)
            {
                mPlaybackStartedCallback->onPlaybackStarted();
            }
        }
        return res;
    }
    else if (avform == AUDIO)
    {
        LOG4CPLUS_INFO(msLogger, "get audio source");
	    res = libvlc_media_player_play(mMediaPlayer);
        mMutex->unlockWrite();
        if (res == ERROR_PLAY)
        {
            LOG4CPLUS_WARN(msLogger, "vlc player cannot start playing media");
        }
        else
        {
            if(mPlaybackStartedCallback)
            {
                mPlaybackStartedCallback->onPlaybackStarted();
            }
        }
        return res;
    }
    LOG4CPLUS_WARN(msLogger, "unknown source format");
    mMutex->unlockWrite();
    return ERROR_PLAY;
}

void CPlayer::release()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    mMutex->lockWrite();
    libvlc_release(mInst);
    mMutex->unlockWrite();
 }

int CPlayer::resume()
{
    if (mMediaPlayer)
    {
	    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
	    int res = ERROR_PLAY;
	    mMutex->lockWrite();
	    res = libvlc_media_player_play(mMediaPlayer);
	    mMutex->unlockWrite();
	    return res;
    }
    LOG4CPLUS_ERROR(msLogger, "CPlayer::resume(): return ERROR_PLAY");
    return ERROR_PLAY;
}

void CPlayer::volumeUp()
{
LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    if (hasPlayerInstance && mVolume < VOLUME_MAX)
    {
		mVolume = mVolume + VOLUME_STEP;
		mMutex->lockWrite();
		libvlc_audio_set_volume(mMediaPlayer, mVolume);
		mMutex->unlockWrite();
    }
 
}

void CPlayer::volumeDown()
{
 LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    if (hasPlayerInstance && mVolume > VOLUME_MIN)
    {	   
		mVolume = mVolume - VOLUME_STEP;
		mMutex->lockWrite();
		libvlc_audio_set_volume(mMediaPlayer, mVolume);
		mMutex->unlockWrite();
    }
 
}

bool CPlayer::isTrackEnded()
{
    if (mMediaPlayer && hasPlayerInstance)
	{
		libvlc_state_t playerState;	
		playerState = libvlc_media_player_get_state(mMediaPlayer);
		return (playerState == libvlc_Ended /*|| playerState == libvlc_Error*/);
	}
	return false;
}

bool CPlayer::isTrackInErrorState()
{
    if (mMediaPlayer && hasPlayerInstance)
	{
		libvlc_state_t playerState;	
		playerState = libvlc_media_player_get_state(mMediaPlayer);	        
		return (playerState == libvlc_Error);
	}
	return false;
}

void CPlayer::playerPlaySlot(QString source, QString avform)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    actuallyPlay(source.toStdString(), avform.toStdString());
} 
void CPlayer::playerStopSlot()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    actuallyStop();
}
