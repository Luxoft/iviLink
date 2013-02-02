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


/**
 * \file Application.hpp
 * This header contains base class for all applications using iviLink functionality
 */


#ifndef CAPP_HPP_
#define CAPP_HPP_

#include "AppLibCommon.hpp"

#include "AppInfo.hpp"
#include "PublicError.hpp"

namespace iviLink
{
namespace App
{

class CAppManProxy;
class CServManProxy;
class InternalThread;

} // namespace App
   
/**
 * \class Application
 * This class is used to initialize application in iviLink and use general
 * iviLink functionality - load/unload services and get profiles.
 * class should be inherited it to receive callbacks.
 */
class Application
{
    friend class App::InternalThread;
public:
    /**
     * In case of Android, we additionally have to pass path to service
     * repository and application's launch string:
     * otherwise AppMan isn't able to correctly identify how this app was launched and
     * ServiceManager can't find service repository in some cases
     */

    /**
     * Constructor. No information on supported services is passed to iviLink.
     * In order to enable services or disable any of them in run-time
     * setEnabled() method should be used
     */
    Application(iviLink::Android::AppInfo appInfo = iviLink::Android::AppInfo());


    /**
     * Constructor. Information on one supported service is passed to iviLink.
     * In order to enable addtitional services or disable any of them in
     * run-time setEnabled() method should be used
     * @param service UID of the service to be supported by the Application
     */
    Application(const Service::Uid &service, iviLink::Android::AppInfo appInfo
            = iviLink::Android::AppInfo());


    /**
     * Constructor. Information on multiple supported services is passed to
     * iviLink. In order to enable addtitional services or disable any of
     * them in run-time setEnabled() method should be used
     * @param services list of the services UIDs to be supported by the Application
     */
    Application(const Service::ListOfUids &services, iviLink::Android::AppInfo appInfo
            = iviLink::Android::AppInfo());

    virtual ~Application();

    /**
     * Starts interactions with iviLink core, registers application and starts waiting for incoming requests
     * Must be called after constructor of CApp inheritor.
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
     * Get information on how the application was launched
     * @retval ERROR_APP_NOT_INITED_IN_IVILINK an error occurred during
     *    application's initialization
     * @retval LAUNCHED_BY_USER application was launched by user
     * @retval LAUNCHED_BY_IVILINK application was launched by Application
     * Manager after a request from the other side
     */
    ELaunchInfo getLaunchInfo();

    /**
     * Register callbacks for given profile API.
     * @param profileApi UID of a Profile API
     * @param pCallbacks pointer to a handler of this profile's callbacks
     */
    void registerProfileCallbacks(const Profile::ApiUid &profileApi, Profile::IProfileCallbackProxy* pCallbacks);


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
    // TODO ERROR! fix inside ;)
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
     * Called after the initialization of this application.
     * @param launcher is the output parameter providing information on how the
     *    application was launched: app may be launched either by user (in this
     *    case services should be loaded with loadService() method call
     *    ,and then proxies for profiles may be obtained),
     *    or by Application Manager (after request from the other side -
     *    then proxies may be used).
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
     * Called when two iviLinks connected ans authentificated with each other
     */
    virtual void onLinkUp();
    /**
     * Called when Connectivity Agent's link has been broken
     */
    virtual void onPhysicalLinkDown();
    // Connectivity state
    virtual bool isLinkAlive();

private:

    void internalRun();

    App::CAppManProxy * mpAppManProxy;
    App::CServManProxy * mpServManProxy;
    Service::ListOfUids mSupportedServices;
    iviLink::Android::AppInfo mAppInfo;
    App::InternalThread * mInternalThread;
};

} // namespace iviLink
#endif
