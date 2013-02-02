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
#ifndef SEAT_COORDS_HPP
#define SEAT_COORDS_HPP

#include "ISeatProfiles.hpp"

typedef ISeatSenderProfile::state_t state_t;

// Seat bottom screen  <--> relative coords transform helper
// s[xy] -- screen coords
// r[xy] -- relative coords

namespace {
  const int sxMin  = 454;
  const int sxMax  = 494;
  const int syMin  = 302;
  const int syMax  = 342;

  const int rxMin  = SEAT_MIN_X;
  const int rxMax  = SEAT_MAX_X;
  const int ryMin  = SEAT_MIN_Y;
  const int ryMax  = SEAT_MAX_Y;

  // back coordinates (central position)
  const int sBackX = 535;
  const int sBackY = 197;
}

inline int normalizeRx( int relx ) {
  return std::max( rxMin, std::min( rxMax, relx ) );
}

inline int normalizeRy( int rely ) {
  return std::max( ryMin, std::min( ryMax, rely ) );
}

inline int translate( int c1, int min1, int max1, int min2, int max2 )
{
  const int c2 = (c1 - min1) * (max2 - min2) / (max1 - min1) + min2;
  return c2;
}

// translate relative to bottom position

inline int rel2screenX( int relx ) {
  return translate( normalizeRx( relx ), rxMin, rxMax, sxMin, sxMax );
}

inline int rel2screenY( int rely ) {
  return translate( normalizeRy( rely ), ryMin, ryMax, syMin, syMax );
}

// translate bottom to back position

inline int bottom2backX( int sx ) {
  const int dx = sx - ( sxMax + sxMin ) / 2;
  return sBackX + dx;
}

inline int bottom2backY( int sy ) {
  const int dy = sy - ( syMin + syMax ) / 2;
  return sBackY + dy;
}

#endif //SEAT_COORDS_HPP
