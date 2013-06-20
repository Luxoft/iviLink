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

#ifndef __iviLinkCoreStack__Application__
#define __iviLinkCoreStack__Application__

#include <map>
#include <tr1/memory>
#include <dispatch/dispatch.h>

#include "AppLibCommon.hpp"
#include "AppInfo.hpp"
#include "PublicError.hpp"
#include "Logger.hpp"
#include "CMutex.hpp"

typedef std::tr1::shared_ptr<std::string> tStringPtr;

namespace iviLink
{
namespace App
{
class ServiceHolder;
class ApplicationRegistry;
namespace Service
{
class CServiceManager;
}
}

typedef std::map<std::string, App::ServiceHolder*> tServicesMap;
class Application
{
    friend class iviLink::App::Service::CServiceManager;
    friend class iviLink::App::ApplicationRegistry;
public:
    /**
     * No information on supported services is passed to iviLink.
     * In order to enable services or disable any of them in run-time
     * setEnabled() method should be used
     */
    Application(iviLink::Android::AppInfo appInfo = iviLink::Android::AppInfo());

    /**
     * Information on one supported service is passed to iviLink.
     * In order to enable addtitional services or disable any of them in
     * run-time setEnabled() method should be used
     * @param service UID of the service to be supported by the Application
     */
    Application(const Service::Uid &service, iviLink::Android::AppInfo appInfo =
            iviLink::Android::AppInfo());

    /**
     * Constructor. Information on multiple supported services is passed to
     * iviLink. In order to enable addtitional services or disable any of
     * them in run-time setEnabled() method should be used
     * @param services list of the services UIDs to be supported by the Application
     */
    Application(const Service::ListOfUids &services, iviLink::Android::AppInfo appInfo =
            iviLink::Android::AppInfo());

    virtual ~Application();

    /**
     * Starts stack if it is not started already.
     * After that Application will start receiving incoming loads and link up/down notifications.
     * It may also load a service.
     */
    void initInIVILink();

    /**
     * Changes enabled status of service. If service is enabled it means
     * that application supports incoming service requests from iviLink.
     * @param service UID of a service to be enabled/disabled
     * @param enable true enables service, false disables it
     */
    void setEnabled(const Service::Uid &service, bool enable);

    /**
     * Changes enabled status of list of services. If service is enabled
     * it means that application supports incoming service requests from iviLink.
     * @param services list of UIDs of services to be enabled/disabled
     * @param enable true enables list of services, false disables them
     */
    void setEnabled(const Service::ListOfUids &services, bool enable);

    /**
     * This method is just a stub for API compatibility. It always returns same value.
     * @retval LAUNCHED_BY_USER application was launched by user
     */
    ELaunchInfo getLaunchInfo();

    /**
     * Register callbacks for given profile API.
     * @param profileApi UID of a Profile API
     * @param pCallbacks pointer to a handler of this profile's callbacks
     */
    void registerProfileCallbacks(const Profile::ApiUid &profileApi,
            Profile::IProfileCallbackProxy* pCallbacks);

    /**
     * Load a service. This method should be called to load a service in case
     * the Application is started by user (see the initDone() callback.
     * loadService method causes execution of the Application and loading of the
     * appropriate service on the other side. After a service loaded successfully
     * it is possible to obtain a profile proxy of a profile associated with
     * the service and use profile functionality from the application.
     * @param service UID of a service to load
     * @return true if success, otherwise false
     */
    Error loadService(const Service::Uid &service);

    /**
     * Unload a service.
     * @param service UID of a service to be unloaded
     */
    Error unloadService(const Service::Uid &service);

    /**
     * Obtain list of loaded services.
     * @return list of loaded services (see loadService())
     */
    Service::ListOfUids getActiveServices();

    /**
     * Check whether a service is loaded.
     * @param service UID of a service
     * @return true if the service is active, otherwise false
     */
    bool isActive(const Service::Uid &service);

    /* The following functions are callbacks invoked after events occurred.
     * In order to hadnle either of conditions described below,
     * appropriate callbacks should be implemented in a derived class.
     */

    /**
     * This method is just a stub for API compatibility. It is never called,
     * so all logic related to loading a service as soon as possible
     * should be moved to onLinkUp().
     */
    virtual void onInitDone(ELaunchInfo launcher);

    /**
     * Called before loading an incoming service.
     * @param service UID of an incoming service
     */
    virtual void onIncomingServiceBeforeLoading(const Service::Uid &service);

    /**
     * Called after loading an incoming service.
     * @param service UID of a loaded incoming service
     */
    virtual void onIncomingServiceAfterLoading(const Service::Uid &service);

    /**
     * Called if an error has occurred during service loading.
     * @param service UID of an incoming service that caused the error
     */
    virtual void onServiceLoadError(const Service::Uid &service);

    /**
     * Called when a service session ends.
     * @param service UID of a service whose session has ended
     */
    virtual void onServiceDropped(const Service::Uid &service);

    // Connectivity callbacks
    /**
     * Called when two iviLinks have connected and authentificated with each other
     */
    virtual void onLinkUp();
    /**
     * Called when Connectivity Agent's link to the other side has been broken.
     * Currently there is no way to reconnect, so the best option is to exit.
     */
    virtual void onPhysicalLinkDown();
    // Connectivity state
    virtual bool isLinkAlive();

private:
    // iOS-specific
    /**
     * Silently unloads all loaded services and calls onPhysicalLinkDown
     */
    void linkDownNotification();
    void linkUpNotification();
    bool isEnabled(const Service::Uid& service);
    void setEnabledUnsafe(const Service::Uid &service, bool enable);
    void processIncomingCreateRequest(std::string const&, std::string const&, std::string const&);
    void processIncomingDiedRequest(std::string const&, std::string const&);

private:
    // threading-related methods
    void initQueues();
    void releaseQueues();
    void processIncomingCreateRequestAsync(tStringPtr, tStringPtr, tStringPtr);
    void processIncomingDiedRequestAsync(tStringPtr, tStringPtr);

    void loadServiceSync(tStringPtr service);
    void unloadServiceSync(tStringPtr service);

    void postServiceDroppedAsync(tStringPtr service);
    void postLinkUpAsync();
    void postLinkDownAsync();
    void postServiceLoadErrorAsync(tStringPtr service);
    void postIncomingServiceAfterAsync(tStringPtr service);
private:
    static Logger mLogger;
    Service::ListOfUids mSupportedServices;
    CMutex mSupportedServicesMutex;
    tServicesMap mServiceHolders;

    bool mIsInited;
    CMutex mInitedMutex;

    Error mSavedError;
    // all code related to mServiceHolders is executed on this queue
    dispatch_queue_t mSerialProcessingQueue;
    // queue for event callbacks - link up/down, service load error, service dropped
    dispatch_queue_t mSerialCallbacksQueue;
};
}

#endif /* defined(__iviLinkCoreStack__Application__) */
