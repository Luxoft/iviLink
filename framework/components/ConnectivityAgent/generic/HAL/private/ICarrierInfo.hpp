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


#ifndef ICARRIERINFO_HPP_
#define ICARRIERINFO_HPP_
namespace AXIS
{
   namespace ConnectivityAgent
   {
      namespace HAL
      {
         /**
          * This interface is used to encapsulate information about carrier.
          * This information is used to create new connection by CCarrierAdapter.
          * Information is obtained (and instances of this interface are created) by
          * classes of ICarrierFinder interface.
          */
         class ICarrierInfo
         {

         };
      }
   }
}


#endif /* ICARRIERINFO_HPP_ */
