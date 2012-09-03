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

#ifndef STATE_APP_HPP
#define STATE_APP_HPP

#include <string>
#include <tr1/memory>

#include "framework/public/appLib/CApp.hpp"
#include "../Profiles/ProfileAPI/IClimateProfiles.hpp"
#include "utils/threads/CMutex.hpp"
#include "framework/libraries/ServiceManager/CServiceManager.hpp"

class state_app : public iviLink::CApp
{
 public:
    typedef IClimateSenderProfile::state_t state_t;
    typedef IClimateSenderProfile*   sender_ptr;
    typedef IClimateReceiverProfile* receiver_ptr;

    void retrive_state( state_t& st );
    void update_state( const state_t& st );  // update timestamp and send state to other side.

    typedef std::tr1::function<void ()> callback_t;
    void set_state_callback( callback_t callback );

    #ifndef ANDROID
    state_app( const iviLink::Service::Uid& service_uid
    #else
    state_app( iviLink::Android::AppInfo appInfo
               , const iviLink::Service::Uid& service_uid
    #endif //ANDROID
               , const iviLink::Profile::ApiUid& sender_auid
               , const iviLink::Profile::ApiUid& receiver_auid
               , const std::string& state_file = std::string());

    ~state_app();


    enum APP_STATE {
        CREATING_, /// Constructor working
        CREATED,  /// Just created - still not initialized
        MASTER,   /// Started by user, initialized
        AWAITING_SLAVE_MODE_, /// Avaiting initialization by iviLink
        SLAVE,    /// Started by iviLink, initialized
        OFFLINE,  /// Failed to load service, connect to iviLink lost etc.
    };

    APP_STATE app_state() const;
    typedef std::tr1::function< void (APP_STATE)> app_callback_t;
    void set_app_state_callback( app_callback_t app_callback );

    // "Callbacks" from CApp

    // Called after the initialization of this application.
    virtual void initDone(iviLink::ELaunchInfo launcher);

    // Called before loading an incoming service. // In Slave mode
    virtual void incomingServiceBeforeLoading(const iviLink::Service::Uid &service);

    // Called after loading an incoming service. // In Slave mode
    virtual void incomingServiceAfterLoading(const iviLink::Service::Uid &service);

    // Called if an error has occurred during service loading.
    virtual void serviceLoadError(const iviLink::Service::Uid &service);

    // Called when a service session ends.
    virtual void serviceDropped(const iviLink::Service::Uid &service);

    void saveState();

 private:
    CMutex state_mutex;

 private:
    // avoid copy
    state_app(const state_app&);
    state_app& operator=(const state_app&);

 private:
    void switch2CREATED();
    void switch2AWAITINGSLAVE();
    void switch2MASTER();
    void switch2SLAVE();
    void switch2OFFLINE();
    void switch2AWITINGSLAVE();

 private:
    IClimateSenderProfile::profile_callback_t   sender_cb;
    IClimateReceiverProfile::profile_callback_t receiver_cb;
    void profile_fail( bool sender_, const CError& e );
    bool setup_profiles();
    void release_profiles();
    void update_state_from_other_side( const state_t& ostate );
    void send_state_unsafe();
 private:
    callback_t callback;
    app_callback_t app_callback;
 private:
    void notify_state() const;
    void notify_app_state() const;
 private:
    state_t   st;
    APP_STATE app_st;
 private:
    sender_ptr   sender;
    receiver_ptr receiver;

 private:
    std::string state_file;

 private:
    const iviLink::Profile::ApiUid sender_uid, receiver_uid;
    const iviLink::Service::Uid service_uid;

   #ifdef ANDROID
     iviLink::Android::AppInfo mAppInfo;
   #endif //ANDROID

   // For Android
   friend
   iviLink::Service::CServiceManager* service_manager( state_app* app );
};

typedef std::tr1::shared_ptr<state_app> state_app_ptr;

bool is_active( state_app_ptr app );


inline iviLink::Service::CServiceManager* service_manager( state_app* app )
{
    namespace isv = iviLink::Service;
    iviLink::Service::CServiceManager* sm =
#   if !defined(ANDROID)
        isv::CServiceManager::getInstance();
    (void)(app); // avoid gcc warning
#   else
        app ?
            isv::CServiceManager::getInstance(app->mAppInfo)
            : 0;
#   endif
    return sm;
}

#endif //STATE_APP_HPP
