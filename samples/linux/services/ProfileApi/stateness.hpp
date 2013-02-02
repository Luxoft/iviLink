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


#if !defined(STATENESS_HPP)
#define STATENESS_HPP

#include <tr1/functional>
#include "BaseProfileApi.hpp"

template<typename T>
class stateness_sender {
public:

    typedef T state_t;

    virtual void send_state( const T& st ) = 0;

    virtual ~stateness_sender() {}
};


template<typename T>
class stateness_receiver {
public:

    typedef T state_t;

    typedef std::tr1::function<void ( const state_t& )> callback_t;

    void register_receiver( const callback_t& on_receive )
    {
        receiver = on_receive;
    }

    virtual ~stateness_receiver() {}

protected:
    void receive( const state_t& st )
    {
        if( receiver )
            receiver( st );
    }

private:
    callback_t receiver;
};


/*******************************************************************

The channel's tags swap scheme.

 ==========================           =============================
|        Slave             |         |            Master           |
|==========================|         |=============================|
| Sender(ch="XXXReceiver") |----\ /--|   Sender(ch="XXXSender")    |
|--------------------------|     X   |-----------------------------|
| Receiver(ch="XXXSender") |____/ \__| Receiver(ch="XXXReceiver")  |
 ==========================           =============================

********************************************************************/

template<typename T, typename U>
class channel_tag {
public:
    channel_tag() : swapped(false) {}
    void swap_channels() { swapped = !swapped; }

    std::string get() const { return (swapped ? U::uid() : T::uid()).value(); }

private:
    bool swapped;
};

template<typename T, typename U>
class ErrorHandleCallback : public iviLink::Profile::IProfileCallbackProxy {
public:
    typedef std::tr1::function<void (const BaseError& )> err_handler_callback_t;

    ErrorHandleCallback() {}
    void swap_channels() { ch_tag.swap_channels(); }
    std::string get_channel_id() const { return ch_tag.get(); }

    void set_error_handler( const err_handler_callback_t& handler) { handler_= handler; }

protected:
    virtual void handleError( const BaseError& e ) // from IProfileCallbackProxy
    {
        if( handler_ ) handler_(e);
    }
private:
    err_handler_callback_t handler_;
    channel_tag<T,U> ch_tag;
};


template<typename T, typename U>
class IStateProfileBase : public iviLink::BaseProfileApi
{
public:
    typedef ErrorHandleCallback<T,U>      profile_callback_t;
    typedef iviLink::Profile::ApiUid      uid_t;

    virtual uid_t getProfileApiUid() const // from CProfileApiBase
    {
        return T::uid();
    }
};

#endif //STATENESS_HPP
