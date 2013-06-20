
/**
 *
 */

/**
 * @ingroup iOS Port
 * @author Bratanova Elena <ebratanova@luxoft.com>
 * @date 11.03.2013
 *
 * Provides log4cplus-like interface to use NSLog.
 */
#ifdef __APPLE__
#include <sstream>

#include "IOsLogger.hpp"

void PropertyConfigurator::doConfigure() {}
void PropertyConfigurator::doConfigure(const char *properties) {}

namespace IosLog {
    
    std::string convertIntegerToString(unsigned long intVal)
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
    
    void Logger::trace(const std::string& event, const std::string& file, const std::string& func, const int& line) const
    {
        printLog("V", event, file, func, line);
    }
    
    void Logger::debug(const std::string& event, const std::string& file, const std::string& func, const int& line) const
    {
        printLog("D", event, file, func, line);
    }
    
    void Logger::info(const std::string& event, const std::string& file, const std::string& func, const int& line) const
    {
        printLog("I", event, file, func, line);
    }
    
    void Logger::warn(const std::string& event, const std::string& file, const std::string& func, const int& line) const
    {
        printLog("W", event, file, func, line);
    }
    
    void Logger::error(const std::string& event, const std::string& file, const std::string& func, const int& line) const
    {
        printLog("E", event, file, func, line);
    }
    
    void Logger::fatal(const std::string& event, const std::string& file, const std::string& func, const int& line) const
    { 
        std::string fatalEvent=std::string(" FATAL: ")+event;
        error(fatalEvent, file, func, line);
    }
    
    Logger::Logger(const std::string& tagStr)
    {
        mTag=tagStr;
    }
    
    inline void Logger::printLog(const char * verbosity, const std::string& event, const std::string& file, const std::string& func, const int& line) const
    {
        std::string message = std::string(verbosity) + ": " + mTag + " : " +  event;
        NSLog([NSString stringWithUTF8String:message.c_str()]);
    }
    
    Tracer::Tracer(const Logger& logger, const std::string& logEvent)
    : mTag(logger.mTag)
    , mEvent(logEvent)
    {
        std::string msg = "V: " + mTag + std::string(" ENTER: ") + mEvent;
        NSLog([NSString stringWithUTF8String:msg.c_str()]);
    }
    
    Tracer::~Tracer()
    {
        std::string msg = "V: " + mTag + std::string(" EXIT: ") + mEvent;
        NSLog([NSString stringWithUTF8String:msg.c_str()]);
    }

}

#endif