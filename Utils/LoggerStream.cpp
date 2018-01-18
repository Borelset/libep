//
// Created by borelset on 1/18/18.
//

#include <memory>
#include <cstring>
#include "LoggerStream.h"

Utils::LoggerStream::LoggerStream(std::weak_ptr <Logger> logger, LogLevel level):
        mLogger(logger),
        mLevel(level),
        mLineStart(true)
{
    mOutput = logger.lock()->getLevel() <= level ? 1 : 0;
}

Utils::LoggerStream &Utils::LoggerStream::operator<<(char * str) {
    if(mOutput){
        mLogger.lock()->append(str, strlen(str));
    }
    return *this;
}

Utils::LoggerStream &Utils::LoggerStream::operator<<(int num) {
    if(mOutput){
        char numbuffer[32];
        sprintf(numbuffer, "%d", num);
        mLogger.lock()->append(numbuffer, strlen(numbuffer));
    }
    return *this;
}

Utils::LoggerStream &Utils::LoggerStream::operator<<(Utils::LoggerStream&) {
    if(mOutput){
        mLogger.lock()->flush();
    }
    return *this;
}

Utils::LoggerStream &Utils::LoggerStream::operator<<(Utils::LoggerEndl &) {
    if(mOutput){
        mLogger.lock()->append("\n", 1);
    }
    return *this;
}

Utils::LoggerStream &Utils::LoggerStream::operator<<(std::string & str) {
    if(mOutput){
        mLogger.lock()->append((char*)str.c_str(), str.size());
    }
    return *this;
}

Utils::LoggerStream &Utils::LoggerStream::operator<<(std::string str) {
    std::cout << mOutput << std::endl;
    if(mOutput){
        mLogger.lock()->append((char*)str.c_str(), str.size());
    }
    return *this;
}

void Utils::LoggerStream::updateLevelTest() {
    mOutput = mLogger.lock()->getLevel() <= mLevel ? 1 : 0;
}
