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

#ifndef ANDROID

#include "CServer.h"

Logger CServer::msLogger = Logger::getInstance(
        LOG4CPLUS_TEXT("samples.Applications.Media.CServer"));

CServer::CServer()
{
    mLocalSource = "";
}

CServer::~CServer()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
}

void CServer::play(std::string const& url, std::string const& form)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    mLocalSource = url;
}

void CServer::stop()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
}

void CServer::pause()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
}

void CServer::resume()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
}

std::string CServer::getSource()
{
    return mLocalSource;
}
#endif //ANDROID
