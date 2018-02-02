//
// Created by borelset on 2/2/18.
//

#include <fcntl.h>
#include <cstdio>
#include <sys/stat.h>
#include <cstring>
#include "ServerAddressParser.h"

std::vector<NetModule::SockAddr> SAParser(const char* path){
    int fd = open(path, O_RDONLY);
    if(fd < 0){
        perror("open");
        exit(0);
    }
    struct stat stat_buffer;
    fstat(fd, &stat_buffer);
    int fileSize = stat_buffer.st_size;
    if(fileSize > 1024){
        printf("error: list too large\n");
        exit(0);
    }

    char buffer[1024];
    memset(buffer, 0, sizeof buffer);
    read(fd, buffer, 1024);
    close(fd);

    int length = strlen(buffer);
    if(length==0){
        printf("error: no server provided\n");
        exit(0);
    }

    std::vector<NetModule::SockAddr> result;
    char tempIp[256];
    char tempPort[6];
    memset(tempIp, 0, sizeof tempIp);
    memset(tempPort, 0, sizeof tempPort);
    bool addrSplit = true;
    int ptr1 = 0;

    for(int i=0 ;i<length; i++){
        if(buffer[i] == '\n'
           || buffer[i] == ' '
           || i == length-1){
            memcpy(tempPort, &buffer[ptr1], i-ptr1+1);
            NetModule::SockAddr newAddr(tempIp, atoi(tempPort));
            result.push_back(newAddr);

            memset(tempIp, 0, sizeof tempIp);
            memset(tempPort, 0, sizeof tempPort);
            addrSplit = true;
            i++;
        }
        if(addrSplit){
            ptr1 = i;
            addrSplit = false;
        }
        if(buffer[i] == ':'){
            memcpy(tempIp, &buffer[ptr1], i-ptr1);
            ptr1 = i+1;
        }
    }

    return result;
}
