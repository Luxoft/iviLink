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


#include <vector>

#include "BaseUid.hpp"
#include "CTrustListError.hpp"

namespace iviLink
{

class ITrustListStorage
{
public:

	virtual CTrustListError connect() = 0;
	virtual CTrustListError disconnect() = 0;

	virtual bool isReady() const = 0;

	typedef std::vector<iviLink::BaseUid> tUidVector;
	virtual CTrustListError readAll(tUidVector& uids) = 0;

	virtual CTrustListError insert(BaseUid const& uid) = 0;
	virtual CTrustListError remove(BaseUid const& uid) = 0;

	virtual CTrustListError clear() = 0;

	virtual CTrustListError getOurUid(BaseUid& uid) = 0;
	virtual CTrustListError setOurUid(BaseUid const& uid) = 0;

	virtual ~ITrustListStorage() {}
};

}
