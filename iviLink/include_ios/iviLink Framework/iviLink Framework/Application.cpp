/**
 *
 */

/**
 * @ingroup iOS Port
 * @author Bratanova Elena <ebratanova@luxoft.com>
 * @date 26.03.2013
 *
 * Similar to Ubuntu's and Android's application in functionality.
 * Important: getLaunchInfo is just a stub for compatibility;
 * onInitDone is also left for compatibility and is never called.
 */

#include "CMutex.hpp"
#include "ServiceHolder.hpp"

#include "Application.hpp"
#include "SystemControllerStub.hpp"
#include "CServiceManager.hpp"

#include "ApplicationRegistry.hpp"
#include "Database.hpp"
#include "PmpPIM.hpp"

using namespace iviLink;
using namespace iviLink::PMP;

Logger Application::mLogger = Logger::getInstance("iviLink::App::iOS::Application");

Application::Application(iviLink::Android::AppInfo) :
mSavedError(Error::NoError())
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    mIsInited = false;
    initQueues();
}

Application::Application(const Service::Uid &service, iviLink::Android::AppInfo) :
mSavedError(Error::NoError())
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    LOG4CPLUS_INFO(mLogger, "Service: " + service.value());
    mSupportedServices.push_back(service);
    mIsInited = false;
    initQueues();
}

Application::Application(const Service::ListOfUids &services, iviLink::Android::AppInfo) :
mSavedError(Error::NoError())
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    for (Service::ListOfUids::const_iterator iter = services.begin(); iter != services.end();
         iter++)
    {
        LOG4CPLUS_INFO(mLogger, "Service: " + (*iter).value());
    }
    mSupportedServices = services;
    mIsInited = false;
    initQueues();
}

void Application::initQueues()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    mSerialProcessingQueue = dispatch_queue_create("Application internal thread", NULL);
    mSerialCallbacksQueue = dispatch_queue_create("Application state callbacks", NULL);
}

void Application::releaseQueues()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    dispatch_release(mSerialProcessingQueue);
    dispatch_release(mSerialCallbacksQueue);
}

Application::~Application()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    App::ApplicationRegistry::getInstance()->deregisterApplication(this);
    dispatch_sync(mSerialProcessingQueue, ^
                  {
                      tServicesMap::iterator iter = mServiceHolders.begin();
                      for (; iter != mServiceHolders.end(); iter ++)
                      {
                          if (SystemController::SystemControllerStub::getInstance()->canCommunicateWithOtherSide())
                          {
                              iter->second->unload();
                          }
                          else
                          {
                              iter->second->silentUnload();
                          }
                          delete iter->second;
                          iter->second = NULL;
                      }
                      mServiceHolders.clear();
                  });
    releaseQueues();
}

void Application::initInIVILink()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    App::ApplicationRegistry::getInstance()->registerApplication(this);
    SystemController::SystemControllerStub::getInstance()->startStack();
    MutexLocker lock(mInitedMutex);
    mIsInited = true;
}

void Application::setEnabled(const Service::Uid &service, bool enable)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    LOG4CPLUS_INFO(mLogger, "Service: " + service.value());
    MutexLocker lock(mSupportedServicesMutex);
    setEnabledUnsafe(service, enable);
}

void Application::setEnabledUnsafe(const Service::Uid &service, bool enable)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    Service::ListOfUids::iterator found = std::find(mSupportedServices.begin(),
                                                    mSupportedServices.end(), service);
    if (enable)
    {
        if (found == mSupportedServices.end())
        {
            mSupportedServices.push_back(service);
        }
    } else
    {
        if (found != mSupportedServices.end())
        {
            mSupportedServices.erase(found);
        }
    }
}

void Application::setEnabled(const Service::ListOfUids &services, bool enable)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    MutexLocker lock(mSupportedServicesMutex);
    for (Service::ListOfUids::const_iterator iter = services.begin(); iter != services.end();
         iter++)
    {
        setEnabledUnsafe((*iter), enable);
    }
}

bool Application::isEnabled(const Service::Uid& service)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    LOG4CPLUS_INFO(mLogger, "Service: " + service.value());
    MutexLocker lock(mSupportedServicesMutex);
    return std::find(mSupportedServices.begin(), mSupportedServices.end(), service)
    != mSupportedServices.end();
}

ELaunchInfo Application::getLaunchInfo()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    return LAUNCHED_BY_USER;
}

void Application::registerProfileCallbacks(const Profile::ApiUid &profileApi,
                                           Profile::IProfileCallbackProxy* pCallbacks)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    LOG4CPLUS_INFO(mLogger, "registering for api: " + profileApi.value());
    App::Service::CServiceManager::getInstance()->registerProfileCallbacks(profileApi, pCallbacks);
}

