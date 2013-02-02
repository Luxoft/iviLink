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

#include <cassert>

#include "CVideoWidget.hpp"

CVideoWidget::CVideoWidget(QWidget *parent)
   : QWidget(parent)
{
   setStyleSheet("QWidget{background-color:black}");
   setWindowState(Qt::WindowFullScreen);
   setWindowFlags(Qt::WindowStaysOnTopHint);
   setCursor(QCursor(Qt::BlankCursor));
}

CVideoWidget::~CVideoWidget()
{

}

void CVideoWidget::mousePressEvent ( QMouseEvent * event )
{
    if (mControlView->isVisible())
    {
        mControlView->hide();
    }
    else
    {
        mControlView->show();
        emit startTimerForView();
    }
}

void CVideoWidget::closeControlView()
{
   mControlView->hide();
}

void CVideoWidget::video_hide()
{
   hide();
   mControlView->hide();
}

void CVideoWidget::setMediaPlayer(libvlc_media_player_t * pMediaPlayer)
{
   assert(pMediaPlayer);
   if (pMediaPlayer)
   {
      libvlc_media_player_set_xwindow(pMediaPlayer,winId());
   }
}

void CVideoWidget::setControlView(QDeclarativeView* view_control)
{
	mControlView = view_control;
}

#endif //ANDROID
