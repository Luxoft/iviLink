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

#ifndef COMMON_H_
#define COMMON_H_

#include <string>
#include <ctime>

namespace AXIS {
	namespace ChannelSupervisor {

		enum CS_ERROR 
		{
			CS_SUCCESS = 0,
			CS_ERR_OTHER = 1000,
		};

		struct UID 
		{
			int uid;

			UID() : uid(0) { }

			UID(int a)
			{
				uid = a;
			}
		};

		struct ChannelId
		{
			unsigned int caChannelId;

			ChannelId() : caChannelId(0) { }
		};
	}
}

#endif /* COMMON_H_ */
