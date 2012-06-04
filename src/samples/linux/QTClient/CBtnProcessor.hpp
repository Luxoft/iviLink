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

#ifndef CBTN_PROCESSOR_HPP
#define CBTN_PROCESSOR_HPP

#include <QString>
#include <QDebug>
#include <QDir>
#include <qcoreapplication.h>
#include <phonon/audiooutput.h>
#include <phonon/seekslider.h>
#include <phonon/mediaobject.h>
#include <phonon/volumeslider.h>
#include <phonon/backendcapabilities.h>
#include <iostream>
#include "common.h"
#include "writeMessage.h"
#include "VideoContainer.h"
#include <QtTest/QTest>
#include "samples/linux/Profiles/ProfileAPI/ISeatProfileAPI.hpp"
#include "samples/linux/Profiles/ProfileAPI/IClimateProfileAPI.hpp"
#include "utils/misc/include/Logger.hpp"

class CVehicleSystemService;
class CClimateService;

class CBtnProcessor : public QObject
{
     Q_OBJECT

public:
    CBtnProcessor(int sock = 0, QObject *parent = 0);

    void activitySeatOn(){activitySeat();}

    void activityClimateOn(){activityClimate();}

    void pinCodeOn(){pinCode();}

    void vncOn()
    {
        std::cout<<"on Vnc handler"<<std::endl;
        emit vnc();
    }
    void playerOn()
    {
        emit getPlayerMode();
    }
    void audioOn()
    {
        emit audioPage();
    }

    void videoOn()
    {
      emit videoPage();
    }
    void discOn(){
        emit disconnectPlayer();
    }

    void stopOn(){
        //emit stop();
    }

    void emitPassTemp(int temp){
        emit passTemp(temp);
    }

    void emitDrvTemp(int temp){
        emit drvTemp(temp);
    }

    void emitFanSpeed(int speed){
        emit fanSpeed(speed);
    }

    void emitDual(bool status){
        emit dual(status);
    }

    void emitOff(bool status){
        emit off(status);
    }

    void emitAuto(bool status){
        emit autoSig(status);
    }

    void emitFanLow(bool status){
        emit fanLow(status);
    }

    void emitFanMiddle(bool status){
        emit fanMiddle(status);
    }

    void emitHeater(int heat){
        emit heater(heat);
    }
    void emitHeaterPass(int heat){
        emit heaterPass(heat);
    }

    void emitFanTop(bool status){
        emit fanTop(status);
    }

    void emitAc(bool status){
        emit ac(status);
    }

    void emitHead(bool status){
        emit head(status);
    }

    void emitRear(bool status){
        emit rear(status);
    }
    void emitRecirc(bool status){
        emit recirc(status);
    }

    void videoPopulateList();



    int getPlaying(){ //this and the paused getter are used to determine state of player
        return playingIndex;
    }

    void setPlaying(int pos){
        if(pos<=maxPlaylistPosition){
            pausedIndex=-1;
            playingIndex=pos;
        }
    }

    int pausedIndex;

    int getPaused(){
        return pausedIndex;
    }

    void setPaused(int pos){
        if(pos<=maxPlaylistPosition){
            pausedIndex=pos;
            playingIndex=-1;
        }
    }

   void setStopped()
   {
      pausedIndex=-1;
      playingIndex=-1;
    }

    bool isStopped(){
        return (pausedIndex==playingIndex)&&(playingIndex==-1);
    }

   void populateList();
   void initPhonon();
//goto slots now
    void emitTrackListShare(QString songName){ emit trackListShare(songName);}


signals:
    void pinCode();
    void vnc();


    //new
    void passTemp(int temp);
    void drvTemp(int temp);
    void fanSpeed(int speed);
    void heater(int heat);
    void heaterPass(int heat);
    void dual(bool status);
    void off(bool status);
    void autoSig(bool status);
    void fanLow(bool status);
    void fanMiddle(bool status);
    void fanTop(bool status);
    void ac(bool status);
    void head(bool status);
    void rear(bool status);
    void recirc(bool status);


    //signals for audio
    void swapPlayPause();
    void getPlayerMode();
    void audioPage();
    void playlistPosChanged(int pos);
    void listPopulate(QString songName);
    void trackListShare(QString  songName);
    //end of signals for audio

