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
 

#ifndef CPMPPROTOCONTROLLER_HPP_
#define CPMPPROTOCONTROLLER_HPP_

#include "Logger.hpp"

namespace iviLink
{

   namespace PMP
   {

      class CPmpProtoThread;
      class CPmpProtocol;
      class IPmpProtocol;

      /**
       * Class is used to organize working of PMP Protocol in it's own thread
       */
      class CPmpProtoController
      {
      public:
         /**
          * @return pointer on instance of PMP Protocol
          */
         IPmpProtocol * protocol();

         CPmpProtoController();
         ~CPmpProtoController();

         void start();
         void stop();

      private:
         /**
          * Not implemented
          */
         CPmpProtoController(const CPmpProtoController&);
         CPmpProtoController & operator = (const CPmpProtoController&);

         CPmpProtocol* mpProtocol;       ///< PMP Protocol
         CPmpProtoThread* mpProtoThread; ///< PMP Protocol thread


         static Logger msLogger;       ///< object of logger
      };

   }

}

#endif /* CPMPPROTOCONTROLLER_HPP_ */
