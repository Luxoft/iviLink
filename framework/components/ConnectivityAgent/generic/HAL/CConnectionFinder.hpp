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


#ifndef CCONNECTIONFINDER_HPP_
#define CCONNECTIONFINDER_HPP_

/********************************************************************
 *
 * Other includes
 *
 ********************************************************************/
#include "CThread.hpp"
#include "CSignalSemaphore.hpp"
#include "CCondVar.hpp"
#include "EGenderType.hpp"
#include "testable.hpp"
/********************************************************************
 *
 * Forward declarations
 *
 ********************************************************************/

namespace iviLink
{
namespace ConnectivityAgent
{
namespace HAL
{

class IFoundConnectionHandler;
class CCarrierAdapter;

/// CConnectionFinder class
/**
 * CConnectionFinder is the base class for other Finder classes.
 * Connection finder is concerned with detection of possible connection
 * and creation of corresponding CCarierAdapter.
 * Finder is not concerned with handshaking or other more sophisticated
 * behavior.
 *
 * CConnectionFinder can use passed CSignalSemaphore to signal that connection is found.
 * @todo need to implement real usage of this CSignalSemapore.
 */
class CConnectionFinder: public CThread
{
private:
   static Logger msLogger;

public:
   // Types section

   /**
    * State of connection searching process
    */
   enum EState
   {
      eSearching,//!< eSearching connection finder performs searching
      eWaitAck,  //!< eWaitAck connection finder found possible connection and
                 //!< awaits the decision from connectivity agent
      eStopped   //!< eStopped connection finder stopped and does nothing
   };

public:
   // Methods section

   /**
    * Destructor
    */
   virtual ~CConnectionFinder();

   /**
    * Getter method for created object of CCarrierAdapter with basic connection
    * established.
    * Connection established, but real handshake is not performed.
    *
    * CConnectionFinder gives away CCarrierAdapter object and forgets about it,
    * so taker must destroy CCarrierAdapter correctly.
    *
    * @retval object of CCarrierAdapter if state is eWaitAck
    * @retval NULL in case if state is @c eSearching or @c eStopped
    */
   CCarrierAdapter* getConnectedAdapter();

   EState getState() const;

   /**
    *
    */
   void startSearch();

   /**
    *
    */
   void stopSearch();

   /**
    *
    */
   void continueSearch();

protected:
   // Methods section

   /**
    * Constructor
    * @param handler object which will process found connections
    * @param gender gender of connection
    */
   CConnectionFinder(IFoundConnectionHandler& handler, EGenderType gender);


   /**
    * Sets the member mIsReady to true and mpAdapter to passed adapter.
    * Also, makes a signal through semaphore.
    * @param pAdapter - found adapter
    */
   void setReady(CCarrierAdapter* pAdapter);

   virtual void performSearch() = 0;

protected:
   // Members section

   const EGenderType mGenderType;

private:
   // from CThread
   // Next methods are hidden

   virtual void threadFunc();

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

private:
   // Members section
   IFoundConnectionHandler& mFoundConnectionHandler;

   mutable CCondVar mStateCondVar;

   EState mState;

   /// CCarrierAdapter used to make connection
   CCarrierAdapter* mpAdapter;

   IVILINK_TESTABLE(CConnectionFinder)
};

}  // namespace HAL
}  // namespace ConnectivityAgent
}  // namespace AXIS
#endif
