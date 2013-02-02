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
#else

#include <sstream>

#include "AndroidLogger.hpp"
#include "Types.hpp"

namespace PropertyConfigurator
{
    void doConfigure()
    {

    }

    void doConfigure(const char *)
    {

    }
}

namespace AndroidLog 
{
    std::string convertIntegerToString(UInt32 intVal)
    {
        std::stringstream ss; //create a stringstream
        ss << intVal;//add number to the stream
        return ss.str();//return a string with the contents of the stream
    }

    Logger Logger::getInstance()
    {
        return getInstance(std::string("default"));
    }

    Logger Logger::getInstance(std::string str)
    {
        Logger log(str);
        return log;
    }

    void Logger::checkAssertion(bool condition, const std::string& func, const std::string& file, int line, const std::string& strCondition)
    {
        if(!condition)
        {
            // actual assert
            __android_log_assert(0,"iviLink.assert","Assertion failed: %s(%s:%d) >> %s ",func.c_str() ,file.c_str(), line, strCondition.c_str());
            // print of an error - to be switched prior to release
            //_android_log_print(ANDROID_LOG_ERROR,"iviLink.assert","Assertion failed: %s(%s:%d) >> %s ",func.c_str() ,file.c_str(), line, strCondition.c_str());
        }
    }

    void Logger::trace(const std::string& event, const std::string& file, const std::string& func, const int& line) const
    {
        printLog(ANDROID_LOG_VERBOSE, event, file, func, line);
    }

    void Logger::debug(const std::string& event, const std::string& file, const std::string& func, const int& line) const
    {
        printLog(ANDROID_LOG_DEBUG, event, file, func, line);
    }

    void Logger::info(const std::string& event, const std::string& file, const std::string& func, const int& line) const
    {
        printLog(ANDROID_LOG_INFO, event, file, func, line);
    }

    void Logger::warn(const std::string& event, const std::string& file, const std::string& func, const int& line) const
    {
        printLog(ANDROID_LOG_WARN, event, file, func, line);
    }

    void Logger::error(const std::string& event, const std::string& file, const std::string& func, const int& line) const
    {
        printLog(ANDROID_LOG_ERROR, event, file, func, line);
    }

    void Logger::fatal(const std::string& event, const std::string& file, const std::string& func, const int& line) const
    {
        std::string fatalEvent=std::string("FATAL: ")+event;
        error(fatalEvent, file, func, line);
    }

    Logger::Logger(const std::string& tagStr)
    {
        mTag=tagStr;
    }

    inline void Logger::printLog(int verbosityLevel, const std::string& event, const std::string& file, const std::string& func, const int& line) const
    {
        // excess parameters may be used to increase logging - see the commented code below
        /*std::string message = "file: " + file +" func: " + func + " line: " + convertIntegerToString(line);
         __android_log_print(verbosityLevel, mTag.c_str(), message.c_str());*/
        __android_log_print(verbosityLevel, mTag.c_str(), event.c_str());
    }

    Tracer::Tracer(const Logger& logger, const std::string& logEvent)
    : mTag(logger.mTag)
    , mEvent(logEvent)
    {
        std::string msg = std::string("ENTER: ")+mEvent;
        __android_log_print(ANDROID_LOG_VERBOSE, mTag.c_str(), msg.c_str());
    }

    Tracer::~Tracer()
    {
        std::string msg = std::string("EXIT: ") + mEvent;
        __android_log_print(ANDROID_LOG_VERBOSE, mTag.c_str(), msg.c_str());
    }
} // namespace AndroidLog

#endif
