//
// Created by borelset on 1/17/18.
//

#ifndef LIBEP_BUFFER_H
#define LIBEP_BUFFER_H

#include <vector>
#include <string>

namespace Utils{
    const unsigned long DEFAULT_BUFFER_SIZE = 1024;
    const int RECEIVER_BUFFER_SIZE = 65536;

    class Buffer{
    public:
        explicit Buffer(unsigned long size = DEFAULT_BUFFER_SIZE);
        ~Buffer();
        int readFromFd(int fd);
        int writeToFD(int fd);
        std::string getContent();
        void writeBuffer(char *str, unsigned long length);
        unsigned long getReadble();
        unsigned long getWriteable();
        char* readPoint();
        void setForward(unsigned long n);
        void clear();
        void swap(Buffer& buffer);
    private:
        std::vector<char> mMemory;
        unsigned long mReadIndex;
        unsigned long mWriteIndex;
        unsigned long mCapacity;

        void ensureSpace(unsigned long length);
    };
}

#endif //LIBEP_BUFFER_H
