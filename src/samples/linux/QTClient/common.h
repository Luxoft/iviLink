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

#ifndef COMMON_H
#define COMMON_H


#include <map>
#include <string>

enum FanMode
{
    FAN_LOW		= 0x01,
    FAN_MIDDLE	= 0x02,
    FAN_HIGH	= 0x04,
    FAN_AUTO	= 0x08,
};

enum Channels
{
    VNC_CHANNEL_NR          = 1,
    CLIMATE_CHANNEL_NR      = 2,
    SEAT_CHANNEL_NR         = 3
};

enum LIMITATIONS
{
FAN_MAX_SPEED	= 7,
FAN_MIN_SPEED	= 0,
TEMP_MIN		= 16,
TEMP_MAX		= 27,
LEFT_MAX		= 20,
RIGHT_MAX		= (-20),
ANGLE_MIN		= (-40),
ANGLE_MAX		= 80,
LIFT_MAX		= 20,
LIFT_MIN		= (-20),
};

struct AirCondState
{
    bool	autoClimate;
    bool	dualClimate;
    int		fanSpeed;
    int		driverTemp;
    int		passTemp;
    bool	airRecirc;
    bool    fanLow;
    bool    fanMiddle;
    bool    fanTop;
    bool	isAC;
    bool	frontWindow;
    bool	rearWindow;
    bool	isOFF;


};



enum EventName{
    NO_EVENT  = 0,

    AUTO_TOGGLE,			//done - may turn on. set everything to default mode except driver temp
    DUAL_TOGGLE,			//done
    OFF_TOGGLE,				//done
    REAR_W_TOGGLE,			//done
    FRONT_W_TOGGLE,			//done
    AC_TOGGLE,				//done - may turn on
    FAN_INC_TOGGLE,			//done - may turn on
    FAN_DECR_TOGGLE,		//done
    DRIV_TEMP_INC_TOGGLE,	//done
    DRIV_TEMP_DECR_TOGGLE,	//done
    PASS_TEMP_INC_TOGGLE,	//done
    PASS_TEMP_DECR_TOGGLE,	//done
    FAN_MODE_LOW_TOGGLE,    //done
    FAN_MODE_MIDDLE_TOGGLE, //done
    FAN_MODE_TOP_TOGGLE,    //done
    RECIRCULATION_TOGGLE,   //done

    PIN_READY,              //done

    //seat
    SEAT_HEATER_TOGGLE,
    SEAT_HEATER_PASS_TOGGLE,

    BOTTOM_UP_TOGGLE,
    BOTTOM_DOWN_TOGGLE,
    BOTTOM_LEFT_TOGGLE,
    BOTTOM_RIGHT_TOGGLE,
    BACK_LEFT_TOGGLE,
    BACK_RIGHT_TOGGLE,
    DRIVER_SEAT_TOGGLE,
    PASSEN_SEAT_TOGGLE,
    //TODO profiles
    DRV_PROFILE_APPLIED,
    PASSEN_PROFILE_APPLIED,

    ACTIVITY_SEAT,
    ACTIVITY_AC,
    ACTIVITY_PLAYER,
    TEST_EVENT,

    //vnc

    REQ_SHARE,
    OK_SHARE,
    STOP_SHARE,
    //VNC_DISC,
    VNC_SERVER,

    //player
    //player
           PLAY,
           STOP,
           PL_PAUSE,
           FORWARD,
           PREVIOUS,
           MUSIC,
           VIDEO,
           PL_SHARE,
           SEEK,
           OPEN,
           DISC,
    PLAYLIST


};

struct Event {
    EventName eventName;
    std::string src;
    int ptos;
    bool plistEndMarker;
    int init;
    std::string andr_ip;
    //seat coord
    int seat_x;
    int seat_y;
    int seat_angle;
};
enum MY_STATE
{
   SHARE = 100,
   NOSHARE,
   ANDR,
   HU,
   TINIT,
};
enum CLIENT_STATE
{
        CLIMATE = 10,
        SEAT,
        PLAYER,
        BROWSER,
};
 const char off[]="off";
 const char tempD[]="temperatureD";
 const char tempP[]="temperatureP";
 const  char fanSpd[]="fan speed";
 const char auto_mode[]="auto";
 const char dual[]="dual";
 const  char head[]="head";
 const char rear[]="rear";
 const char fanlow[] = "fan low";
 const char fanmiddle[] = "fan middle";
 const char fantop[] = "fan top";
 const char acStatus[]="ac";
 const char recStatus[]="recirculation";
 //chair
 const char  seatBackLeft[]="seat back left";
 const char  seatBackRight[]="seat back right";
 const char  seatBottomLeft[]="seat bottom left";
 const char  seatBottomRight[]="seat bottom right";
 const char  seatLiftUp[]="seat lift up";
 const char  seatLiftDown[]="seat lift down";
 const char  heater[]="heater";
 const char  heater_pass[]="heater Pass";
 const char  driverSeat[]="driver seat";
 const char  passenSeat[]="passen seat";

 const char seatBack[]="seat back";
 const char seatBottom[]="seat bottom";
 const char seatLift[]="seat lift";

 const char seatBackDriver[]="seat back driver";	//send
 const char seatBottomDriver[]="seat bottom driver";//send
 const char seatLiftDriver[]="seat lift driver";//send

 const char seatBackPass[]="seat back pass";	//send
 const char seatBottomPass[]="seat bottom pass";//send
 const char seatLiftPass[]="seat lift pass";//send

 const char  activeSeat[]="active seat";

 const char pin_request[]="pin request";
 const char pin_value[]="pin code";


 //values
 const char  up[]="up";
 const char  down[]="down";
 const char  toggle[]="toggle";

 const char activity[]="activity";
 const char seat_tab[]="seat tab";
 const char ac_tab[]="airconditioner tab";

 //vnc
 const char vnc[]="vnc";

 //player
  const char player[] = "player";
  const char play[] = "play";
  const char stop[] = "stop";
  const char pl_share[] = "share";
  const char forward[] = "forward";
  const char previous[] = "previous";
  const char pl_pause[] = "pause";
  const char music[] = "music";
  const char video[] = "video";
  const char seek[] = "seek";
  const char pos[] = "pos";
  const char open[] = "open";
  const char http[] = "http";
  const char source[] = "source";
  const char disc[] = "disc";




//socket definitions
const char IPADDRESS[] = "127.0.0.1";
//port
#define PORT_NUMBER 8889

#define BUF_SIZE 1024



#endif // COMMON_H
