//
// Created by borelset on 1/18/18.
//

#ifndef LIBEP_LOGGER_H
#define LIBEP_LOGGER_H

#include "noncopyable.h"
#include "Thread.h"
#include "Buffer.h"
#include "Mutex.h"
#include "FD.h"

namespace Utils{
    static const int BUFFER_NUMS = 2;
    static const int BUFFER_FLASH_LIMIT = 2;

    enum LogLevel{
        Info,
        Debug,
        Warning,
        Error
    };
    static std::vector<LogLevel> LogLevelCollection = {Info, Debug, Warning, Error};

    class Logger : Utils::noncopyable{
    public:
        explicit Logger(LogLevel level, char* path);
        ~Logger();
        LogLevel getLevel();
        void setLevel(LogLevel level);
        void append(char* log, int length);
        void flush();
    private:

        Utils::Thread mWriteThread;
        Utils::Buffer mBuffers[BUFFER_NUMS];
        Utils::MutexLock mMutexLock;
        Utils::Condition mCondition;
        int mCounter;
        int mUsing;
        bool mQuit;
        LogLevel mLevel;
        Utils::FD mLogFile;

        void bufferSwapCallback();
    };


}

#endif //LIBEP_LOGGER_H
