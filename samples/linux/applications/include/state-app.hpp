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
#ifndef STATE_APP_HPP
#define STATE_APP_HPP

#include <string>
#include <cstdlib>

#include "Application.hpp"
#include "CMutex.hpp"
#include "Exit.hpp"
#include "CServiceManager.hpp"

#include "IStateAppProgressBar.hpp"
#include "logger-decl.hpp"
#include "state-serialization.hpp"
#include "casts.hpp"


template<typename S, typename R>
class StateApp : public iviLink::Application
{
 public:
    typedef S  ISenderProfile;
    typedef R  IReceiverProfile;
    typedef typename ISenderProfile::state_t state_t;

    void retrieveState(state_t& st);
    // update timestamp and send state to other side.
    void updateState(const state_t& st);  

    typedef std::tr1::function<void ()> callback_t;
    void setStateCallback(callback_t callback);

#ifndef ANDROID
    StateApp(const iviLink::Service::Uid& service_uid
#else
    StateApp(iviLink::Android::AppInfo appInfo
               , const iviLink::Service::Uid& service_uid
#endif
               , const iviLink::Profile::ApiUid& sender_auid
               , const iviLink::Profile::ApiUid& receiver_auid
               , const std::string& state_file = std::string());

    ~StateApp();

    void initStateApp(IStateAppProgressBar* progress = NULL);

    enum APP_STATE {
        CREATING_ = 10,            /// Constructor working
        CREATED   = 11,            /// Just created - still not initialized
        MASTER    = 100,           /// Started by user, initialized
        AWAITING_SLAVE_MODE_ = 13, /// Avaiting initialization by iviLink
        SLAVE     = 101,           /// Started by iviLink, initialized
        OFFLINE   = -1,            /// Failed to load service, connect to iviLink lost etc.
    };

    APP_STATE appState() const;
    typedef std::tr1::function< void (APP_STATE)> app_callback_t;
    void setAppStateCallback(app_callback_t app_callback);

    bool isActive() const;
    // "Callbacks" from CApp

    // Called after the initialization of this application.
    virtual void onInitDone(iviLink::ELaunchInfo launcher);

    // Called before loading an incoming service. // In Slave mode
    virtual void onIncomingServiceBeforeLoading(const iviLink::Service::Uid &service);

    // Called after loading an incoming service. // In Slave mode
    virtual void onIncomingServiceAfterLoading(const iviLink::Service::Uid &service);

    // Called if an error has occurred during service loading.
    virtual void onServiceLoadError(const iviLink::Service::Uid &service);

    // Called when a service session ends.
    virtual void onServiceDropped(const iviLink::Service::Uid &service);

    void saveState();

 private:
    CMutex stateMutex;

 private:
    // avoid copy
    StateApp(const StateApp&);
    StateApp& operator=(const StateApp&);

 private:
    void switch2CREATED();
    void switch2AWAITINGSLAVE();
    void switch2MASTER();
    void switch2SLAVE();
    void switch2OFFLINE();

    void assertCurrentStateIs(APP_STATE st) const;

 private:
    typename ISenderProfile::profile_callback_t   sender_cb;
    typename IReceiverProfile::profile_callback_t receiver_cb;
    void profileFail(bool sender_, const BaseError& e);
    bool setupProfiles();
    void releaseProfiles();
    void updateStateFromOtherSide(const state_t& ostate);
    void sendStateUnsafe();

 private:
    callback_t callback;
    app_callback_t app_callback;

 private:
    void notifyState() const;
    void notifyAppState() const;

 private:
    state_t   st;
    APP_STATE app_st;

 private:
    ISenderProfile   *sender;
    IReceiverProfile *receiver;

 private:
    std::string state_file;

 private:
    IStateAppProgressBar* mProgressBar;
    const iviLink::Profile::ApiUid  senderUid, receiverUid;
    const iviLink::Service::Uid serviceUid;
#ifdef ANDROID
    iviLink::Android::AppInfo mAppInfo;
#endif

   // For Android support
   iviLink::Service::CServiceManager* serviceManager();
};


///////////////////////////////////////////////////////////////////
// Let's rock
///////////////////////////////////////////////////////////////////

template<typename S, typename R>
bool StateApp<S,R>::isActive() const
{
    return appState() == StateApp::MASTER ||
        appState() == StateApp::SLAVE;
}

template<typename S, typename R>
StateApp<S,R>::~StateApp()
{
    trySave<state_t>(st, state_file);
    if (appState() == MASTER || appState() == SLAVE)
    {
        LOG4CPLUS_INFO(log4StateApp(), "StateApp: destructor call while app is in active state. Trying release profiles.");
        releaseProfiles();
        switch2OFFLINE();
    }
}

template<typename S, typename R>
void StateApp<S,R>::setStateCallback(StateApp<S,R>::callback_t cb)
{
    if (callback)
    {
        LOG4CPLUS_WARN(log4StateApp(), "StateApp: rewriting state callback");
    }

    callback = cb;
}

template<typename S, typename R>
void StateApp<S,R>::setAppStateCallback(StateApp<S,R>::app_callback_t acb)
{
    if (app_callback)
    {
        LOG4CPLUS_WARN(log4StateApp(), "StateApp: rewriting app state callback");
    }    

    app_callback = acb;
}

template<typename S, typename R>
void StateApp<S,R>::notifyState() const
{
    if (callback)
    {
        callback();
    }    
    else
    {
        LOG4CPLUS_WARN(log4StateApp(), "StateApp: where is no callback for notify_state()");
    }
}

template<typename S, typename R>
void StateApp<S,R>::notifyAppState() const
{
    if (app_callback)
    {
        app_callback(appState());
    }    
    else
    {
        LOG4CPLUS_WARN(log4StateApp(), "StateApp: where is no app_callback for notify_app_state()");
    }
}

template<typename S, typename R>
void StateApp<S,R>::onInitDone(iviLink::ELaunchInfo launcher)
{
    LOG4CPLUS_TRACE_METHOD(log4StateApp(), __PRETTY_FUNCTION__);
    const bool master = iviLink::LAUNCHED_BY_USER == launcher;
    if (master)
    {
        if(mProgressBar)
        {
            mProgressBar->showLoadingScreen();
        }
        iviLink::Error loadError = loadService(serviceUid);
        if(mProgressBar)
        {
            mProgressBar->hideLoadingScreen();
        }
        if( loadError.isNoError() && setupProfiles() )
        {
            switch2MASTER();
        }
        else
        {
            LOG4CPLUS_ERROR(log4StateApp(), "Could not load service: " + loadError.toString());
            killProcess(1); // temp change
        }
    }
    else
    {
        switch2AWAITINGSLAVE();
    }
}

template<typename S, typename R>
void StateApp<S,R>::updateState(const state_t& src)
{
    LOG4CPLUS_TRACE_METHOD(log4StateApp(), __PRETTY_FUNCTION__);
    {
        MutexLocker lock(stateMutex);
        st = src;
        LOG4CPLUS_INFO(log4StateApp(), "Update state: " + st.serialize());
        st.touchCookie();

        if (appState() == MASTER || appState() == SLAVE)
        {
            sendStateUnsafe();
        }
    }
    notifyState();
}

template<typename S, typename R>
void StateApp<S,R>::retrieveState(state_t& dst)
{
    LOG4CPLUS_TRACE_METHOD(log4StateApp(), __PRETTY_FUNCTION__);
    MutexLocker lock(stateMutex);
    dst = st;
}

template<typename S, typename R>
void StateApp<S,R>::sendStateUnsafe()
{
    LOG4CPLUS_TRACE_METHOD(log4StateApp(), __PRETTY_FUNCTION__);

    LOG4CPLUS_INFO(log4StateApp(), "send: cookie: " + cast_to_string(st.getCookie()));

    if (sender)
    {
        sender->send_state(st);
    }
    else
    {
        LOG4CPLUS_WARN(log4StateApp(), "StateApp: can't send new state: sender is null" );
    }
}

template<typename S, typename R>
void StateApp<S,R>::updateStateFromOtherSide(const state_t& ostate)
{
    LOG4CPLUS_TRACE_METHOD(log4StateApp(), __PRETTY_FUNCTION__);

    bool needNotify = false;
    {
        LOG4CPLUS_INFO(log4StateApp(), "receive: local cookie and remote cookie: " + cast_to_string(st.getCookie()) + " " 
            +           cast_to_string(ostate.getCookie()));

        MutexLocker lock(stateMutex);
        const bool newerState = st.compareTo(ostate) == iviLink::LESS;
        if (ostate.compareTo(st) == iviLink::EQUAL)
        {
            LOG4CPLUS_INFO(log4StateApp(), "we got the same state");
            if (appState() == MASTER)
            {
                st.touchCookie();
                LOG4CPLUS_INFO(log4StateApp(), "update other side by this state (MASTER)");
                sendStateUnsafe();
            }
        }
        else if (newerState)
        { 
            LOG4CPLUS_INFO(log4StateApp(), "other side state is the newer state");
            st = ostate;
            needNotify = true;
        }
        else 
        { 
            LOG4CPLUS_INFO(log4StateApp(), "our state is newer, update other side by this state");
            sendStateUnsafe();
        }
    }
    if(needNotify)
    {
       notifyState();
    }
 }

template<typename S, typename R>
void StateApp<S,R>::profileFail(bool sender_, const BaseError& e)
{
    LOG4CPLUS_TRACE_METHOD(log4StateApp(), __PRETTY_FUNCTION__);
    LOG4CPLUS_ERROR(log4StateApp(), string("StateApp: Error in ") + (sender_ ? "Sender" : "Receiver") + "Profile: " + e.operator string() );
    releaseProfiles();
    switch2OFFLINE();
}


template<typename S, typename R>
void StateApp<S,R>::releaseProfiles()
{
    LOG4CPLUS_TRACE_METHOD(log4StateApp(), __PRETTY_FUNCTION__);

    if (serviceManager())
    {
        if (receiver)
        {
            serviceManager()->releaseProfile(serviceUid, receiverUid);
        }
        if (sender)
        {
            serviceManager()->releaseProfile(serviceUid, senderUid);
        }
    }

    sender = 0;
    receiver = 0;
}

template<typename S, typename R>
bool StateApp<S,R>::setupProfiles()
{
    LOG4CPLUS_TRACE_METHOD(log4StateApp(), __PRETTY_FUNCTION__);
    assert(appState() == CREATED || appState() == AWAITING_SLAVE_MODE_);
    assert(sender == 0);
    assert(receiver == 0);
    if (serviceManager())
    {
        sender = polymorphic_downcast<ISenderProfile*>(serviceManager()->getProfile(serviceUid, senderUid));
        if (!sender)
        {
            LOG4CPLUS_ERROR(log4StateApp(), "StateApp: failed to get ISenderPofile pointer");
            return false;
        }
        receiver = polymorphic_downcast<IReceiverProfile*>(serviceManager()->getProfile(serviceUid, receiverUid));
        if (receiver)
        {
            using namespace std::tr1::placeholders;
            receiver->register_receiver(std::tr1::bind(&StateApp<S,R>::updateStateFromOtherSide, this, _1));
            return true;
        }
        else
        {
            LOG4CPLUS_ERROR(log4StateApp(), "StateApp: failed to get IReceiverPofile pointer");
            // release sender profile
            releaseProfiles(); 
            return false;
        }
    }
    else
    {
        LOG4CPLUS_ERROR(log4StateApp(), "StateApp: failed to get ServiceManager");
        return false;
    }
}

template<typename S, typename R>
void StateApp<S,R>::assertCurrentStateIs(APP_STATE st) const
{
    LOG4CPLUS_TRACE_METHOD(log4StateApp(), __PRETTY_FUNCTION__);

    if (appState() != st)
    {
        LOG4CPLUS_FATAL( log4StateApp(), "StateApp: application should be in state " + cast_to_string(appState())
                     + "but really is in state " +  cast_to_string( st ) );
    }
}

template<typename S, typename R>
void StateApp<S,R>::switch2MASTER()
{
    assertCurrentStateIs(CREATED);
    app_st = MASTER;
    LOG4CPLUS_INFO(log4StateApp(), "StateApp: switched to MASTER");
    notifyAppState();
    {
        st.touchCookie();
        MutexLocker lock(stateMutex);
        sendStateUnsafe();
    }
}

template<typename S, typename R>
void StateApp<S,R>::switch2SLAVE()
{
    assertCurrentStateIs(AWAITING_SLAVE_MODE_);
    app_st = SLAVE;
    LOG4CPLUS_INFO(log4StateApp(), "StateApp: switched to SLAVE");
    notifyAppState();
}

template<typename S, typename R>
void StateApp<S,R>::switch2OFFLINE()
{
    app_st = OFFLINE;
    LOG4CPLUS_INFO(log4StateApp(), "StateApp: switched to OFFLINE");
    notifyAppState();
}

template<typename S, typename R>
void StateApp<S,R>::switch2AWAITINGSLAVE()
{
    assertCurrentStateIs(CREATED);
    app_st = AWAITING_SLAVE_MODE_;
    // Reverse channels for simmetrics app.
    // See comments in ProfileAPI/stateness.hpp for details.
    sender_cb.swap_channels();
    receiver_cb.swap_channels();

    LOG4CPLUS_INFO(log4StateApp(), "StateApp: switched to AWAITING_SLAVE_MODE_");
    notifyAppState();
}

template<typename S, typename R>
void StateApp<S,R>::switch2CREATED()
{
    assertCurrentStateIs(CREATING_);
    app_st = CREATED;
    LOG4CPLUS_INFO(log4StateApp(), "StateApp: switched to CREATED state.");
    notifyAppState();
}

template<typename S, typename R>
typename StateApp<S,R>::APP_STATE StateApp<S,R>::appState() const
{
    LOG4CPLUS_TRACE_METHOD(log4StateApp(), __PRETTY_FUNCTION__);
    return app_st;
}


template<typename S, typename R>
void StateApp<S,R>::onIncomingServiceBeforeLoading(const iviLink::Service::Uid &service)
{
    LOG4CPLUS_INFO(log4StateApp(), "StateApp: incoming service `" +service.value()+ "` before loading");
    if(mProgressBar)
    {
        mProgressBar->showLoadingScreen();
    }
}

template<typename S, typename R>
void StateApp<S,R>::onIncomingServiceAfterLoading(const iviLink::Service::Uid &service)
{
    assert(appState() == AWAITING_SLAVE_MODE_);
    LOG4CPLUS_INFO(log4StateApp(), "StateApp: incoming service `" +service.value() + "` after loading");
    if(mProgressBar)
    {
        mProgressBar->hideLoadingScreen();
    }
    if (setupProfiles())
    {
        switch2SLAVE();
    }
    else
    {
        LOG4CPLUS_ERROR(log4StateApp(), "StateApp: failed to setup profiles pointers. Switching to OFFLINE");
        releaseProfiles();
        switch2OFFLINE();
    }
}

template<typename S, typename R>
void StateApp<S,R>::onServiceLoadError(const iviLink::Service::Uid &service)
{
    LOG4CPLUS_ERROR(log4StateApp(), "StateApp: service '" + service.value() + "' load error. Switching to OFFLINE.");
    killProcess(1);
}

template<typename S, typename R>
void StateApp<S,R>::onServiceDropped(const iviLink::Service::Uid& service)
{
    LOG4CPLUS_INFO(log4StateApp(), "StateApp: service '" + service.value() + "' dropped. Switching to OFFLINE");
    killProcess();
}

template<typename S, typename R>
void StateApp<S,R>::saveState()
{
    LOG4CPLUS_TRACE_METHOD(log4StateApp(), __PRETTY_FUNCTION__);
    LOG4CPLUS_INFO(log4StateApp(), "StateApp: saveState()");
    trySave(st, state_file);
}

#ifndef ANDROID
template<typename S, typename R>
StateApp<S,R>::StateApp(const iviLink::Service::Uid& serviceUid
#else
template<typename S, typename R>
StateApp<S,R>::StateApp(iviLink::Android::AppInfo appInfo
                      , const iviLink::Service::Uid& serviceUid
#endif //ANDROID
                      , const iviLink::Profile::ApiUid& senderUid
                      , const iviLink::Profile::ApiUid& receiverUid
                      , const std::string& state_file) :
    #ifndef ANDROID
    Application(serviceUid)
    #else
    Application(serviceUid, appInfo)
    , mAppInfo(appInfo)
    #endif //ANDROID
    , app_st(CREATING_)
    , sender(0)
    , receiver(0)
    , state_file(state_file)
    , senderUid(senderUid)
    , receiverUid(receiverUid)
    , serviceUid(serviceUid)
    , mProgressBar(NULL)
{
    LOG4CPLUS_INFO( log4StateApp(), "StateApp constructor");
}

template<typename S, typename R>
void StateApp<S,R>::initStateApp(IStateAppProgressBar* progress)
{
   LOG4CPLUS_TRACE_METHOD(log4StateApp(), __PRETTY_FUNCTION__);
   mProgressBar = progress;

   initInIVILink();
   registerProfileCallbacks(senderUid, &sender_cb);
   LOG4CPLUS_INFO(log4StateApp(),
                   "register sender callback \"" + cast_to_string(senderUid)
                   + "\" at " + cast_to_string(&sender_cb));
   registerProfileCallbacks(receiverUid, &receiver_cb);
   LOG4CPLUS_INFO(log4StateApp(),
                   "register receiver callback \"" + cast_to_string(receiverUid)
                   + "\" at " + cast_to_string(&receiver_cb));

   using namespace std::tr1::placeholders;
   sender_cb.set_error_handler(std::tr1::bind(&StateApp<S,R>::profileFail, this, true, _1));
   receiver_cb.set_error_handler(std::tr1::bind(&StateApp<S,R>::profileFail, this, false, _1));

   LOG4CPLUS_INFO(log4StateApp(), "Before try load");

   tryLoad(st, state_file);
   switch2CREATED();
   LOG4CPLUS_INFO(log4StateApp(), "After switch2CREATED ()");
}

template<typename S, typename R>
iviLink::Service::CServiceManager* StateApp<S,R>::serviceManager()
{
    namespace isv = iviLink::Service;
    iviLink::Service::CServiceManager* sm =
    #ifndef ANDROID
    isv::CServiceManager::getInstance();
    #else
    isv::CServiceManager::getInstance(mAppInfo);
    #endif
    assert(sm);
    return sm;
}

#endif //StateApp_HPP

