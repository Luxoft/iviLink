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


#ifndef SERVICEREPOSITORY_HPP_
#define SERVICEREPOSITORY_HPP_

#include <map>

#include "UID.hpp"
#include "ServiceInfo.hpp"
#include "Logger.hpp"

namespace iviLink
{

namespace conf
{
class Configurator;
}

namespace PMP
{

class ServiceRepository
{
public:
    typedef std::map<Service::Uid, ServiceInfo*> ServiceInfoMap;

    ServiceRepository(conf::Configurator * configurator);
    ~ServiceRepository();

    bool hasService(Service::Uid service) const;
    const ServiceInfo * getServiceInfo(Service::Uid service) const;

    void printRepository() const;

private:

    //not implemented
    ServiceRepository(const ServiceRepository &);
    ServiceRepository & operator = (const ServiceRepository &);

    void init();
    void deinit();

    ServiceInfoMap mServices;
    Logger mLogger;
    conf::Configurator * mConfigurator;
};

} /* namespace PMP */
} /* namespace iviLink */

#endif /* SERVICEREPOSITORY_HPP_ */
