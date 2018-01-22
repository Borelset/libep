//
// Created by borelset on 1/18/18.
//

#include <fcntl.h>
#include <cstring>
#include "Logger.h"
#include "unistd.h"

Log::Logger::Logger(LogLevel level):
        mWriteThread(
                std::bind(&Logger::bufferSwapCallback, this)
        ),
        mCondition(mMutexLock),
        mCounter(0),
        mUsing(0),
        mQuit(false),
        mIsLog(false),
        mLevel(Info),
        mLogFilePtr()
{
    std::cout << "Utils::Logger::Logger==>"
              << "Construction" << std::endl;
    mWriteThread.run();
}

Log::Logger::~Logger() {
    std::cout << "Logger is Releasing!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
    mQuit = true;
    mCondition.notify();
    mWriteThread.join();
}


void Log::Logger::bufferSwapCallback() {
    while(!mQuit){
        {
            Utils::MutexLockGuard localLock(mMutexLock);
            mCondition.wait();
            mUsing = (mUsing+1)%BUFFER_NUMS;
            mBuffers[mUsing].clear();
            mCounter = 0;
        }
        if(mLogFilePtr.get() == nullptr){
            std::cout << "Utils::Logger::bufferSwapCallback==>"
                      << "No LoggerFile Path" << std::endl;
            mIsLog = true;
            continue;
        }
        int n = mBuffers[(mUsing+BUFFER_NUMS-1)%BUFFER_NUMS].writeToFD(mLogFilePtr->getFd());
        std::cout << "Utils::Logger::bufferSwapCallback==>"
                  << "Logger Thread write " << n << " character(s)" << std::endl;
    }
}

void Log::Logger::append(char *log, int length) {
    if(!mIsLog){
        return;
    }
    {
        Utils::MutexLockGuard localLock(mMutexLock);
        mBuffers[mUsing].writeBuffer(log, length);
        if(strchr(log, '\n'))
            mCounter++;
    }
    if(mCounter >= BUFFER_FLASH_THRESHOLD){
        mCondition.notify();
    }
}

Log::LogLevel Log::Logger::getLevel() {
    return mLevel;
}

void Log::Logger::flush() {
    mCondition.notify();
}

void Log::Logger::setLevel(Log::LogLevel level) {
    mLevel = level;
}

void Log::Logger::setLogPath(char *path) {
    int fd = open(path, O_RDWR|O_CREAT, O_APPEND);
    if(fd < 0){
        std::cout << "Utils::Logger::Logger==>"
                  << "errno: " << errno << std::endl;
    }
    mLogFilePtr.release();
    mLogFilePtr.reset(new Utils::FD(fd));
}

