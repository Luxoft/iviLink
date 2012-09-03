/* 
 * 
 * iviLINK SDK, version 1.1.2
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











#ifndef CDUMMYCONNECTIONFINDER_HPP_
#define CDUMMYCONNECTIONFINDER_HPP_

/********************************************************************
 *
 * System includes
 *
 ********************************************************************/
#include <cassert>
/********************************************************************
 *
 * Other includes
 *
 ********************************************************************/
#include "CConnectionFinder.hpp"
 
namespace iviLink
{
   namespace ConnectivityAgent
   {
      namespace HAL
      {
         /// CDummyConnectionFinder class
         /**
          * Special ConnectionFinder for testing purposes. It creates CTcpCarrierAdapter and connects to
          * address "localhost".
          */
         class CDummyConnectionFinder: public CConnectionFinder
         {
         private:
            static Logger logger;

         public:
            // Methods section

            /**
             * Constructor
             * @param handler object which will process found connections
             * @param gender gender of connection
             */
            CDummyConnectionFinder(IFoundConnectionHandler& handler, EGenderType gender);

            /**
             * Destructor
             */
            virtual ~CDummyConnectionFinder();

         private:
            // Methods section

            /**
             * Method that will work in separate thread.
             */
            virtual void performSearch();

         };
      }
   }
}

#endif
