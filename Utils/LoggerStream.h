//
// Created by borelset on 1/18/18.
//

#ifndef LIBEP_LOGGERSTREAM_H
#define LIBEP_LOGGERSTREAM_H

#include <memory>
#include "noncopyable.h"
#include "Logger.h"

namespace Utils{
    class LoggerEndl{
    };

    class LoggerStream{
    public:
        LoggerStream(std::weak_ptr<Logger> logger, LogLevel);
        LoggerStream&operator<<(char*);
        LoggerStream&operator<<(std::string&);
        LoggerStream&operator<<(std::string);
        LoggerStream&operator<<(int);
        LoggerStream&operator<<(LoggerStream&);
        LoggerStream&operator<<(LoggerEndl&);
        void updateLevelTest();
    private:
        LogLevel mLevel;
        bool mOutput;
        bool mLineStart;
        std::weak_ptr<Logger> mLogger;
    };
}

#endif //LIBEP_LOGGERSTREAM_H
