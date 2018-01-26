//
// Created by borelset on 1/18/18.
//

#include <memory>
#include <cstring>
#include "LoggerStream.h"

Log::LoggerStream::LoggerStream(std::weak_ptr <Logger> logger, LogLevel level):
        mLogger(logger),
        mLevel(level),
        mLineStart(true),
        mConsole(false)
{
    mOutput = logger.lock()->getLevel() <= level ? 1 : 0;
}

Log::LoggerStream &Log::LoggerStream::operator<<(const char * str) {
    if(mConsole && mOutput){
        std::cout << str;
    }
    if(mOutput){
        mLogger.lock()->append((char*)str, strlen(str));
    }
    return *this;
}

Log::LoggerStream &Log::LoggerStream::operator<<(int num) {
    if(mConsole && mOutput){
        std::cout << num;
    }
    if(mOutput){
        char numbuffer[32];
        sprintf(numbuffer, "%d", num);
        mLogger.lock()->append(numbuffer, strlen(numbuffer));
    }
    return *this;
}

Log::LoggerStream &Log::LoggerStream::operator<<(Log::LoggerStream&) {
    if(mOutput){
        mLogger.lock()->flush();
    }
    return *this;
}

Log::LoggerStream &Log::LoggerStream::operator<<(Log::LoggerEndl &) {
    if(mConsole && mOutput){
        std::cout << std::endl;
    }
    if(mOutput){
        mLogger.lock()->append((char*)"\n", 1);
    }
    return *this;
}

Log::LoggerStream &Log::LoggerStream::operator<<(std::string& str) {
    if(mConsole && mOutput){
        std::cout << str;
    }
    if(mOutput){
        mLogger.lock()->append((char*)str.c_str(), str.size());
    }
    return *this;
}

void Log::LoggerStream::updateLevelTest() {
    mOutput = mLogger.lock()->getLevel() <= mLevel ? 1 : 0;
}
