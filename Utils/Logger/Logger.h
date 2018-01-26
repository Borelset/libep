//
// Created by borelset on 1/18/18.
//

#ifndef LIBEP_LOGGER_H
#define LIBEP_LOGGER_H

#include <bits/unique_ptr.h>
#include "../noncopyable.h"
#include "../Thread.h"
#include "../Buffer.h"
#include "../Mutex.h"
#include "../FD.h"

namespace Log{
    constexpr static int BUFFER_NUMS = 2;
    constexpr static int BUFFER_FLASH_THRESHOLD = 20;

    enum LogLevel{
        Info,
        Debug,
        Warning,
        Error,
        Nothing
    };

    class Logger : Utils::noncopyable{
    public:
        explicit Logger(LogLevel level);
        ~Logger();
        LogLevel getLevel();
        void setLevel(LogLevel level);
        void append(char* log, int length);
        void flush();
        void setLogPath(char* path);
    private:

        Utils::Thread mWriteThread;
        Utils::Buffer mBuffers[BUFFER_NUMS];
        Utils::MutexLock mMutexLock;
        Utils::Condition mCondition;
        int mCounter;
        int mUsing;
        bool mQuit;
        bool mIsLog;
        LogLevel mLevel;
        std::unique_ptr<Utils::FD> mLogFilePtr;

        void bufferSwapCallback();
    };


}

#endif //LIBEP_LOGGER_H
