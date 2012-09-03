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

#include "state-app.h"
#include <fstream>
#include <iostream>
#include <ctime>
#include <cstdio>
#include "utils/misc/Logger.hpp"

template <class Derived, class Base>
inline Derived polymorphic_downcast(Base* x)
{
#ifndef NDEBUG  // add one more "if" for Android support plz
    assert( dynamic_cast<Derived>(x) != 0 );
#endif
    return static_cast<Derived>(x);
}

using namespace std;
using namespace iviLink;

typedef state_app::state_t state_t;

bool is_active( state_app_ptr app )
{
    assert( app );
    return app &&
        ( app->app_state() == state_app::MASTER ||
          app->app_state() == state_app::SLAVE );
}

namespace {

    Logger log4 = Logger::getInstance("ClimateClientSample.state-app");

    void try_load(state_t& state, const string& file)
    {
        namespace pb = google::protobuf;
        ifstream f(file.c_str(), ios::in | ios::binary);
        if( !f )
            LOG4CPLUS_WARN(log4, "Failed to load prev state from "+ file);
        else
        {
            // get size of file
            f.seekg(0,ifstream::end);
            const size_t size= f.tellg();
            f.seekg(0);
            std::string buf(size,'\0');

            if( !f.read( &*buf.begin(), buf.size() ) )
            {
                LOG4CPLUS_WARN(log4, "Failed to read prev state from "+ file);
                return;
            }

            state_t st;
            if( !st.ParseFromString( buf ))
                LOG4CPLUS_WARN(log4, "Failed to parse prev state from "+ file);
            else
                state = st;
        }
    }

    void try_save(const state_t& state, const string& file)
    {
        namespace pb = google::protobuf;
        const std::string tmp_file = file+".tmp";

        std::string buffer;
        if (!state.SerializeToString(&buffer))
        {
            LOG4CPLUS_WARN(log4, "Failed to serialize last state." );
            return;
        }

        std::ofstream f(tmp_file.c_str(), ios::out | ios::binary);
        if( !f || f.write(buffer.c_str(), buffer.size()).bad() )
            LOG4CPLUS_WARN(log4, "Failed to serialize last state to " + tmp_file);
        else
        {
            f.flush();
            f.close();
            if( rename( tmp_file.c_str(), file.c_str() ) )
                LOG4CPLUS_ERROR( log4, "Failed to rename state file " + tmp_file + "to " + file );
        }
    }

}

state_app::~state_app()
{
    try_save( st, state_file );
    if( app_state() == MASTER || app_state() == SLAVE )
    {
        LOG4CPLUS_WARN( log4, "state_app: destructor call while app is in active state. Try release profiles." );
        release_profiles();
        switch2OFFLINE();
    }
}

void state_app::set_state_callback( state_app::callback_t cb )
{
    if( callback )
        LOG4CPLUS_WARN( log4, "state_app: rewriting state callback" );
    callback = cb;
}

void state_app::set_app_state_callback( state_app::app_callback_t acb )
{
    if( app_callback )
        LOG4CPLUS_WARN( log4, "state_app: rewriting app state callback" );
    app_callback = acb;
}

void state_app::notify_state() const
{
    if( callback )
        callback();
    else
        LOG4CPLUS_WARN( log4, "state_app: where is no callback for notify_state()");
}

void state_app::notify_app_state() const
{
    if( app_callback )
        app_callback( app_state() );
    else
        LOG4CPLUS_WARN( log4, "state_app: where is no app_callback for notify_app_state()");
}

void state_app::initDone(iviLink::ELaunchInfo launcher)
{
    LOG4CPLUS_TRACE_METHOD(log4, __PRETTY_FUNCTION__)
    const bool master = iviLink::LAUNCHED_BY_USER == launcher;
    if( master )
    {
        if( loadService(service_uid) && setup_profiles() )
        {
            switch2MASTER();
        }
        else
            switch2OFFLINE();
    }
    else
    {
        switch2AWAITINGSLAVE();
    }
}

void state_app::update_state( const state_t& src )
{
    const time_t t = std::time(0);
    {
        lock l(state_mutex);
        if( t > st.timestamp().utime() )
        {
            st= src;
            st.mutable_timestamp()->set_utime(t);
            if( app_state() == MASTER || app_state() == SLAVE )
                send_state_unsafe();
        }
    }
    notify_state();
}

void state_app::retrive_state( state_t& dst )
{
    lock l(state_mutex);
    dst= st;
}

void state_app::send_state_unsafe()
{
    if( sender )
        sender->send_state(st);
    else
        LOG4CPLUS_WARN(log4, "state_app: can't send new state: sender is null" );
}

void state_app::update_state_from_other_side( const state_t& ostate )
{
    bool need_notify = false;
    {
        lock l(state_mutex);
        const bool older_state = less_by_time( st, ostate );
        if( !older_state  )
        { // update other side by newest state
            send_state_unsafe();
        }
        else
        {
            st = ostate;
            need_notify= true;
        }

    }
    if( need_notify )
        notify_state();
 }

void state_app::profile_fail( bool sender_, const CError& e )
{
    LOG4CPLUS_ERROR(log4, string("state_app: Error in ") + (sender_ ? "Sender" : "Receiver") + "Profile: " + e.operator string() );
    release_profiles();
    switch2OFFLINE();
}


void state_app::release_profiles()
{
    if( service_manager(this) )
    {
        if( receiver )
            service_manager(this)->releaseProfile(service_uid, receiver_uid);
        if( sender )
            service_manager(this)->releaseProfile(service_uid, sender_uid);
    }
    sender = 0;
    receiver = 0;
}

