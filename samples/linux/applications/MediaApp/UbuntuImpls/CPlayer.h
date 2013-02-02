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
#ifndef CPLAYER_H
#define CPLAYER_H

#include <string>
#include <vlc/vlc.h>
#include <QWidget>
#include <QObject>

#include "Logger.hpp"
#include "CRWMutex.hpp"

#include "CVideoWidget.hpp"
#include "IPlayer.hpp"
#include "MediaAppCommon.hpp"
#include "CVideoWidget.hpp"


class CRWMutex;

class CPlayer: public QObject, 
               public IPlayer
{
    Q_OBJECT
public:
    CPlayer();
    void setWidget(CVideoWidget* videoWidget);
    
    virtual ~CPlayer();
    virtual void registerPlaybackStartedCallback(IPlayerCallback* playerLogic);
    virtual void stop();
    virtual void pause();
    virtual int  play(const std::string& source, const  std::string& avform);
    virtual int  resume();
    virtual void volumeUp();
    virtual void volumeDown();
    virtual bool isTrackEnded();
    virtual bool isTrackInErrorState();

private slots:

    void playerPlaySlot(QString source, QString avform); 
    void playerStopSlot(); 

signals:

    void playerPlaySig(QString source, QString avform); 
    void playerStopSig(); 

private:

    void release();

    int actuallyPlay(const std::string& source, const std::string& avform);
    void actuallyStop();

    CRWMutex*                mMutex;
    libvlc_instance_t*       mInst;
    libvlc_media_player_t*   mMediaPlayer;
    libvlc_media_t*          mPath;
    int                      mVolume;
    bool 		     hasPlayerInstance;
    CVideoWidget*            mVideoWidget;
    static Logger            msLogger;

    IPlayerCallback * mPlaybackStartedCallback;

};

#endif // CPLAYER_H

