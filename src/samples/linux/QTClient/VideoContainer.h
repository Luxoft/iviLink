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
