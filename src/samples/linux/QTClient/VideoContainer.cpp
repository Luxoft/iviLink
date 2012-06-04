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

 /****************************************************************************/
 /*                                                                          */
 /* This file is part of iviLINK 0.9 Technology Preview Samples Set.         */
 /*                                                                          */
 /* iviLINK 0.9 Technology Preview Samples Set is free software:             */
 /* you can redistribute it and/or modify                                    */
 /* it under the terms of the GNU General Public License as published by     */
 /* the Free Software Foundation, version 3 of the License.                  */
 /*                                                                          */
 /* iviLINK 0.9 Technology Preview Samples Set is distributed in the         */
 /* hope that it will be useful, but WITHOUT ANY WARRANTY; without           */
 /* even the implied warranty of MERCHANTABILITY or FITNESS FOR A            */
 /* PARTICULAR PURPOSE.  See the GNU General Public License for more details.*/
 /*                                                                          */
 /* You should have received a copy of the GNU General Public License        */
 /* along with iviLINK 0.9 Technology Preview.                               */
 /* If not, see <http://www.gnu.org/licenses/>.                              */
 /*                                                                          */
 /****************************************************************************/

#include "VideoContainer.h"
#include "ColorFilterPrWidget.h"


VideoContainer::VideoContainer(QDeclarativeItem *parent) :
    QDeclarativeItem(parent)
{ //all initialization is here
    container=new QWidget;
    container->setFixedHeight(359);
    container->setFixedWidth(575);
    mMedia = new Phonon::MediaObject(container);
    mWidget = new Phonon::VideoWidget(container);
    mWidget->setFixedHeight(359);
    mWidget->setFixedWidth(575);
    mWidget->setAspectRatio(Phonon::VideoWidget::AspectRatioAuto);
    mWidget->setScaleMode(Phonon::VideoWidget::FitInView); //set to ScaleAndCrop if you wish to get rid of the black bars

    Phonon::createPath(mMedia, mWidget);
    mAudio=new Phonon::AudioOutput(Phonon::VideoCategory, this);
    Phonon::createPath(mMedia, mAudio);

    mProxy = new ColorFilterProxyWidget(this);
    mProxy->setWidget(container);

    mWidget->setHue(0.1);
    mWidget->setSaturation(-0.4);

    seekSlider = new Phonon::SeekSlider(container);
    seekSlider->setMediaObject(mMedia);

    connect(mMedia, SIGNAL(aboutToFinish()),this, SLOT(aboutToFinish()));
}

void VideoContainer::play(QString source)
{
    mMedia->setCurrentSource(source);
    mMedia->play();
    setPlaying();
}

void VideoContainer::pause()
{
    mMedia->pause();
    setPaused();
}

void VideoContainer::resume()
{
    mMedia->play();
    setPlaying();
}

void VideoContainer::stop()
{
    mMedia->stop();
    setStopped();
}

void VideoContainer::aboutToFinish()
{
   //signals to btnProcessor
    emit outerAboutToFinish();
}

void VideoContainer::setVolume(float volume)
{
    mAudio->setVolume(volume);
}

float VideoContainer::getVolume()
{
    return mAudio->volume();
}

bool VideoContainer::isPlaying()
{
    return state==2;
}

bool VideoContainer::isPaused()
{
    return state==1;
}

bool VideoContainer::isStopped()
{
    return state==0;
}

void VideoContainer::setPlaying()
{
    state=2;
}

void VideoContainer::setPaused()
{
    state=1;
}

void VideoContainer::setStopped()
{
    state=0;
}

