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

/***************************************************************************
 * Project           AXIS
 * (c) copyright     2011
 * Company           LUXOFT
 * @file             CConnectionFinder.hpp
 * @brief            CConnectionFinder class header file
 * @author           PIlin
 ***************************************************************************/

#ifndef CCONNECTIONFINDER_HPP_
#define CCONNECTIONFINDER_HPP_

/********************************************************************
 *
 * Other includes
 *
 ********************************************************************/
#include "utils/threads/include/CThread.hpp"
#include "utils/threads/include/CSignalSemaphore.hpp"
#include "utils/threads/include/CRWMutex.hpp"

/********************************************************************
 *
 * Forward declarations
 *
 ********************************************************************/
class CCarrierAdapter;

/// CConnectionFinder class
/**
 * CConnectionFinder is the base class for other Finder classes. Connection finder is concerned with
 * detection of possible connection and creation of corresponding CCarierAdapter. Finder is not
 * concerned with handshaking or other more sophisticated behavior.
 *
 * CConnectionFinder can use passed CSignalSemaphore to signal that connection is found.
 * @todo need to implement real usage of this CSignalSemapore.
 */
class CConnectionFinder: public CThread
{
private:
   static Logger logger;

public:
   // Methods section

   /**
    * Getter method for created object of CCarrierAdapter with basic connection established.
    * Connection established, but real checks of AXIS on other side is not performed.
    *
    * CConnectionFinder gives away CCarrierAdapter object and forgets about it, so taker must destroy
    * CCarrierAdapter correctly.
    * Also, sets the member mIsReady to false.
    * @return object of CCarrierAdapter or NULL.
    */
   CCarrierAdapter* getConnectedAdapter();

   /**
    * Check if CConnectionFinder found some connection and ready to return CCarrierAdapter
    * @return true if CConnectionFinder is ready.
    */
   bool isReady() const;

protected:
   // Members section

   /// Semaphore used to signal about found connection
   CSignalSemaphore* mpConnAgSigSem;

   mutable CRWMutex mMutex;

   /// Flag used to inform that connection is found
   bool mIsReady;

   /// Flag used to distinguish server- and client-side application.
   bool mIsServer;

   /// CCarrierAdapter used to make connection
   CCarrierAdapter* mpAdapter;

protected:
   // Methods section

   /**
    * Constructor
    * @param pSem - semaphore used to signal about found connection
    * @param isServer - flag is true when system is server-side.
    */
   CConnectionFinder(CSignalSemaphore* pSem, bool isServer);

   /**
    * Destructor
    */
   virtual ~CConnectionFinder();

   /**
    * Sets the member mIsReady to true and mpAdapter to passed adapter.
    * Also, makes a signal through semaphore.
    * @param pAdapter - found adapter
    */
   void setReady(CCarrierAdapter* pAdapter);

private:
   // Methods section

   /**
    * Copying constructor.
    * Unimplemented to avoid misusing.
    * @param classRef The reference to the object the class is constructing on.
    */
   CConnectionFinder(const CConnectionFinder& classRef);

   /**
    * Assigning operator.
    * Unimplemented to avoid misusing.
    * @param classRef The reference to the object the class is assigning from.
    */
   CConnectionFinder& operator=(const CConnectionFinder& classRef);
};

#endif
