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











#ifndef IUNIXSOCKETSUBSCRIBER_HPP_
#define IUNIXSOCKETSUBSCRIBER_HPP_

/********************************************************************
 *
 * Other includes
 *
 ********************************************************************/
#include "Types.hpp"

class CUnixSocket;

/// IUnixSocketSubscriber interface
/**
 * CUnixSocked uses this interface to pass received data, signal about new connection or connection lost.
 */
class IUnixSocketSubscriber
{
public:
   // Methods section

   /**
    * Called by CUnixSocket separate thread on data receive.
    * @param pSender is the pointer to CUnixSocket called this method
    * @param pData is the pointer to data array
    * @param size is the size of data array
    */
   virtual void onReceive(CUnixSocket* pSender, const UInt8* pData, UInt32 size) = 0;

   /**
    * Called by CUnixSocket separate thread on data receive. Implementor must check its readiness to
    * receive data.
    * @param pSender is the pointer to CUnixSocket called this method
    * @param size is the size of received data array
    * @return true if implementor is ready to receive data
    */
   virtual bool isReadyToReceiveData(CUnixSocket* pSender, UInt32 size) = 0;

   /**
    * Called by CUnixSocket on connection lost.
    * @param pSender is the pointer to CUnixSocket called this method
    */
   virtual void onConnectionLost(CUnixSocket* pSender) = 0;

   /**
    * Called by the listening CUnixSocket on new connection accepted
    * @param pSender is the pointer to the listening CUnixSocket called this method
    * @param newSock is the pointer to the new instance of accepted connected CUnixSocket
    */
   virtual void onIncomingConnection(CUnixSocket* pSender, CUnixSocket* pNewSocket) = 0;

};

#endif /* IUNIXSOCKETSUBSCRIBER_HPP_ */
