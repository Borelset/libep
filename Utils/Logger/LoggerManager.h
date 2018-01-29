//
// Created by borelset on 1/18/18.
//

#ifndef LIBEP_LOGGERMANAGER_H
#define LIBEP_LOGGERMANAGER_H

#include <map>
#include "Logger.h"
#include "LoggerStream.h"

namespace Log{
    constexpr static char* LOG_DEFAULT_PATH = (char*)"./log.log";
    constexpr static LogLevel DEFAULT_LOGGER_LEVEL = Error;

    class LoggerManager : Utils::noncopyable{
    public:
        static LoggerManager& getInstance();
        LoggerStream& getStream(LogLevel level);
        void setLogLevel(LogLevel level);
        void setLogPath(char*);
    private:
        explicit LoggerManager(LogLevel level = DEFAULT_LOGGER_LEVEL);
        ~LoggerManager();
        std::shared_ptr<Logger> mLogger;
        std::map<LogLevel, std::shared_ptr<LoggerStream>> mLoggerStreams;
        LoggerManager* mLoggerManager;
    };

    static Log::LoggerManager& gLoggerManager = Log::LoggerManager::getInstance();
    static Log::LoggerStream& LogInfo = gLoggerManager.getStream(Info);
    static Log::LoggerStream& LogDebug = gLoggerManager.getStream(Debug);
    static Log::LoggerStream& LogWarning = gLoggerManager.getStream(Warning);
    static Log::LoggerStream& LogError = gLoggerManager.getStream(Error);
    static Log::LoggerEndl endl;
}

#endif //LIBEP_LOGGERMANAGER_H
