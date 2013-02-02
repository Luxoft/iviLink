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
#ifndef I_STATE_HPP
#define I_STATE_HPP

#include <string>

#include "Logger.hpp"
#include "Types.hpp"
#include "json/json.h"

#define STATE_COOKIE_TAG "cookie"

namespace iviLink
{

enum CompareResult
{
    EQUAL,
    GREATER,
    LESS,
};

class State 
{
public:
    State() : mCookie(0) {}
    virtual ~State() {}
    virtual std::string serialize() const = 0;
    virtual bool deserialize(const std::string& newState) = 0;

    void appendCookieToJson(Json::Value& root) const
    {
        LOG4CPLUS_TRACE_METHOD(Logger::getInstance("samples.state"), __PRETTY_FUNCTION__);
        root[STATE_COOKIE_TAG] = mCookie;
        LOG4CPLUS_INFO(Logger::getInstance("samples.state"), "mCookie = " + convertIntegerToString(mCookie));
    }

    void setCookieFromJson(const Json::Value root)
    {
        LOG4CPLUS_TRACE_METHOD(Logger::getInstance("samples.state"), __PRETTY_FUNCTION__);
        LOG4CPLUS_INFO(Logger::getInstance("samples.state"), "mCookie = " + convertIntegerToString(mCookie));
        mCookie = root.get(STATE_COOKIE_TAG, mCookie).asInt();
        LOG4CPLUS_INFO(Logger::getInstance("samples.state"), "after set from json mCookie = " + convertIntegerToString(mCookie));
    }

    void touchCookie()
    {
        LOG4CPLUS_TRACE_METHOD(Logger::getInstance("samples.state"), __PRETTY_FUNCTION__);
        LOG4CPLUS_INFO(Logger::getInstance("samples.state"), "mCookie = " + convertIntegerToString(mCookie));
        mCookie ++;
        LOG4CPLUS_INFO(Logger::getInstance("samples.state"), "after touch mCookie = " + convertIntegerToString(mCookie));
    }
    void setCookie(const int cookie)
    {
        LOG4CPLUS_TRACE_METHOD(Logger::getInstance("samples.state"), __PRETTY_FUNCTION__);
        LOG4CPLUS_INFO(Logger::getInstance("samples.state"), "mCookie = " + convertIntegerToString(mCookie) + " cookie = " + convertIntegerToString(cookie));
        mCookie = cookie;
        LOG4CPLUS_INFO(Logger::getInstance("samples.state"), "after set mCookie = " + convertIntegerToString(mCookie));
    }

    int getCookie() const
    {
        return mCookie;
    }

    CompareResult compareTo(const State& arg) const
    {
        LOG4CPLUS_TRACE_METHOD(Logger::getInstance("samples.state"), __PRETTY_FUNCTION__);
        if (getCookie() == arg.getCookie())
        {
            LOG4CPLUS_INFO(Logger::getInstance("samples.state"), "EQUAL!");
            return EQUAL;
        }

        if(getCookie() > arg.getCookie()) 
        {
            LOG4CPLUS_INFO(Logger::getInstance("samples.state"), "GREATER!");
            return GREATER;
        }

        LOG4CPLUS_INFO(Logger::getInstance("samples.state"), "LESS!");
        return LESS;
    }

protected:
    int mCookie;

};
}

#endif //I_STATE_HPP
