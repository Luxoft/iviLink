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
 * @file             CDummyConnectionFinder.hpp
 * @brief            CDummyConnectionFinder class header file
 * @author           PIlin
 ***************************************************************************/

#ifndef CDUMMYCONNECTIONFINDER_HPP_
#define CDUMMYCONNECTIONFINDER_HPP_

/********************************************************************
 *
 * Other includes
 *
 ********************************************************************/
#include "CConnectionFinder.hpp"

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
    * @param pSem - semaphore used to signal about found connection
    * @param isServer - flag is true when system is server-side.
    */
   CDummyConnectionFinder(CSignalSemaphore* sem, bool isServer);

   /**
    * Destructor
    */
   virtual ~CDummyConnectionFinder();

protected:
   // Methods section

   /**
    * Method that will work in separate thread.
    */
   virtual void threadFunc();

};

#endif
