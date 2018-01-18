//
// Created by borelset on 1/18/18.
//

#ifndef LIBEP_LOGGERMANAGER_H
#define LIBEP_LOGGERMANAGER_H

#include <map>
#include "Logger.h"
#include "LoggerStream.h"

namespace Utils{
    static const char* LOG_DEFAULT_PATH = "./log.log";
    static const LogLevel DEFAULT_LOGGER_LEVEL = Info;

    class LoggerManager : Utils::noncopyable{
    public:
        LoggerManager(LogLevel level = DEFAULT_LOGGER_LEVEL,
                      char* path = (char*)LOG_DEFAULT_PATH);
        ~LoggerManager();
        LoggerStream& getStream(LogLevel level);
        void setLogLevel(LogLevel level);
    private:
        std::shared_ptr<Logger> mLogger;
        std::map<LogLevel, std::shared_ptr<LoggerStream>> mLoggerStreams;
    };
}

#endif //LIBEP_LOGGERMANAGER_H