bool state_app::setup_profiles()
{
    assert( app_state() == CREATED || app_state() == AWAITING_SLAVE_MODE_ );
    assert( sender == 0 );
    assert( receiver == 0 );
    if( service_manager(this) )
    {
        sender = polymorphic_downcast<IClimateSenderProfile*>( service_manager(this)->getProfile(service_uid, sender_uid) );
        if( !sender )
        {
            LOG4CPLUS_ERROR( log4, "state_app: failed to get IClimateSenderPofile pointer" );
            return false;
        }
        receiver = polymorphic_downcast<IClimateReceiverProfile*>( service_manager(this)->getProfile(service_uid, receiver_uid) );
        if( receiver )
        {
            using namespace std::tr1::placeholders;
            receiver->register_receiver( std::tr1::bind(&state_app::update_state_from_other_side, this, _1) );
            return true;
        }
        else
        {
            LOG4CPLUS_ERROR( log4, "state_app: failed to get IClimateReceiverPofile pointer" );
            release_profiles(); // release sender profile
            return false;
        }
    }
    else
    {
        LOG4CPLUS_ERROR( log4, "state_app: failed to get ServiceManager" );
        return false;
    }
}

void state_app::switch2MASTER()
{
    assert( app_state() == CREATED );
    app_st = MASTER;
    LOG4CPLUS_INFO( log4, "state_app: switched to MASTER" );
    notify_app_state();
}

void state_app::switch2SLAVE()
{
    assert( app_state() == AWAITING_SLAVE_MODE_ );
    app_st = SLAVE;
    LOG4CPLUS_INFO( log4, "state_app: switched to SLAVE" );
    notify_app_state();
}

void state_app::switch2OFFLINE()
{
    assert( app_state() != OFFLINE );
    app_st = OFFLINE;
    LOG4CPLUS_WARN( log4, "state_app: switched to OFFLINE" );
    notify_app_state();
}

void state_app::switch2AWAITINGSLAVE()
{
    assert( app_state() == CREATED );
    //if(app_state() == SLAVE) return; // wroted by @elena, comment by @dmi3s
    app_st = AWAITING_SLAVE_MODE_;

    // Reverse channels for simmetrics app.
    // See comments in ProfileAPI/stateness.hpp for details.
    sender_cb.swap_channels();
    receiver_cb.swap_channels();

    LOG4CPLUS_INFO(log4, "state_app: switched to AWAITING_SLAVE_MODE_");
    notify_app_state();
}

void state_app::switch2CREATED()
{
    app_st = CREATED;
    LOG4CPLUS_INFO(log4, "state_app: switched to CREATED state.");
    notify_app_state();
}

state_app::APP_STATE state_app::app_state() const
{
    return app_st;
}


void state_app::incomingServiceBeforeLoading(const iviLink::Service::Uid &service)
{
    //assert( app_state() == AWAITING_SLAVE_MODE_ );
    // Oops...
    if(app_state()!= AWAITING_SLAVE_MODE_) {
      LOG4CPLUS_ERROR(log4, "incomingServiceBeforeLoading(): init was not done! But this callback indicates we are the slave");
      //switch2AWAITINGSLAVE();
    }
    LOG4CPLUS_INFO(log4, "state_app: incoming service `" +service.value()+ "` before loading");
}

void state_app::incomingServiceAfterLoading(const iviLink::Service::Uid &service)
{
    assert( app_state() == AWAITING_SLAVE_MODE_ );
    LOG4CPLUS_INFO(log4, "state_app: incoming service `" +service.value() + "` after loading");
    if( setup_profiles() )

        switch2SLAVE();
    else
    {
        LOG4CPLUS_ERROR( log4, "state_app: failed to setup profiles pointers. Switching to OFFLINE" );
        release_profiles();
        switch2OFFLINE();
    }
}

void state_app::serviceLoadError(const iviLink::Service::Uid &service)
{
    LOG4CPLUS_ERROR( log4, "state_app: service '" + service.value() + "' load error. Switching to OFFLINE." );
    release_profiles();
    switch2OFFLINE();
}

void state_app::serviceDropped(const iviLink::Service::Uid& service)
{
    LOG4CPLUS_INFO( log4, "state_app: service '" + service.value() + "' dropped. Switching to OFFLINE" );
    release_profiles();
    switch2OFFLINE();
}

void state_app::saveState()
{
    LOG4CPLUS_INFO( log4, "state_app: saveState()" );
    try_save( st, state_file );
}



#ifndef ANDROID
state_app::state_app( const iviLink::Service::Uid& service_uid
#else
state_app::state_app( iviLink::Android::AppInfo appInfo, const iviLink::Service::Uid& service_uid
#endif //ANDROID
                      , const iviLink::Profile::ApiUid& sender_uid
                      , const iviLink::Profile::ApiUid& receiver_uid
                      , const std::string& state_file) :
    #ifndef ANDROID
    CApp(service_uid)
    #else
    CApp(service_uid, appInfo)
    , mAppInfo(appInfo)
    #endif //ANDROID
    , app_st(CREATING_)
    , sender(0)
    , receiver(0)
    , state_file(state_file)
    , sender_uid(sender_uid)
    , receiver_uid(receiver_uid)
    , service_uid(service_uid)
{
    registerProfileCallbacks(sender_uid, &sender_cb);
    registerProfileCallbacks(receiver_uid, &receiver_cb);

    using namespace std::tr1::placeholders;
    sender_cb.set_error_handler( std::tr1::bind( &state_app::profile_fail, this, true, _1 ) );
    receiver_cb.set_error_handler( std::tr1::bind( &state_app::profile_fail, this, false, _1 ) );

    init(st); // init by default values - for pb this is different from default construction
    try_load(st, state_file);
    switch2CREATED();
}