    //signals for video
    void vidSwapViews(); //swaps VideoContainer and ListView
    void vidSwapPlayPause();
    void videoPage();
    void videoListPopulate(QString vidName);
    void vidTrackListShare(QString  vidName);
    //end of  signals for video

    void disconnectPlayer();
    void activitySeat();
    void activityClimate();




  public slots:

        //btn events
       void fanIncToggle()
       {
           qDebug("fanIncToggle");
           event.eventName = FAN_INC_TOGGLE;
           //writeMessage(event, m_sock);
           this->mpClimateProfile->processEvent(event,true);
       }
       void fanDecToggle()
       {
           qDebug("fanDecToggle");
           event.eventName = FAN_DECR_TOGGLE;
        //   writeMessage(event, m_sock);
           mpClimateProfile->processEvent(event,true);
       }
       void driveTempIncToggle()
       {
           qDebug("driveTempIncToggle");
           event.eventName = DRIV_TEMP_INC_TOGGLE;
           //writeMessage(event, m_sock);
           mpClimateProfile->processEvent(event,true);
       }
       void driveTempDecToggle()
       {
           qDebug("driveTempDecToggle");
           event.eventName = DRIV_TEMP_DECR_TOGGLE;
           //writeMessage(event, m_sock);
           mpClimateProfile->processEvent(event,true);
       }
       void passTempIncToggle()
       {
           qDebug("passTempIncToggle");
           event.eventName = PASS_TEMP_INC_TOGGLE;
           //writeMessage(event, m_sock);
           mpClimateProfile->processEvent(event,true);
       }
       void passTempDecToggle()
       {
           qDebug("passTempDecToggle");
           event.eventName = PASS_TEMP_DECR_TOGGLE;
           //writeMessage(event, m_sock);
           mpClimateProfile->processEvent(event,true);
       }
       void acToggle()
       {
           qDebug("acToggle");
           event.eventName = AC_TOGGLE;
            //writeMessage(event, m_sock);
           mpClimateProfile->processEvent(event,true);
       }
       void frontWToggle()
       {

           qDebug("frontWToggle");
           event.eventName = FRONT_W_TOGGLE;
             //writeMessage(event, m_sock);
           mpClimateProfile->processEvent(event,true);
       }
       void rearWToggle()
       {
           qDebug("rearWToggle");
           event.eventName = REAR_W_TOGGLE;
           //writeMessage(event, m_sock);
           mpClimateProfile->processEvent(event,true);
       }
       void offToggle()
       {
           qDebug("offToggle");
           event.eventName = OFF_TOGGLE;
            //writeMessage(event, m_sock);
           mpClimateProfile->processEvent(event,true);
       }
       void dualToggle()
       {
           qDebug("dualToggle");
           event.eventName = DUAL_TOGGLE;
            //writeMessage(event, m_sock);
           mpClimateProfile->processEvent(event,true);
       }
       void autoToggle()
       {
           qDebug("autoToggle");
           event.eventName = AUTO_TOGGLE;
            //writeMessage(event, m_sock);
           mpClimateProfile->processEvent(event,true);
       }
       void fanModeLowToggle()
       {
           qDebug("fanModeLowToggle");
           event.eventName = FAN_MODE_LOW_TOGGLE;
            //writeMessage(event, m_sock);
           mpClimateProfile->processEvent(event,true);
       }
       void fanModeMedToggle()
       {
           qDebug("fanModeMedToggle");
           event.eventName = FAN_MODE_MIDDLE_TOGGLE;
           //writeMessage(event, m_sock);
           mpClimateProfile->processEvent(event,true);
       }
       void fanModeHighToggle()
       {
           qDebug("fanModeHighToggle");
           event.eventName = FAN_MODE_TOP_TOGGLE;
            //writeMessage(event, m_sock);
           mpClimateProfile->processEvent(event,true);
       }
       void recircToggle()
       {
           qDebug("recircToggle");
           event.eventName = RECIRCULATION_TOGGLE;
           //writeMessage(event, m_sock);
           mpClimateProfile->processEvent(event,true);
       }
       void climateMode()
       {
           qDebug("climateMode");
           event.eventName = ACTIVITY_AC;
            //writeMessage(event, m_sock);
            mpClimateProfile->processEvent(event,true);
       }
       void playerMode()
       {
           qDebug("playerMode");
           event.eventName = ACTIVITY_PLAYER;
            //writeMessage(event, m_sock);
           //mpClimateProfile->processEvent(event,true);

       }
       void chairMode()
       {
           qDebug("seatToggle");
           event.eventName = ACTIVITY_SEAT;
           //writeMessage(event, m_sock);
           mpSeatProfile->processEvent(event,true);
       }
       void vncShare()
       {
           qDebug("vncShare");
           event.eventName = OK_SHARE;
           //writeMessage(event, m_sock);
           //mpVNCProfile->processEvent(event,true);
       }
       void vnc_req()
       {
           qDebug("vnc_req");
           event.eventName = REQ_SHARE;
           // writeMessage(event, m_sock);
           //mpVNCProfile->processEvent(event,true);
       }
       void driverHeaterToggle()
       {
           qDebug("driverHeaterToggle");
           event.eventName = SEAT_HEATER_TOGGLE;
           //writeMessage(event, m_sock);
           mpSeatProfile->processEvent(event,true);
       }
       void passHeaterToggle()
       {
           qDebug("passHeaterToggle");
           event.eventName = SEAT_HEATER_PASS_TOGGLE;
            //writeMessage(event, m_sock);
           mpSeatProfile->processEvent(event,true);
       }

