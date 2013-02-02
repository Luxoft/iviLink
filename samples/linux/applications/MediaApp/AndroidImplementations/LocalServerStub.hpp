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
#ifndef CLOCALSERVERSTUB_HPP
#define CLOCALSERVERSTUB_HPP

#include <string>

#include "IServer.hpp"
#include "Logger.hpp"

class LocalServerStub: public IServer
{
public:
    LocalServerStub();
    virtual ~LocalServerStub();
    virtual void resume();
    virtual void pause();
    virtual void stop();
    virtual void play(std::string const& url, std::string const& form);
    virtual std::string getSource();

private:
    Logger mLogger;

    std::string mLatestUrl;
};

#endif // CLOCALSERVERSTUB_HPP
