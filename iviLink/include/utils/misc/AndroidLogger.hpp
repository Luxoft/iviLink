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


#ifndef IVI_LINK_LOGGER_HPP_
#define IVI_LINK_LOGGER_HPP_

#ifndef ANDROID
#else

#include <string>
#include <android/log.h>
#include "Types.hpp"

#define LOG4CPLUS_TEXT(string) string

#define LOG4CPLUS_TRACE_METHOD(logger,logEvent) Tracer tracer(logger, logEvent)

#define LOG4CPLUS_TRACE(logger, logEvent)    logger.trace(logEvent, __FILE__, __func__, __LINE__)
#define LOG4CPLUS_DEBUG(logger, logEvent)    logger.debug(logEvent, __FILE__, __func__, __LINE__)
#define LOG4CPLUS_INFO(logger, logEvent)     logger.info(logEvent, __FILE__, __func__, __LINE__)
#define LOG4CPLUS_WARN(logger, logEvent)     logger.warn(logEvent, __FILE__, __func__, __LINE__)
#define LOG4CPLUS_ERROR(logger, logEvent)    logger.error(logEvent, __FILE__, __func__, __LINE__)
#define LOG4CPLUS_FATAL(logger, logEvent)    logger.fatal(logEvent, __FILE__, __func__, __LINE__)

#undef assert
#define assert(e) Logger::checkAssertion(e, __func__ ,__FILE__, __LINE__, #e);


namespace AndroidLog
{
    std::string convertIntegerToString(UInt32 intVal);

    class Logger
    {
        friend class Tracer;

    public:
        static Logger getInstance();
        static Logger getInstance(std::string str);

        static void checkAssertion(bool condition, const std::string& func, const std::string& file, int line, const std::string& strCondition);

        void trace(const std::string& event, const std::string& file, const std::string& func, const int& line) const;

        void debug(const std::string& event, const std::string& file, const std::string& func, const int& line) const;

        void info(const std::string& event, const std::string& file, const std::string& func, const int& line) const;

        void warn(const std::string& event, const std::string& file, const std::string& func, const int& line) const;

        void error(const std::string& event, const std::string& file, const std::string& func, const int& line) const;

        void fatal(const std::string& event, const std::string& file, const std::string& func, const int& line) const;

    private:
        std::string mTag;
        Logger(const std::string& tagStr);

        void printLog(int verbosityLevel, const std::string& event, const std::string& file, const std::string& func, const int& line) const;
    };

    class Tracer
    {
        std::string mEvent;
        std::string mTag;
    public:
        Tracer(const Logger& logger, const std::string& logEvent);
        ~Tracer();

    }; // Tracer
} // namespace AndroidLog

namespace PropertyConfigurator
{
    void doConfigure();
    void doConfigure(const char * properties);
}

#endif //ANDROID
#endif /* IVI_LINK_LOGGER_HPP_ */
