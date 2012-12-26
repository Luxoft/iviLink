/* 
 * iviLINK SDK, version 1.1.19
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
 */ 
 

#ifndef CREPOCONTROLLER_HPP_
#define CREPOCONTROLLER_HPP_

#include "Logger.hpp"

#include "CProfileRepoClient.hpp"
#include "CProfileRepoConnection.hpp"

namespace iviLink
{

   namespace conf
   {
      class Configurator;
   } // namespace conf

   namespace PMP
   {

      /**
       * Class is used to control Connection with Profile Repository
       */
      class CRepoController
      {
      public:
         /**
          * Returns instance of Profile Repository client
          */
         ProfileRepository::IProfileRepoClient * repository();

         /**
          * Returnes instance of CRepoController class
          */
         static CRepoController * instance(conf::Configurator * pConfig = 0);

         /**
          * Returns true if connection is OK
          */
         bool checkConnection() const;

      private:
         /**
          * Construtor
          * @param pConfig is pointer to configurator object
          */
         CRepoController(conf::Configurator * pConfig);

         /**
          * Destructor
          */
         ~CRepoController();

         /// Not implemented
         CRepoController(const CRepoController &);
         CRepoController & operator = (const CRepoController &);

         ProfileRepository::CProfileRepoClient mRepoClient; ///< Object of Profile Repository client
         CProfileRepoConnection mConnection;                ///< Object of Profile Repository connection handler

         static Logger msLogger;                            ///< object of logger
      };

   }

}

#endif /* CREPOCONTROLLER_HPP_ */
