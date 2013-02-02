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

#ifndef ANDROID

#ifndef CVIDEOWIDGET_HPP_
#define CVIDEOWIDGET_HPP_

#include <QWidget>
#include <qdeclarativeview.h>
#include <vlc/vlc.h>

#include "Logger.hpp"

class CVideoWidget   : public QWidget
{
      Q_OBJECT
   public:
      explicit CVideoWidget(QWidget *parent = 0);
      virtual ~CVideoWidget();
      void setControlView(QDeclarativeView* view_control);

   public slots:
      void video_hide();
      void closeControlView();  

   public:
      void setMediaPlayer(libvlc_media_player_t * pMediaPlayer);
      void mousePressEvent ( QMouseEvent * event );

   signals:
      void startTimerForView();

   private:
      QDeclarativeView*  mControlView;
      static Logger      msLogger;
};

#endif /* CVIDEOWIDGET_HPP_ */

#endif //ANDROID
