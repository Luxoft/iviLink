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


#ifndef TYPES_HPP
#define TYPES_HPP


/**
 * 8-bit signed value
 */
typedef char            Int8;

/**
 * 8-bit unsigned value
 */
typedef unsigned char   UInt8;

/**
 * 16-bit signed value
 */
typedef short           Int16;

/**
 * 16-bit unsigned value
 */
typedef unsigned short  UInt16;

/**
 * 32-bit signed value
 */
typedef int             Int32;

/**
 * 32-bit unsigned value
 */
typedef unsigned int    UInt32;

/**
 * 64-bit unsigned value
 */
typedef unsigned long long UInt64;

/**
 * 64-bit signed value
 */
typedef long long Int64;

/**
 * Number of data source priorities in AXIS
 */
const UInt8    PRIO_NUM                   = 3;

/**
 * Maximum buffer size
 */
const UInt32   MAX_SIZE                   = 4096;

const UInt32 CHANNEL_ID_START_NUMBER    = 100;

/**
 * Number of service channel
 */
const UInt32   CA_SERVICE_CHANNEL     = 0;

/**
 * Channel supervisor reserved channel
 */
const UInt32   CS_SERVICE_CHANNEL         = 1;

/**
 * Default port for TCP connection
 */
//const char AXIS_PORT[] = "40000";
const char AXIS_PORT[] = "45000";

/**
 * Default port for CTCPAutoConnectionFinder
 */
//const int AXIS_TCP_AUTO_CONNECTION_FINDER_BRD_PORT = 49789;
const int AXIS_TCP_AUTO_CONNECTION_FINDER_BRD_PORT = 45789;


/**
 * Default size of buffer queues for transmission and receiving
 */
  const UInt32   QUEUE_SIZE     = 200;

/**
* Time before frame must be retransmitted
*/
const UInt64 RETRANSMIT_TIME_NSEC = 5LLU * 1000 * 1000 * 1000;

/**
 * Data source priorities
 */
enum TChannelPriority
{
    /**
     * Only for system channels
     */
    eSafetyCritical = 0,

    /**
     * Priorities for applications
     */
    eRealTime = 1,
    ePlainData = 2
};

enum ERROR_CODE
{
    ERR_OK                     = 1,
    ERR_FAIL                   = 2,
    ERR_NUMBER_BUSY            = 3,
    ERR_CONNECTION_LOST        = 4,
    ERR_DISCONNECTED           = 5,
    ERR_NOTFOUND               = 6,
    ERR_TIMEOUT                = 7,
    ERR_WRONG_SEQUENCE         = 8,
    ERR_AGAIN                  = 9,
    ERR_WRONG_FRAME_SEQUENCE   = 10,
    ERR_IN_PROGRESS            = 11,
    ERR_REASSIGN               = 12,
    ERR_XML_PARSING            = 13,
    ERR_RESEND_ACK             = 14,
    ERR_DEFERRED               = 15,
    ERR_UNKNOWN                = 255
};
/**
 * List of available types of carriers
 */
enum ECarrierType
{
    eDummyTcpCarrier,    //!< eDummyTcpCarrier - TCP carrier for testing purposes
    eTcpAutoCarrier,     //!< eTcpAutoCarrier - TCP carrier, detected by CTcpAutoConnectionFinder
    eLastCarrier         //!< eLastCarrier - special value used for compile time counting of carrier types. Must be last in enum.
};

enum CALLBACKTYPE
{
    DATA_RECEIVED,
    BUFF_OVERFLOW,
    CHANN_DELETED,
    CONNECTI_LOST
};

#endif // TYPES_HPP
