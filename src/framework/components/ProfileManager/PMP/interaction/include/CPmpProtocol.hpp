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





#ifndef CPMPPROTOCOL_HPP_
#define CPMPPROTOCOL_HPP_

#include <map>

#include "utils/misc/include/Logger.hpp"
#include "utils/threads/include/CMutex.hpp"
#include "utils/threads/include/CSignalSemaphore.hpp"
#include "utils/threads/include/CCondVar.hpp"
#include "framework/components/ChannelSupervisor/Tube/include/IChannelSupervisorObserver.hpp"
#include "framework/components/ChannelSupervisor/Tube/include/ChannelSupervisorTube.hpp"

#include "framework/components/ProfileManager/PMP/interaction/include/IPmpProtocol.hpp"

namespace AXIS
{

   namespace PMP
   {

      class CPmpCoreClbCtrl;
      class CPmpPimClbCtrl;

      /**
       * Class that implements PMP Protocol, implements two interfaces:
       * IChannelSupervisorTubeObserver and IPmpProtocol interface
       */
      class CPmpProtocol : public ChannelSupervisor::IChannelSupervisorTubeObserver, public IPmpProtocol
      {
         friend class CPmpProtoThread;
      public:
         // from IPmpProtocol

         /**
          * Function is used to make requests from PMP Core
          */
         virtual PMPFrame * makeCoreRequest(PMPFrame & request);

         /**
          * Function is used to make requests from PMP PIM
          */
         virtual PMPFrame * makePimRequest(PMPFrame & request);

         /**
          * Function is used for making responses on another side
          */
         virtual void makeResponse(PMPFrame & response);

         /**
          * Functions registers PMP Core Callback
          */
         virtual void registerCoreClb(IPmpCoreClb * pCoreClb);

         /**
          * Function registers PMP PIM Callback
          */
         virtual void registerPimClb(IPmpPimClb * pPimClb);

      public:
         /**
          * Constructor
          */
         CPmpProtocol();
         /**
          * Destructor
          */
         virtual ~CPmpProtocol();

         /**
          * Function is used to say thar PMP is ready to receive requests from anothre side
          * @return true if OK, false if connection error
          */
         bool ready();

         /**
          * Function is used to know if anothre side is ready to receive our requests
          */
         bool isReady() const;

      private:
         // from ChannelSupervisor::IChannelSupervisorTubeObserver

         virtual void dataReceivedCallback(const unsigned int channelId, const unsigned int readSize, const unsigned int freeSize);

         virtual void bufferOverflowCallback(const unsigned int channelId);

         virtual void channelDeletedCallback(const unsigned int channelId);

         virtual void connectionLostCallback();

      private:

         /**
          * Is used for request needed for PMP Conmponent manager
          */
         PMPFrame * makeCmpMgrRequest(PMPFrame & request);

         /**
          * Callback, is used to react on Component manager requests
          */
         void onCmpMgrRequest(PMPFrame * pFrame);

         /**
          * Connects with Channel supervisor
          */
         void connect();

         /**
          * Disconnects
          * 
          * @param final if true, then connection will not be reestablished
          */
         void disconnect(bool final);

         UInt32 getChannelId() const;


         /**
          * Generates message ID
          */
         tMessageId generateId ();

         /**
          * Makes request on another side
          */
         PMPFrame * makeRequest(PMPFrame & requestFrame);

         /**
          * Callback, is used to react on requests from another side
          */
         void onRequest(PMPFrame * pFrame);

         /**
          * Callback, is used to react on Core requests
          */
         void onCoreRequest(PMPFrame * pFrame);

         /**
          * Callback, is used to react on PIM requests
          */
         void onPimRequest(PMPFrame * pFrame);

         /**
          * Callback, is used to react on responses
          */
         void onResponse(PMPFrame * pFrame);

         volatile unsigned int mChannelId;      ///< ID of Channel Supervisor's channel for PMP
         mutable CCondVar mChannelIdCond;

         bool mBe;                              ///< if false - stop process
         CSignalSemaphore mConnectionLostSem;   ///< Semaphore used to lock connection thread

         std::string mTag;                      ///< Tag of channel
         std::map<tMessageId, PMPFrame*> mResponseMap;      ///< Responce map
         std::map<tMessageId, CSignalSemaphore*> mWaitMap;  ///< Map with semaphores for responces
         CMutex mRequestMutex;            ///< Mutex is used for making requests
         tMessageId mGenId;               ///< is used by generateId()
         CMutex mGenIdMutex;
         CPmpCoreClbCtrl * mpCoreClbCtrl; ///< pointer co PMP Core Callback Controller
         CPmpPimClbCtrl * mpPimClbCtrl;   ///< pointer to PMP PIM Callback Controller

         volatile bool mReady;            ///< is true if PMP on another side has been inited
         mutable CMutex mReadyMutex;

         static Logger msLogger;          ///< object of logger
      };

   }

}

#endif /* CPMPPROTOCOL_HPP_ */
