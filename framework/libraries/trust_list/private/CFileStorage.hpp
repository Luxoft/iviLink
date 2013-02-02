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


#include <string>
#include <fstream>
#include "ITrustListStorage.hpp"

#include "Logger.hpp"

namespace iviLink
{

class CFileStorage : public ITrustListStorage
{
public:
	// from ITrustListStorage
	virtual CTrustListError connect();
	virtual CTrustListError disconnect();
	virtual bool isReady() const;
	virtual CTrustListError readAll(tUidVector& uids);
	virtual CTrustListError insert(BaseUid const& uid);
	virtual CTrustListError remove(BaseUid const& uid);
	virtual CTrustListError clear();
	virtual CTrustListError getOurUid(BaseUid& uid);
	virtual CTrustListError setOurUid(BaseUid const& uid);

public:

	CFileStorage(std::string const& pathToFile);
	~CFileStorage();

private:

	bool reopen(std::ios_base::openmode falgs);

	CFileStorage(CFileStorage&);
	CFileStorage& operator=(CFileStorage&);

private:

	std::string mFilename;
	std::fstream file;

public: 
	static Logger msLogger;
};

} // namespace iviLink
