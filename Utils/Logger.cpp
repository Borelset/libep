//
// Created by borelset on 1/18/18.
//

#include <fcntl.h>
#include <cstring>
#include "Logger.h"
#include "unistd.h"

Utils::Logger::Logger(LogLevel level, char* path):
        mWriteThread(
                std::bind(&Logger::bufferSwapCallback, this)
        ),
        mCondition(mMutexLock),
        mCounter(0),
        mUsing(0),
        mQuit(0),
        mLevel(Info),
        mLogFile(open(path, O_RDWR|O_CREAT, O_APPEND))
{
    std::cout << "Utils::Logger::Logger==>"
              << "Logger file fd: " << mLogFile.getFd() << std::endl;
    if(mLogFile.getFd() == -1){
        std::cout << "Utils::Logger::Logger==>"
                  << "errno: " << errno << std::endl;
    }
    mWriteThread.run();
}

Utils::Logger::~Logger() {
    mQuit = true;
    mCondition.notify();
    mWriteThread.join();
}


void Utils::Logger::bufferSwapCallback() {
    while(!mQuit){
        {
            Utils::MutexLockGuard localLock(mMutexLock);
            mCondition.wait();
            mUsing = (mUsing+1)%BUFFER_NUMS;
            mBuffers[mUsing].clear();
            mCounter = 0;
        }
        int n = mBuffers[(mUsing+BUFFER_NUMS-1)%BUFFER_NUMS].writeToFD(mLogFile.getFd());
        std::cout << "Utils::Logger::bufferSwapCallback==>"
                  << "Logger Thread write " << n << " character(s)" << std::endl;
    }
}

void Utils::Logger::append(char *log, int length) {
    {
        Utils::MutexLockGuard localLock(mMutexLock);
        mBuffers[mUsing].writeBuffer(log, length);
        if(strchr(log, '\n'))
            mCounter++;
        std::cout << mCounter <<std::endl;
    }
    if(mCounter >= BUFFER_FLASH_LIMIT){
        mCondition.notify();
    }
}

Utils::LogLevel Utils::Logger::getLevel() {
    return mLevel;
}

void Utils::Logger::flush() {
    mCondition.notify();
}

void Utils::Logger::setLevel(Utils::LogLevel level) {
    mLevel = level;
}

