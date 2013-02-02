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


#ifndef ISEATCLIENTPROFILE_HPP_
#define ISEATCLIENTPROFILE_HPP_

#include "SeatState.hpp"
#include "stateness.hpp"

class ISeatReceiverProfile;

class ISeatSenderProfile : public IStateProfileBase<ISeatSenderProfile,ISeatReceiverProfile>
                         , public stateness_sender<iviLink::SeatState>
{
public:
  virtual void  send_state( const state_t& st ) = 0; // from stateness_sender
  static  uid_t uid() { return uid_t("SeatSenderProfile_API_UID"); }
};


class ISeatReceiverProfile : public IStateProfileBase<ISeatReceiverProfile,ISeatSenderProfile>
                           , public stateness_receiver<iviLink::SeatState>
{
public:
  static uid_t uid() { return uid_t("SeatReceiverProfile_API_UID"); }
};

#endif /* ISEATCLIENTPROFILE_HPP_ */