Error Application::loadService(const Service::Uid &service)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    if (dispatch_get_current_queue() == mSerialProcessingQueue)
    {
        // may happen if load is called from onIncomingServiceBeforeLoading - will deadlock otherwise
        return iviLink::Error(UNKNOWN_ERROR, "Trying to load service on the internal queue", false);
    }
    LOG4CPLUS_INFO(mLogger, "loading service: " + service.value());
    {
        MutexLocker lock(mInitedMutex);
        if (!mIsInited)
        {
            LOG4CPLUS_ERROR(mLogger, "application not inited!");
            return iviLink::Error(INITIALIZATION_ERROR, "app not inited!", false);
        }
    }
    if (!SystemController::SystemControllerStub::getInstance()->canCommunicateWithOtherSide())
    {
        LOG4CPLUS_ERROR(mLogger, "loadService:: cannot communicate with the other side!");
        return iviLink::Error(UNKNOWN_ERROR, "cannot communicate with the other side!", false);
    }
    setEnabled(service, true);
    tStringPtr servicePtr(new std::string(service.value()));
    
    loadServiceSync(servicePtr);
    return mSavedError;
}

void Application::loadServiceSync(tStringPtr service)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    dispatch_sync(mSerialProcessingQueue, ^
                  {
                      LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
                      if (mServiceHolders.find(*service.get()) != mServiceHolders.end())
                      {
                          LOG4CPLUS_ERROR(mLogger, "loadService:: service already loaded!");
                          mSavedError = iviLink::Error(SERVICE_ALREADY_LOADED, "Service already loaded!", false);
                          return;
                      }
                      App::ServiceHolder * serviceHolder = new App::ServiceHolder(*service.get());
                      mSavedError = serviceHolder->load();
                      if (!mSavedError.isNoError())
                      {
                          LOG4CPLUS_ERROR(mLogger, "loadService:: could not load the service " + (*service.get()));
                          delete serviceHolder;
                          return;
                      }
                      LOG4CPLUS_INFO(mLogger, "adding new service to map " + (*service.get()));
                      mServiceHolders[*service.get()] = serviceHolder;
                  });
}

Error Application::unloadService(const iviLink::Service::Uid &service)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    LOG4CPLUS_INFO(mLogger, "unloading service: " + service.value());
    {
        MutexLocker lock(mInitedMutex);
        if (!mIsInited)
        {
            LOG4CPLUS_ERROR(mLogger, "application not inited!");
            return iviLink::Error(INITIALIZATION_ERROR, "app not inited!", false);
        }
    }
    if (dispatch_get_current_queue() == mSerialProcessingQueue)
    {
        // may happen if unload is called from onIncomingServiceBeforeLoading
        return iviLink::Error(UNKNOWN_ERROR, "trying to unload from the wrong thread!", false);
    }
    tStringPtr servicePtr(new std::string(service.value()));
    unloadServiceSync(servicePtr);
    return mSavedError;
}

void Application::unloadServiceSync(tStringPtr service)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    dispatch_sync(mSerialProcessingQueue, ^
                  {
                      tServicesMap::iterator findIt = mServiceHolders.find(*service.get());
                      if (findIt == mServiceHolders.end())
                      {
                          mSavedError = iviLink::Error(SERVICE_NOT_FOUND, "Service not found", false);
                          return;
                      }
                      App::ServiceHolder * serviceHolder = findIt->second;
                      mServiceHolders.erase(findIt);
                      if (SystemController::SystemControllerStub::getInstance()->canCommunicateWithOtherSide())
                      {
                          iviLink::Error unloadError = serviceHolder->unload();
                          delete serviceHolder;
                          mSavedError = unloadError;
                      }
                      else
                      {
                          serviceHolder->silentUnload();
                          delete serviceHolder;
                          mSavedError = iviLink::Error::NoError();
                      }
                  });
}

Service::ListOfUids Application::getActiveServices()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    Service::ListOfUids result;
    if (dispatch_get_current_queue() == mSerialProcessingQueue)
    {
        // to avoid deadlocks on dispatch_sync
        LOG4CPLUS_INFO(mLogger, "getActiveServices() on processing thread");
        tServicesMap::iterator iter = mServiceHolders.begin();
        for (; iter != mServiceHolders.end(); iter++)
        {
            LOG4CPLUS_INFO(mLogger, "Has active service: " + iter->first);
            result.push_back(Service::Uid(iter->first));
        }
        return result;
    }
    
    std::tr1::shared_ptr < std::list < BaseUid >> resultPtr(new std::list<BaseUid>());
    dispatch_sync(mSerialProcessingQueue, ^
                  {
                      tServicesMap::iterator iter = mServiceHolders.begin();
                      for (; iter != mServiceHolders.end(); iter ++)
                      {
                          LOG4CPLUS_INFO(mLogger, "Has active service: " + iter->first);
                          resultPtr.get()->push_back(Service::Uid(iter->first));
                      }
                  });
    std::copy(resultPtr.get()->begin(), resultPtr.get()->end(), result.begin());
    return result;
}