       void bottomUp()
       {
           qDebug("bottomUp");
           event.eventName = BOTTOM_UP_TOGGLE;
            //writeMessage(event, m_sock);
           mpSeatProfile->processEvent(event,true);
       }
       void bottomDown()
       {
           qDebug("bottomDown");
           event.eventName = BOTTOM_DOWN_TOGGLE;
            //writeMessage(event, m_sock);
           mpSeatProfile->processEvent(event,true);
       }
       void bottomLeft()
       {
           qDebug("bottomLeft");
           event.eventName = BOTTOM_LEFT_TOGGLE;
            //writeMessage(event, m_sock);
           mpSeatProfile->processEvent(event,true);
       }
       void bottomRight()
       {
           qDebug("bottomRight");
            event.eventName = BOTTOM_RIGHT_TOGGLE;
           //writeMessage(event, m_sock);
            mpSeatProfile->processEvent(event,true);
       }
       void backLeft()
       {
           qDebug("backLeft");
           event.eventName = BACK_LEFT_TOGGLE;
           //writeMessage(event, m_sock);
           mpSeatProfile->processEvent(event,true);
       }
       void backRight()
       {
           qDebug("backRight");
           event.eventName = BACK_RIGHT_TOGGLE;
            //writeMessage(event, m_sock);
           mpSeatProfile->processEvent(event,true);
       }
       void driverSeatToggle()
       {
           qDebug("driverSeatToggle");
           event.eventName = DRIVER_SEAT_TOGGLE;
          //writeMessage(event, m_sock);
           mpSeatProfile->processEvent(event,true);
       }
       void passenSeatToggle()
       {
           qDebug("passenSeatToggle");
           event.eventName = PASSEN_SEAT_TOGGLE;
            //writeMessage(event, m_sock);
           mpSeatProfile->processEvent(event,true);
       }
   

   void s_videoAboutToFinish()
   {
     //next();
   }


   void stateChanged(int state);
   void setServices(CVehicleSystemService* pVehicle,CClimateService* pClimateService );
   void setContainer(VideoContainer* pContainer)
   {
       myVid = pContainer;
   }
   void setRootObject(QObject * pObj)
   {
       rootObject = pObj;
   }

protected:
       bool reverse;

private:

   IClimateProfile_API*     mpClimateProfile;
   ISeatProfile_API*        mpSeatProfile;

    int m_sock;
  //properties
   bool m_airRecirc;
   QObject * rootObject;
   Event event;
   int isServer;
   int media_st; //1 - audio, 2 - video
   int curr_state; // 1 if HU is a client in player sharing
   std::vector<std::string> remoteSongs;
   VideoContainer * myVid;
   //video logic
   QStringList videoList;
   QDir videoDir;
   int videoPosition;
   int videoMaxPosition;
   int maxPlaylistPosition; //=playlist.size()-1 - needed for next and prev calculations

   int playingIndex;
    QStringList playlist; //stores list of files
    QDir dir; //where all files from playlist are actually (needed to restore absolute path)
    Phonon::MediaObject* media; //needed to control playback (play, stop, etc.)
    //Phonon::SeekSlider *seekSlider;
    Phonon::AudioOutput * mOutput; //needed to control volume
    float volStep;

    static Logger msLogger;
};


#endif // BTNPROCESSOR_H
