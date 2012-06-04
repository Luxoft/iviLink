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

#include "CBtnProcessor.hpp"
#include "framework/libraries/ServicesLib/src/include/CVehicleSystemService.hpp"
#include "framework/libraries/ServicesLib/src/include/CClimateService.hpp"

Logger CBtnProcessor::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("samples.app.CBtnProcessor"));

CBtnProcessor::CBtnProcessor(int sock, QObject *parent)
   : QObject(parent)
   , reverse(false)
   , m_sock(sock)
   , curr_state(0)
   , mpClimateProfile(NULL)
   , mpSeatProfile(NULL)
{
}

void CBtnProcessor::videoPopulateList()
{ //called at startup, initiates GUI
   QDir myDir("/home/bububu/temp/vids");
   videoDir=myDir; //store it globally to have access to abs path later
   QStringList filters;
   filters << "*.mp4";
   videoDir.setNameFilters(filters);
   videoList=videoDir.entryList(); //limited by filter only to *.mp3 files
   for(int i=0;i<videoList.length();i++)
   {
      emit videoListPopulate(videoList.at(i)); //strings are passed to qml to be displayed
   }
   videoMaxPosition=videoList.size()-1;
   videoPosition=-1;
   connect(myVid, SIGNAL(outerAboutToFinish()), this, SLOT(videoAboutToFinish()));
}

void CBtnProcessor::setServices(CVehicleSystemService*  pVehicle, CClimateService* pClimate )
{
   if(pClimate)
   {
      CProfile* pProfile  = pClimate->getProfile("ClimateProfile");
      mpClimateProfile = static_cast<IClimateProfile_API*>(pProfile);
   }
   if(pVehicle)
   {
      CProfile* pProfile  = pVehicle->getProfile("SeatProfile");
      mpSeatProfile = static_cast<ISeatProfile_API*>(pProfile);
   }
}

void CBtnProcessor::stateChanged(int state)
{
   LOG4CPLUS_TRACE(msLogger, "state changed from: " + convertIntegerToString(media_st) +
         " to: " + convertIntegerToString(state));
   if(media_st!=state)
   {
      if(isServer==1)
      {
         LOG4CPLUS_INFO(msLogger, "stateChanged");
         event.eventName = state==1? MUSIC : VIDEO;
         writeMessage(event, m_sock);
      }
      else
      {
         if(curr_state==1)
         {
           LOG4CPLUS_INFO(msLogger, "fanIncToggle");
           event.eventName = state==1? MUSIC : VIDEO;
           writeMessage(event, m_sock);
         }
      }
      media_st=state;
   }
}
void CBtnProcessor::initPhonon()
{
   media= new Phonon::MediaObject(this);
   mOutput=new Phonon::AudioOutput(Phonon::MusicCategory, this);
   createPath(media, mOutput);
   mOutput->setVolume(0.5);
   volStep=0.1;
   media->setTickInterval(100);

   connect(media, SIGNAL(tick(qint64)), this, SLOT(tick(qint64))); //can be used to update slider or smth else
   connect(media, SIGNAL(aboutToFinish()), this, SLOT(aboutToFinish())); //is neccessary to continue playing ("repeat all" logic)

   setStopped();
}
void CBtnProcessor::populateList()
{ //called at startup, initiates GUI
   LOG4CPLUS_INFO(msLogger, "HELOOOO");
   QDir myDir("/home/bububu/XplodingPlastixs");
   dir=myDir; //store it globally to have access to abs path later
   QStringList filters;
   filters << "*.mp3";
   myDir.setNameFilters(filters);
   playlist=myDir.entryList(); //limited by filter only to *.mp3 files
   for(int i=0;i<playlist.length();i++)
   {
      LOG4CPLUS_INFO(msLogger, playlist.at(i).toStdString());
      emit listPopulate(playlist.at(i)); //strings are passed to qml to be displayed
   }
   maxPlaylistPosition=playlist.size()-1;

   //should be called somewhere else, actually (constructor?)
   initPhonon();
}