bool Application::isActive(const iviLink::Service::Uid &service)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    if (dispatch_get_current_queue() == mSerialProcessingQueue)
    {
        LOG4CPLUS_INFO(mLogger, "isActive() on processing thread");
        // to avoid deadlocks
        return (mServiceHolders.find(service.value()) != mServiceHolders.end());
    }
    tStringPtr result(new std::string(""));
    tStringPtr servicePtr(new std::string(service.value()));
    dispatch_sync(mSerialProcessingQueue, ^
                  {
                      if (mServiceHolders.empty())
                      {
                          (*result.get()) = "0";
                      }
                      else
                      {
                          tServicesMap::iterator iter = mServiceHolders.find(*servicePtr.get());
                          (*result.get()) = convertIntegerToString(iter != mServiceHolders.end());
                      }
                  });
    return (*result.get()) == "1";
}

void Application::linkUpNotification()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    postLinkUpAsync();
}

void Application::postLinkUpAsync()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    dispatch_async(mSerialCallbacksQueue, ^
    {
        onLinkUp();
    });
}

void Application::linkDownNotification()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    dispatch_async(mSerialProcessingQueue, ^
    {
        for (tServicesMap::iterator iter = mServiceHolders.begin(); iter != mServiceHolders.end(); iter ++)
        {
            (*iter).second->silentUnload();
            delete (*iter).second;
        }
        mServiceHolders.clear();
        postLinkDownAsync();
    });
}

void Application::postLinkDownAsync()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    dispatch_async(mSerialCallbacksQueue, ^
    {
        onPhysicalLinkDown();
    });
}

void Application::processIncomingCreateRequest(std::string const& service,
                                               std::string const& profile, std::string const& piuid)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    tStringPtr servicePtr(new std::string(service));
    tStringPtr profilePtr(new std::string(profile));
    tStringPtr piuidPtr(new std::string(piuid));
    processIncomingCreateRequestAsync(servicePtr, profilePtr, piuidPtr);
}

void Application::processIncomingCreateRequestAsync(tStringPtr service, tStringPtr profile,
                                                    tStringPtr profileImplUid)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    dispatch_async(mSerialProcessingQueue, ^
    {
        tServicesMap::iterator findIt = mServiceHolders.find(*service.get());
        if (findIt == mServiceHolders.end())
        {
            App::ServiceHolder * newService = new App::ServiceHolder(*service.get());
            mServiceHolders[*service.get()] = newService;
            // this is done synchronously on same queue, because callbacks may be registered here
            onIncomingServiceBeforeLoading(Service::Uid(*service.get()));
        }
        App::ServiceHolder * serviceHolder = mServiceHolders[*service.get()];
        if (!serviceHolder->incomingProfile(profile, profileImplUid).isNoError())
        {
            LOG4CPLUS_ERROR(mLogger, "could not process incoming profile: " + (*profile.get()));
            delete serviceHolder;
            mServiceHolders.erase(mServiceHolders.find(*service.get()));
            postServiceLoadErrorAsync(service);
        }
        else
        {
            if (serviceHolder->allProfilesLoaded())
            {
                postIncomingServiceAfterAsync(service);
            }
        }
    });
}

void Application::postIncomingServiceAfterAsync(tStringPtr service)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    dispatch_async(mSerialCallbacksQueue, ^
    {
        onIncomingServiceAfterLoading(Service::Uid(*service.get()));
    });
}

void Application::postServiceLoadErrorAsync(tStringPtr service)
{
    dispatch_async(mSerialCallbacksQueue, ^
    {
        onServiceLoadError(Service::Uid(*service.get()));
    });
}

void Application::processIncomingDiedRequest(std::string const& service, std::string const& piuid)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, "processIncomingDiedRequest " + service);
    tStringPtr servicePtr(new std::string(service));
    tStringPtr piuidPtr(new std::string(piuid));
    processIncomingDiedRequestAsync(servicePtr, piuidPtr);
}

void Application::processIncomingDiedRequestAsync(tStringPtr service, tStringPtr piuid)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    dispatch_async(mSerialProcessingQueue, ^
    {
        LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
        tServicesMap::iterator findIt = mServiceHolders.find(*service.get());
        if (findIt == mServiceHolders.end())
        {
            LOG4CPLUS_ERROR(mLogger, "could not process incoming profile died, no service: " + (*service.get()));
            return;
        }
        findIt->second->incomingProfileDied(piuid);
        if (findIt->second->allProfilesDead())
        {
            delete findIt->second;
            mServiceHolders.erase(findIt);
            postServiceDroppedAsync(service);
        }
    });
}

void Application::postServiceDroppedAsync(tStringPtr service)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    dispatch_async(mSerialCallbacksQueue, ^
    {
        onServiceDropped(Service::Uid(*service.get()));
    });
}

void Application::onInitDone(ELaunchInfo launcher)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
}

void Application::onIncomingServiceBeforeLoading(const Service::Uid &service)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
}

void Application::onIncomingServiceAfterLoading(const Service::Uid &service)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
}

void Application::onServiceLoadError(const Service::Uid &service)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
}

void Application::onServiceDropped(const Service::Uid &service)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
}

void Application::onLinkUp()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
}

void Application::onPhysicalLinkDown()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
}

bool Application::isLinkAlive()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    return SystemController::SystemControllerStub::getInstance()->getLinkState();
}
