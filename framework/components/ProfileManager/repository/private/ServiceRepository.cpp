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


#include <cstdlib>

#include "ServiceRepository.hpp"
#include "configurator.h"
#include "pugixml.hpp"
#include "Exit.hpp"

namespace iviLink
{

namespace PMP
{

ServiceRepository::ServiceRepository(conf::Configurator * configurator)
    : mLogger(Logger::getInstance(LOG4CPLUS_TEXT("profileManager.Repository")))
    , mConfigurator(configurator)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
    init();
    printRepository();
}

ServiceRepository::~ServiceRepository()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
    deinit();
}

void ServiceRepository::init()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
    std::string repoPath;
#ifndef ANDROID
    char buf[256];
    getcwd(buf,255);
    repoPath = std::string(buf)+"/../database/services/";
#else
    repoPath = mConfigurator->getParam("path_to_services");
#endif //ANDROID

    std::string dbPath = repoPath
            #ifndef ANDROID
            + "HeadUnitServices.xml";
            #else
            + "AndroidServices.xml";
            #endif //ANDROID

    LOG4CPLUS_INFO(mLogger, "services xml path = " + repoPath);
    LOG4CPLUS_INFO(mLogger, "DB path = " + dbPath);

    pugi::xml_document doc;

    pugi::xml_parse_result result = doc.load_file(dbPath.c_str());
    if (result)
    {
        pugi::xml_node device_node = doc.child("device");
        for (pugi::xml_node service_node = device_node.child("service");
                service_node; service_node = service_node.next_sibling("service"))
        {
            if (strcmp(service_node.attribute("name").value(), "") != 0)
            {
                Service::Uid sid = Service::Uid(service_node.attribute("name").value());
                std::string maniPath = repoPath + sid.value() + std::string(".xml");
                UInt32 prio = atoi(service_node.attribute("priority").value());
                mServices.insert(std::make_pair(sid,new ServiceInfo(maniPath, prio)));
            }
        }
    }
    else
    {
        LOG4CPLUS_FATAL(mLogger, "Service repository load error: " +
                std::string(result.description()));
        killProcess(1);
    }
}

void ServiceRepository::deinit()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
    for (ServiceInfoMap::iterator it = mServices.begin();
            mServices.end() != it; ++it)
    {
        delete it->second;
    }
    mServices.clear();
}

bool ServiceRepository::hasService(Service::Uid service) const
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
    ServiceInfoMap::const_iterator it = mServices.find(service);
    if (mServices.end() == it)
    {
        return false;
    }
    return true;
}

const ServiceInfo * ServiceRepository::getServiceInfo(Service::Uid service) const
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
    ServiceInfoMap::const_iterator it = mServices.find(service);
    if (mServices.end() == it)
    {
        return NULL;
    }
    return it->second;
}

void ServiceRepository::printRepository() const
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );

    for (ServiceInfoMap::const_iterator it = mServices.begin();
            mServices.end() != it; ++it)
    {
        it->second->print();
    }
}

} /* namespace PMP */
} /* namespace iviLink */
