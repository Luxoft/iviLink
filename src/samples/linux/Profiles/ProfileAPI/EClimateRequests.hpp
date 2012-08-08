/* 
 * 
 * iviLINK SDK, version 1.0.1
 * http://www.ivilink.net
 * Cross Platform Application Communication Stack for In-Vehicle Applications
 * 
 * Copyright (C) 2012, Luxoft Professional Corp., member of IBS group
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
 * 
 */





#ifndef ECLIMATEREQUESTS_HPP_
#define ECLIMATEREQUESTS_HPP_

enum ETempEvent
{
   TEMP_UP_DRIVER = 0,
   TEMP_DOWN_DRIVER,
   TEMP_UP_PASS,
   TEMP_DOWN_PASS,
   TEMP_SYNC
};

enum EFanEvent
{
   FAN_FAST = 0,
   FAN_LOW,
   FAN_OFF
};

enum EHeaterEvent
{
   HEAT_DOWN = 0,
   HEAT_UP,
   HEAT_ALL   
};

enum EGeneralRequest
{
   GEN_REQ_AUTO = 0,
   GEN_REQ_RECIR,
   GEN_REQ_AC,
   GEN_REQ_FRONT,
   GEN_REQ_REAR
};

#endif /* ECLIMATEREQUESTS_HPP_ */
