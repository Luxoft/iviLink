/* 
 *  iviLINK SDK, version 0.9 (for preview only)                                      
 *    http://www.ivilink.net                                                         
 *  Cross Platform Application Communication Stack for In-Vehicle Applications       
 *                                                                                   
 *  Copyright (C) 2012, Luxoft Professional Corp., member of IBS group               
 *                                                                                   
 *  This library is free software; you can redistribute it and/or                    
 *  modify it under the terms of the GNU Lesser General Public                       
 *  License as published by the Free Software Foundation; version 2.1.               
 *                                                                                   
 *  This library is distributed in the hope that it will be useful,                  
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of                   
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU                
 *  Lesser General Public License for more details.                                  
 *                                                                                   
 *  You should have received a copy of the GNU Lesser General Public                 
 *  License along with this library; if not, write to the Free Software              
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA   
 * 
 * 
 * 
 * 
 */

#ifndef VIDEOCONTAINER_H
#define VIDEOCONTAINER_H

#include <phonon/phonon>
#include <QDeclarativeItem>
#include <qdeclarative.h>
#include <iostream>
#include <QGraphicsProxyWidget>
#include <QWidget>
#include "ColorFilterPrWidget.h"

class VideoContainer : public QDeclarativeItem
{
    Q_OBJECT

public:
    VideoContainer(QDeclarativeItem *parent = 0);

    void play(QString source);
    void resume();
    void pause();
    void stop();
    void setVolume(float volume);

    float getVolume();

    bool isPlaying();
    bool isPaused();
    bool isStopped();

public slots:
    void aboutToFinish();

signals :
    void outerAboutToFinish();

private:

    Phonon::VideoWidget * mWidget;
    Phonon::MediaObject * mMedia;
    Phonon::AudioOutput * mAudio;
    QGraphicsProxyWidget* mProxy;
    Phonon::SeekSlider *seekSlider;
    QWidget * container;

    int state; //0 - stopped, 1 - paused, 2 - playing

    void setPlaying();
    void setPaused();
    void setStopped();
};

#endif // VIDEOCONTAINER_H
