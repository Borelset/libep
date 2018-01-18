//
// Created by borelset on 1/18/18.
//

#include "Logger.h"
#include "LoggerManager.h"
#include <algorithm>

Log::LoggerManager::LoggerManager(LogLevel level):
        mLogger(new Logger(level))
{

}

Log::LoggerManager::~LoggerManager() {

}

Log::LoggerStream& Log::LoggerManager::getStream(Log::LogLevel level) {
    if(!mLoggerStreams[level]){
        std::shared_ptr<LoggerStream> ptr(new LoggerStream(mLogger, level));
        mLoggerStreams[level] = ptr;
    }
    return *mLoggerStreams[level];
}

void Log::LoggerManager::setLogLevel(Log::LogLevel level) {
    mLogger->setLevel(level);
    for(auto i = mLoggerStreams.begin(); i!= mLoggerStreams.end(); i++){
        i->second->updateLevelTest();
    }
}

void Log::LoggerManager::setLogPath(char * path) {
    mLogger->setLogPath(path);
}

Log::LoggerManager &Log::LoggerManager::getInstance() {
    static LoggerManager instance;
    return instance;
}
