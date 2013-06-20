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







#include <algorithm>
#include <vector>
#include <ctime>
#include <unistd.h>

#include "BaseUid.hpp"

#include "CTrustListError.hpp"

#include "CTrustList.hpp"
#include "CFileStorage.hpp"
#include "get_mac.hpp"

using namespace iviLink;

Logger CTrustList::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("profiles.CAuthenticationProfileImpl.trust.CTrustList"));

iviLink::BaseUid CTrustList::getOurUid() const
{
	LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
	BaseUid uid;
	BaseError err = mpStorage->getOurUid(uid);
	if (!err.isNoError())
	{
		LOG4CPLUS_WARN(msLogger, static_cast<std::string>(err));
		uid = generateNewUid();
		LOG4CPLUS_INFO(msLogger, "Our newly-generated uid is: " + uid.value());
		err = mpStorage->setOurUid(uid);
		if (!err.isNoError())
			LOG4CPLUS_ERROR(msLogger, static_cast<std::string>(err));
	}
	else
	{
		LOG4CPLUS_INFO(msLogger, "Our stored uid is: " + uid.value());		
	}

	return uid;
}

std::string CTrustList::getOurName() const
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    std::string name;
    BaseError err = mpStorage->getOurName(name);
    if (!err.isNoError())
    {
        LOG4CPLUS_WARN(msLogger, static_cast<std::string>(err));
        name = "Generated name" + getOurUid().value();
        LOG4CPLUS_INFO(msLogger, "Our newly-generated name is: " + name);
        err = mpStorage->setOurName(name);
        if (!err.isNoError())
        {
            LOG4CPLUS_ERROR(msLogger, static_cast<std::string>(err));
        }
    }
    else
    {
        LOG4CPLUS_INFO(msLogger, "Our stored name is: " + name);		
    }
    return name;
}

bool CTrustList::isKnownUid(iviLink::BaseUid const& uid) const
{
	LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
	tNameUidVector vec;
	tNameUidVector::iterator i;
	BaseError err = mpStorage->readAll(vec);
	if (!err.isNoError())
	{
		LOG4CPLUS_ERROR(msLogger, static_cast<std::string>(err));
		return false;
	}
	LOG4CPLUS_INFO(msLogger, "Looking for: " + uid.value());

	for(i = vec.begin(); i != vec.end(); ++i)
	{
		if(uid == i->first) return true;
	}
	return false;
}

CTrustListError CTrustList::addUid(iviLink::BaseUid const& uid)
{
	LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
	return mpStorage->insert(uid);
}

CTrustListError CTrustList::addUid(iviLink::BaseUid const& uid, std::string const& name)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    CTrustListError err = mpStorage->insert(uid);
    if(!err.isNoError())
    {
        return err;
    }
    return mpStorage->updateName(uid, name);
}
   
CTrustListError CTrustList::getLastConnection(iviLink::BaseUid& uid, time_t &timestamp)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    return mpStorage->getLastConnection(uid, timestamp);
}

CTrustListError CTrustList::setLastConnection(iviLink::BaseUid const& uid, time_t const &timestamp)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    return mpStorage->setLastConnection(uid, timestamp);
}

CTrustListError CTrustList::setLastConnection(iviLink::BaseUid const& uid)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    return mpStorage->setLastConnection(uid, time(NULL));
}

CTrustListError CTrustList::removeUid(iviLink::BaseUid const& uid)
{
	LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
	return mpStorage->remove(uid);
}

CTrustListError CTrustList::clearList()
{
	LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
	return mpStorage->clear();
}

CTrustListError CTrustList::getKnownUids(tNameUidVector& result) const
{
	LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
	return mpStorage->readAll(result);
}

CTrustList::CTrustList(std::string pathToStorage) :
	mpStorage(new CFileStorage(pathToStorage.c_str()))
{
	LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
	BaseError err = mpStorage->connect();
	if (!err.isNoError())
		LOG4CPLUS_ERROR(msLogger, static_cast<std::string>(err));
}

CTrustList::~CTrustList()
{
	LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
	delete mpStorage;
}

BaseUid CTrustList::generateNewUid() const
{
	LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
	char hostid[30] = "";
	char mac[13] = "";
	get_mac(mac);
#ifndef ANDROID
	snprintf(hostid, sizeof(hostid), "%08x-%12s", (UInt32)(gethostid() & 0xffffffff), mac);
	return BaseUid(hostid);
#else
   // gethostid() is not avaliable on Android
	return BaseUid(mac);
#endif //ANDROID	
}
