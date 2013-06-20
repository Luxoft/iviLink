
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

#ifndef iviLinkFwDraft_IOsLogger_hpp
#define iviLinkFwDraft_IOsLogger_hpp
#ifdef __APPLE__

#include <string>
#include "Types.hpp"

#define LOG4CPLUS_TEXT(string) string

#ifdef LOG_ENABLED

#define LOG4CPLUS_TRACE_METHOD(logger,logEvent) Tracer tracer(logger, logEvent)

#define LOG4CPLUS_TRACE(logger, logEvent)    logger.trace(logEvent, __FILE__, __func__, __LINE__)
#define LOG4CPLUS_DEBUG(logger, logEvent)    logger.debug(logEvent, __FILE__, __func__, __LINE__)
#define LOG4CPLUS_INFO(logger, logEvent)     logger.info(logEvent, __FILE__, __func__, __LINE__)
#define LOG4CPLUS_WARN(logger, logEvent)     logger.warn(logEvent, __FILE__, __func__, __LINE__)
#define LOG4CPLUS_ERROR(logger, logEvent)    logger.error(logEvent, __FILE__, __func__, __LINE__)
#define LOG4CPLUS_FATAL(logger, logEvent)    logger.fatal(logEvent, __FILE__, __func__, __LINE__)

#else

#define LOG4CPLUS_TRACE_METHOD(logger,logEvent)
#define LOG4CPLUS_TRACE(logger, logEvent)   
#define LOG4CPLUS_DEBUG(logger, logEvent)
#define LOG4CPLUS_INFO(logger, logEvent)
#define LOG4CPLUS_WARN(logger, logEvent)
#define LOG4CPLUS_ERROR(logger, logEvent)
#define LOG4CPLUS_FATAL(logger, logEvent)
#endif


namespace IosLog
{
    std::string convertIntegerToString(unsigned long intVal);
    class Logger
    {
        friend class Tracer;
        
    public:
        static Logger getInstance();
        static Logger getInstance(std::string str);
        
        void trace(const std::string& event, const std::string& file, const std::string& func, const int& line) const;
        
        void debug(const std::string& event, const std::string& file, const std::string& func, const int& line) const;
        
        void info(const std::string& event, const std::string& file, const std::string& func, const int& line) const;
        
        void warn(const std::string& event, const std::string& file, const std::string& func, const int& line) const;
        
        void error(const std::string& event, const std::string& file, const std::string& func, const int& line) const;
        
        void fatal(const std::string& event, const std::string& file, const std::string& func, const int& line) const;
        
    private:
        std::string mTag;
        Logger(const std::string& tagStr);
        
        void printLog(const char * verbosityTag, const std::string& event, const std::string& file, const std::string& func, const int& line) const;
    };
    
    class Tracer
    {
        std::string mEvent;
        std::string mTag;
    public:
        Tracer(const Logger& logger, const std::string& logEvent);
        ~Tracer();
        
    }; // Tracer
} // namespace IosLog

namespace PropertyConfigurator
{
    void doConfigure();
    void doConfigure(const char * properties);
}

#endif
#endif
