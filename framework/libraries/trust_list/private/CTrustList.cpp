/* 
 * 
 * iviLINK SDK, version 1.0.1
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





#include <algorithm>
#include <vector>
#include <unistd.h>

#include "utils/misc/CUid.hpp"

#include "CTrustListError.hpp"

#include "CTrustList.hpp"
#include "CFileStorage.hpp"
#include "get_mac.hpp"

using namespace iviLink;

const char gsStorageFileName[] = "trust.db";

Logger CTrustList::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("profiles.CAuthenticationProfileImpl.trust.CTrustList"));

iviLink::CUid CTrustList::getOurUid() const
{
	CUid uid;
	CError err = mpStorage->getOurUid(uid);
	if (!err.isNoError())
	{
		LOG4CPLUS_WARN(msLogger, static_cast<std::string>(err));
		uid = generateNewUid();
		err = mpStorage->setOurUid(uid);
		if (!err.isNoError())
			LOG4CPLUS_ERROR(msLogger, static_cast<std::string>(err));
	}

	return uid;
}

bool CTrustList::isKnownUid(iviLink::CUid const& uid) const
{
	tUidVector vec;
	CError err = mpStorage->readAll(vec);
	if (!err.isNoError())
	{
		LOG4CPLUS_ERROR(msLogger, static_cast<std::string>(err));
		return false;
	}

	return std::find(vec.begin(), vec.end(), uid) != vec.end();
}

CTrustListError CTrustList::addUid(iviLink::CUid const& uid)
{
	return mpStorage->insert(uid);
}

CTrustListError CTrustList::removeUid(iviLink::CUid const& uid)
{
	return mpStorage->remove(uid);
}

CTrustListError CTrustList::clearList()
{
	return mpStorage->clear();
}

CTrustListError CTrustList::getKnownUids(tUidVector& result) const
{
	return mpStorage->readAll(result);
}

CTrustList::CTrustList() :
	mpStorage(new CFileStorage(gsStorageFileName))
{
	CError err = mpStorage->connect();
	if (!err.isNoError())
		LOG4CPLUS_ERROR(msLogger, static_cast<std::string>(err));
}

CTrustList::~CTrustList()
{
	delete mpStorage;
}


CUid CTrustList::generateNewUid() const
{
	char hostid[30] = "";
	char mac[13] = "";
	get_mac(mac);
	snprintf(hostid, sizeof(hostid), "%08x-%12s", (UInt32)(gethostid() & 0xffffffff), mac);

	return CUid(hostid);
}
