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


#ifndef ICLIMATE_PROFILES_HPP
#define ICLIMATE_PROFILES_HPP

#include "ClimateState.hpp"
#include "stateness.hpp"


class IClimateReceiverProfile;

class IClimateSenderProfile : public IStateProfileBase<IClimateSenderProfile,IClimateReceiverProfile>
                         , public stateness_sender<iviLink::ClimateState>
{
public:
  virtual void  send_state( const state_t& st ) = 0; // from stateness_sender
  static  uid_t uid() { return uid_t("ClimateSenderProfile_API_UID"); }
};


class IClimateReceiverProfile : public IStateProfileBase<IClimateReceiverProfile,IClimateSenderProfile>
                           , public stateness_receiver<iviLink::ClimateState>
{
public:
  static uid_t uid() { return uid_t("ClimateReceiverProfile_API_UID"); }
};

#endif /* ICLIMATE_PROFILES_HPP */
