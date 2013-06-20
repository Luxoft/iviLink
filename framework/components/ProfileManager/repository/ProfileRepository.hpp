/*
 *
 * iviLINK SDK, version 1.1.2
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
 *
 */


/**
 * @file                ProfileRepository.hpp
 * @ingroup             Profile Manager
 * @author              Plachkov Vyacheslav <vplachkov@luxoft.com>
 * @date                10.01.2013
 *
 * Repository class provides access to PMP repository for other PMP subcomponents
 */


#ifndef PROFILE_REPOSITORY_HPP_
#define PROFILE_REPOSITORY_HPP_

#include <string>
#include <list>

#include "RepositoryTypes.hpp"
#include "RepositoryError.hpp"
#include "Logger.hpp"
#include "configurator.h"

namespace iviLink
{

namespace PMP
{

class ProfileApiDatabase;
class ProfileDatabase;

class ProfileRepository
{
public:

    ProfileRepository(conf::Configurator * configurator);

    ~ProfileRepository();

    /**
     * Addes Profile API to repository
     * @param manifestPath contains path of manifest XML
     * @return CError code
     */
    BaseError addProfileApi(std::string manifestPath);

    /**
     * Removes profile API from repository
     * @param profileApiUid string is UID of Profile API
     * @return CError code
     */
    BaseError removeProfileApi(Profile::ApiUid profileApiUid);

    /**
     * @param[out] libs contains all profile libs
     */
    void getAllLibs(std::list<LibInfo> & libs);

    /**
     * Counts relevance between request and profile
     * @return value in range 0...100
     */
    int getRelevance(const Profile::Uid & profile, const UInt32 version, const std::map<std::string, std::string> & profileArguments) const;

private:
    ProfileApiDatabase * mProfileApiDatabase;
    ProfileDatabase * mProfileDatabase;
    conf::Configurator * mConfigurator;

    static Logger msLogger; ///< object of logger
};

} /* namespace PMP */
} /* namespace iviLink */
#endif /* PROFILE_REPOSITORY_HPP_ */
