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
#ifndef STATE_SERIALIZATION_HPP
#define STATE_SERIALIZATION_HPP

#include <fstream>
#include <string>
#include <vector>
#include "logger-decl.hpp"

template<typename T>
bool tryLoad(T& state, const std::string& file)
{
    LOG4CPLUS_TRACE_METHOD(log4StateApp(), __PRETTY_FUNCTION__);
    using namespace std;

    ifstream f(file.c_str(), ios::in | ios::binary);
    if (!f)
    {
        LOG4CPLUS_WARN(log4StateApp(), "Failed to load prev state from "+ file);
        return false;
    }
    else
    {
        // get size of file
        f.seekg(0,ifstream::end);
        const size_t size= f.tellg();
        f.seekg(0);

        vector<char> buf(size);

        if (!f.read(&*buf.begin(), buf.size()))
        {
            LOG4CPLUS_WARN(log4StateApp(), "Failed to read prev state from "+ file);
            return false;
        }

        T st;
        if (!st.deserialize(std::string(buf.begin(),buf.end())))
        {
            LOG4CPLUS_WARN(log4StateApp(), "Failed to parse prev state from "+ file);
            return false;
        }
        else
        {
            st.setCookie(0);
            state = st;
            return true;
        }
    }
}

template<typename T>
bool trySave(const T& state, const std::string& file)
{
    LOG4CPLUS_TRACE_METHOD(log4StateApp(), __PRETTY_FUNCTION__);
    using namespace std;

    const string tmp_file = file + ".tmp";
    string buf;
    buf = state.serialize();

    ofstream f(tmp_file.c_str(), ios::out | ios::binary);
    if (!f || f.write(buf.c_str(), buf.size()).bad())
    {
        LOG4CPLUS_WARN(log4StateApp(), "Failed to serialize last state to " + tmp_file);
        return false;
    }

    f.flush();
    f.close();
    if (rename(tmp_file.c_str(), file.c_str( )))
    {
        LOG4CPLUS_ERROR(log4StateApp(), "Failed to rename state file " + tmp_file + "to " + file);
        return false;
    }
    return true;
}


#endif // STATE_SERIALIZATION_HPP

