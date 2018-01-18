//
// Created by borelset on 1/18/18.
//

#include "Logger.h"
#include "LoggerManager.h"
#include <algorithm>

Utils::LoggerManager::LoggerManager(LogLevel level, char *path):
        mLogger(new Logger(level, path))
{

}

Utils::LoggerManager::~LoggerManager() {

}

Utils::LoggerStream& Utils::LoggerManager::getStream(Utils::LogLevel level) {
    if(!mLoggerStreams[level]){
        std::shared_ptr<LoggerStream> ptr(new LoggerStream(mLogger, level));
        mLoggerStreams[level] = ptr;
    }
    return *mLoggerStreams[level];
}

void Utils::LoggerManager::setLogLevel(Utils::LogLevel level) {
    mLogger->setLevel(level);
    for(auto i = mLoggerStreams.begin(); i!= mLoggerStreams.end(); i++){
        i->second->updateLevelTest();
    }
}
