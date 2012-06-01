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

/*
 * DeallocateRequest.hpp
 *
 *  Created on: Mar 14, 2012
 *      Author: mprosuntsov
 */

#ifndef DEALLOCATEREQUEST_HPP_
#define DEALLOCATEREQUEST_HPP_

#include "pugixml.hpp"
#include "Request.hpp"
#include "Message.hpp"


class DeallocateRequest: public Request
{
public:

	DeallocateRequest( const char* tag, const unsigned int channelId );

	DeallocateRequest(pugi::xml_document* doc);

	virtual ~DeallocateRequest()
	{
	}

	const std::string& GetTag() const
	{
		return m_tag;
	}

	const int GetChannelId() const
	{
		return m_offerredChId;
	}


	virtual const char* GetMessageName() const { return m_requestTypes[REQUESTTYPE_DEALLOCATE_CHANNEL].c_str(); }


protected:

	std::string m_tag;
	int			m_offerredChId;
};

#endif /* DEALLOCATEREQUEST_HPP_ */
