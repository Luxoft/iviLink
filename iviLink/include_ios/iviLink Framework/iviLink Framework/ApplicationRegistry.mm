/**
 *
 */

/**
 * @ingroup iOS Port
 * @author Bratanova Elena <ebratanova@luxoft.com>
 * @date 26.03.2013
 *
 * All "living" applications in the process can be obtained here.
 */

#include <cassert>

#include "ApplicationRegistry.hpp"

using namespace iviLink::App;

Logger ApplicationRegistry::mLogger(Logger::getInstance("iviLink::App::iOS::ApplicationRegistry"));

ApplicationRegistry::ApplicationRegistry()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
}

ApplicationRegistry::~ApplicationRegistry()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
}

ApplicationRegistry * ApplicationRegistry::getInstance()
{
    static ApplicationRegistry * instance;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        instance = new ApplicationRegistry();
    });
    return instance;
}

void ApplicationRegistry::registerApplication(Application * app)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    MutexLocker lock(mAppSetMutex);
    mApplications.insert(app);
}

void ApplicationRegistry::deregisterApplication(Application * app)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    MutexLocker lock(mAppSetMutex);
    tAppSet::iterator iter = mApplications.find(app);
    if (iter != mApplications.end())
    {
        mApplications.erase(iter);
    }
    else
    {
        LOG4CPLUS_WARN(mLogger, "Application not found!");
    }
}

void ApplicationRegistry::sendLinkUpNotification()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    MutexLocker lock(mAppSetMutex);
    tAppSet::iterator iter = mApplications.begin();
    for (; iter != mApplications.end(); iter ++)
    {
        (*iter)->linkUpNotification();
    }
}

void ApplicationRegistry::sendLinkDownNotification()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    MutexLocker lock(mAppSetMutex);
    tAppSet::iterator iter = mApplications.begin();
    for (; iter != mApplications.end(); iter ++)
    {
        (*iter)->linkDownNotification();
    }
}

iviLink::Application * ApplicationRegistry::getApplicationWithEnabledService(iviLink::Service::Uid const& service)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    MutexLocker lock(mAppSetMutex);
    tAppSet::iterator iter = mApplications.begin();
    for (; iter != mApplications.end(); iter ++)
    {
        if ((*iter)->isEnabled(service))
        {
            return (*iter);
        }
    }
    LOG4CPLUS_WARN(mLogger, "Application with enabled service: " + service.value() + " not found");
    return NULL;
}


iviLink::Application * ApplicationRegistry::getApplicationWithActiveService(iviLink::Service::Uid const& service)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    MutexLocker lock(mAppSetMutex);
    tAppSet::iterator iter = mApplications.begin();
    for (; iter != mApplications.end(); iter ++)
    {
        if ((*iter)->isActive(service))
        {
            return (*iter);
        }
    }
    LOG4CPLUS_WARN(mLogger, "Application with active service: " + service.value() + " not found");
    return NULL;
}