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

#include <iostream>
#include <QObject>
#include <QVariant>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include <sstream>
#include "string.h"
#include "common.h"
#include "utils/json/include/json/json.h"
#include "utils/misc/include/Logger.hpp"



void writeMessage(Event event, int newsockfd, QObject * rootObject)
{
   Logger logger = Logger::getInstance(LOG4CPLUS_TEXT("samples.app.writeMessage"));

   int n;
   char buffer[BUF_SIZE];
   bzero(buffer,BUF_SIZE);

   char header[3];
   header[0] = 0x01;

   //transform AirCondState into json
   Json::Value root;

   if (event.eventName == FAN_INC_TOGGLE)
   {
       root[fanSpd] = up;
       LOG4CPLUS_INFO(logger, "FAN_INC_TOGGLE");
   }
   else if (event.eventName == REQ_SHARE)
   {
      LOG4CPLUS_INFO(logger, "REQ_SHARE");
       header[0] = 0x04;
       root[vnc] = "request";
   }
   else if (event.eventName == OK_SHARE)
   {
      LOG4CPLUS_INFO(logger, "OK_SHARE");
       header[0] = 0x04;
       root[vnc] = "ok";
   }
   else if (event.eventName == STOP_SHARE)
   {
      LOG4CPLUS_INFO(logger, "STOP_SHARE");
       header[0] = 0x04;
       root[vnc] = "stop";
   }
   else if (event.eventName == FAN_DECR_TOGGLE)
   {
       root[fanSpd] = down;
       LOG4CPLUS_INFO(logger, "FAN_DECR_TOGGLE");
   }
   else if (event.eventName == DRIV_TEMP_INC_TOGGLE)
   {
       root[tempD] = up;
       LOG4CPLUS_INFO(logger, "DRIV_TEMP_INC_TOGGLE");
   }
   else if (event.eventName == DRIV_TEMP_DECR_TOGGLE)
   {
       root[tempD] = down;
       LOG4CPLUS_INFO(logger, "DRIV_TEMP_DECR_TOGGLE");
   }
   else if (event.eventName == PASS_TEMP_INC_TOGGLE)
   {
       root[tempP] = up;
       LOG4CPLUS_INFO(logger, "PASS_TEMP_INC_TOGGLE");
   }
   else if (event.eventName == PASS_TEMP_DECR_TOGGLE)
   {
       root[tempP] = down;
       LOG4CPLUS_INFO(logger, "PASS_TEMP_DECR_TOGGLE");
   }
   else if (event.eventName == AC_TOGGLE)
   {
       root[acStatus] = toggle;
       LOG4CPLUS_INFO(logger, "AC_TOGGLE");
   }
   else if (event.eventName == FRONT_W_TOGGLE)	//not sure weather fan or heater used. using heater for simplification
   {
       root[head] = toggle;
       LOG4CPLUS_INFO(logger, "FRONT_W_TOGGLE");
   }
   else if (event.eventName == REAR_W_TOGGLE)
   {
       root[rear] = toggle;
       LOG4CPLUS_INFO(logger, "REAR_W_TOGGLE");
   }
   else if (event.eventName == OFF_TOGGLE)
   {
       root[off] = toggle;
       LOG4CPLUS_INFO(logger, "OFF_TOGGLE");
   }
   else if (event.eventName == DUAL_TOGGLE)
   {
       root[dual] = toggle;
       LOG4CPLUS_INFO(logger, "DUAL_TOGGLE");
   }
   else if (event.eventName == AUTO_TOGGLE)
   {
       root[auto_mode] = toggle;
       LOG4CPLUS_INFO(logger, "AUTO_TOGGLE");
   }
   else if (event.eventName == FAN_MODE_LOW_TOGGLE)
   {
       root[fanlow] = toggle;
       LOG4CPLUS_INFO(logger, "FAN_MODE_LOW_TOGGLE");
   }
   else if (event.eventName == FAN_MODE_MIDDLE_TOGGLE)
   {
       root[fanmiddle] = toggle;
       LOG4CPLUS_INFO(logger, "FAN_MODE_MIDDLE_TOGGLE");
   }
   else if (event.eventName == FAN_MODE_TOP_TOGGLE)
   {
       root[fantop] = toggle;
       LOG4CPLUS_INFO(logger, "FAN_MODE_TOP_TOGGLE");
   }
   else if (event.eventName == RECIRCULATION_TOGGLE)
   {
       root[recStatus] = toggle;
       LOG4CPLUS_INFO(logger, "RECIRCULATION_TOGGLE");
   }
   else if (event.eventName == ACTIVITY_SEAT)
   {
       root[activity] = seat_tab;
       header[0] = 0x00;
       LOG4CPLUS_INFO(logger, "ACTIVITY_SEAT");
   }
   else if (event.eventName == ACTIVITY_AC)
   {
       root[activity] = ac_tab;
       header[0] = 0x00;
       LOG4CPLUS_INFO(logger, "ACTIVITY_AC");
   }
   else if (event.eventName == ACTIVITY_PLAYER)
   {
   }
   else if (event.eventName == SEAT_HEATER_TOGGLE)
   {
       root[heater] = toggle;
       header[0] = 0x02;
       LOG4CPLUS_INFO(logger, "SEAT_HEATER_TOGGLE");
   }
   else if (event.eventName == SEAT_HEATER_PASS_TOGGLE)
   {
       root[heater_pass] = toggle;
       header[0] = 0x02;
       LOG4CPLUS_INFO(logger, "SEAT_HEATER_PASS_TOGGLE");
   }
   else if (event.eventName == BOTTOM_UP_TOGGLE)
   {
       root[seatLiftUp] = toggle;
       header[0] = 0x02;
       LOG4CPLUS_INFO(logger, "BOTTOM_UP_TOGGLE");
   }
   else if (event.eventName == BOTTOM_DOWN_TOGGLE)
   {
       root[seatLiftDown] = toggle;
       header[0] = 0x02;
       LOG4CPLUS_INFO(logger, "BOTTOM_DOWN_TOGGLE");
   }
   else if (event.eventName == BOTTOM_LEFT_TOGGLE)
   {
       root[seatBottomLeft] = toggle;
       header[0] = 0x02;
       LOG4CPLUS_INFO(logger, "BOTTOM_LEFT_TOGGLE");
   }
   else if (event.eventName == BOTTOM_RIGHT_TOGGLE)
   {
       root[seatBottomRight] = toggle;
       header[0] = 0x02;
       LOG4CPLUS_INFO(logger, "BOTTOM_RIGHT_TOGGLE");
   }
   else if (event.eventName == BACK_LEFT_TOGGLE)
   {
       root[seatBackLeft] = toggle;
       header[0] = 0x02;
       LOG4CPLUS_INFO(logger, "BACK_LEFT_TOGGLE");
   }
   else if (event.eventName == BACK_RIGHT_TOGGLE)
   {
       root[seatBackRight] = toggle;
       header[0] = 0x02;
       LOG4CPLUS_INFO(logger, "BACK_RIGHT_TOGGLE");
   }
   else if (event.eventName == DRIVER_SEAT_TOGGLE)
   {
       root[driverSeat] = toggle;
       header[0] = 0x02;
       LOG4CPLUS_INFO(logger, "DRIVER_SEAT_TOGGLE");
   }
   else if (event.eventName == PASSEN_SEAT_TOGGLE)
   {
       root[passenSeat] = toggle;
       header[0] = 0x02;
       LOG4CPLUS_INFO(logger, "PASSEN_SEAT_TOGGLE");
   }
   else if (event.eventName == PIN_READY)
   {
       std::cout<<"pin step 1"<<std::endl;
       QVariant pin_code = rootObject->property("pincode_str");
       std::stringstream sst;
       LOG4CPLUS_INFO(logger, "pin step 2");
       root[pin_value] = pin_code.toString().toStdString().c_str();
       LOG4CPLUS_INFO(logger, "pin step 3");
       header[0] = 0x03;
       LOG4CPLUS_INFO(logger, "FAN_INC_TOGGLE");
       LOG4CPLUS_INFO(logger, "PIN_READY");
   }
   else if (event.eventName == PLAY) //+++
   {
       root[player] = play;
       header[0] = 0x05;
       LOG4CPLUS_INFO(logger, "PLAY");
   }
   else if (event.eventName == STOP) //+++
   {
       root[player] = stop;
       header[0] = 0x05;
       LOG4CPLUS_INFO(logger, "STOP");

   }else if (event.eventName == PL_PAUSE)//+++
   {
       root[player] = pl_pause;
       header[0] = 0x05;
       LOG4CPLUS_INFO(logger, "PL_PAUSE");

   }else if (event.eventName == FORWARD) //+++ if only client
   {
       root[player] = forward;
       header[0] = 0x05;
       LOG4CPLUS_INFO(logger, "FORWARD");

   }else if (event.eventName == PREVIOUS) ////+++ if only client
   {
       root[player] = previous;
       header[0] = 0x05;
       LOG4CPLUS_INFO(logger, "PREVIOUS");

   }else if (event.eventName == PL_SHARE) //+++
   {
       root[player] = pl_share;
       header[0] = 0x05;
       LOG4CPLUS_INFO(logger, "PL_SHARE");

   }else if (event.eventName == MUSIC) //+++
   {
       root[player] = music;
       header[0] = 0x05;
       LOG4CPLUS_INFO(logger, "MUSIC");

   }else if (event.eventName == VIDEO) //+++
   {
       root[player] = video;
       header[0] = 0x05;
       LOG4CPLUS_INFO(logger, "VIDEO");

   }else if (event.eventName == OPEN)
   {
       root[source] = event.src;
       root[player] = open;
       header[0] = 0x05;
       LOG4CPLUS_INFO(logger, "OPEN");

   }else if (event.eventName == SEEK)
   {
       root[player] = seek;
       header[0] = 0x05;
       LOG4CPLUS_INFO(logger, "SEEK");

   }else if(event.eventName==PLAYLIST){
       LOG4CPLUS_INFO(logger, "sending song " + event.src);
       root[player]="playlist";
       root["playlist"]=event.src;
       root["end"]=event.plistEndMarker;
       header[0]=0x05;
   }
   else if (event.eventName == TEST_EVENT)
   {
      LOG4CPLUS_INFO(logger, "TEST_EVENT");
   }
   else
   {
      LOG4CPLUS_INFO(logger, "Wrong event");
   }

   Json::StyledWriter writer;
   // Make a new JSON document for the configuration. Preserve original comments.
   std::string outputConfig = writer.write(root);

   //write to the socket

   short size = outputConfig.length();

   LOG4CPLUS_INFO(logger, "FAN_INC_TOGGLE");
   int isize = size;
   LOG4CPLUS_INFO(logger, "size " + convertIntegerToString(size));

   header[1] =  (unsigned char)((size & 0xFF00) >> 8 );
   header[2] = (unsigned char) (size & 0x00FF);

   buffer[0] = header[0];
   buffer[1] = header[1];
   buffer[2] = header[2];
   strncpy(buffer+3, outputConfig.c_str(), outputConfig.length());

   n = write(newsockfd, buffer ,outputConfig.length()+3);

   if (n < 0)
   {
      LOG4CPLUS_INFO(logger, "FAN_INC_TOGGLE");
      return;
   }
}

