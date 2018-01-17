//
// Created by borelset on 1/17/18.
//

#include <sys/uio.h>
#include <iostream>
#include "Buffer.h"

Utils::Buffer::Buffer(unsigned long size):
        mMemory(size),
        mReadIndex(0),
        mWriteIndex(0),
        mCapacity(size)
{

}

Utils::Buffer::~Buffer() {

}

void Utils::Buffer::writeBuffer(char *str, unsigned long length) {
    std::cout << "Utils::Buffer::writeBuffer==>>"
              << "write buffer" << std::endl;
    if(length > getWriteable()){
        ensureSpace(length);
    }
    std::copy(str, str+length, mMemory.begin()+mWriteIndex);
    mWriteIndex += length;
}

unsigned long Utils::Buffer::getReadble() {
    return mWriteIndex - mReadIndex;
}

unsigned long Utils::Buffer::getWriteable() {
    return mMemory.capacity() - mWriteIndex;
}

void Utils::Buffer::ensureSpace(unsigned long length) {
    if(mCapacity > getWriteable() + length){
        std::cout << "Utils::Buffer::ensureSpace==>>"
                  << "Moving data" << std::endl;
        std::copy(mMemory.begin()+mReadIndex,
                  mMemory.begin()+mWriteIndex,
                  mMemory.begin());
        mWriteIndex -= mReadIndex;
        mReadIndex = 0;
    }else{
        std::cout << "Utils::Buffer::ensureSpace==>>"
                  << "Larger capacity:" << mWriteIndex+length << std::endl;
        mMemory.resize(mWriteIndex + length);
        mCapacity = mWriteIndex + length;
    }
}

void Utils::Buffer::clear() {
    mReadIndex = 0;
    mWriteIndex = 0;
}

int Utils::Buffer::readFromFd(int fd) {
    char tempbuffer[RECEIVER_BUFFER_SIZE];
    struct iovec receiver[2];
    receiver[0].iov_base = mMemory.data() + mWriteIndex;
    receiver[0].iov_len = getWriteable();
    receiver[1].iov_base = tempbuffer;
    receiver[1].iov_len = sizeof tempbuffer;

    ssize_t n = ::readv(fd, receiver, 2);
    std::cout << "Utils::Buffer::readFromFd==>>"
              << "read " << n << " character(s)" << std::endl;
    if(n<0){
        std::cout << "Utils::Buffer::readFromFd==>>"
                  << "readv error at fd: " << fd << std::endl;
    }else if(n < getWriteable()){
        mWriteIndex += n;
    }else{
        unsigned long length = static_cast<unsigned long>(n) - static_cast<unsigned long>(getWriteable());
        mWriteIndex = mMemory.size();
        writeBuffer(tempbuffer, length);
    }
    return static_cast<int>(n);
}

std::string Utils::Buffer::getContent() {
    std::cout << "Utils::Buffer::getContent==>>"
              << "read " << getReadble() << " characters" << std::endl;
    std::string result(mMemory.data()+mReadIndex, getReadble());
    mReadIndex += getReadble();
    return result;
}

char *Utils::Buffer::readPoint() {
    return mMemory.data() + mReadIndex;
}

void Utils::Buffer::setForward(unsigned long n) {
    mReadIndex += n;
}
