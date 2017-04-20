#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include "methods.h"

#define MAX_URL_LENGTH 256

using namespace std;

void buffer_init(char* buffer, int length)
{
    for(int i=0;i<length;i++)
    {
        buffer[i] = '\0';
    }
}

int startup(int port)
{
    int sockfd;
    struct sockaddr_in serveraddr;
    if( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        cerr << "error: create socket\n";
        return -1;
    }

    memset(&serveraddr, 0, sizeof(sockaddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(port);
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if( (bind(sockfd, (struct sockaddr*)&serveraddr, sizeof(sockaddr))) < 0)
    {
        cerr << "error: bind socket\n";
        return -2;
    }
    if( listen(sockfd, 20) < 0)
    {
        cerr << "error: listen socket\n";
        return -3;
    }
    return sockfd;
}

int request_process(int sock)
{
    char request[MAX_URL_LENGTH];
    recv(sock, &request, MAX_URL_LENGTH, 0);

    char service_type[10];
    buffer_init(service_type, 10);

    for(int i=0;i<10;i++)
    {
        if(*((char*)request + i) != ' ')
        {
            service_type[i] = *((char*)request + i);
        }
    }
    int st_length = strlen(service_type);

    char url[MAX_URL_LENGTH];
    buffer_init(url, MAX_URL_LENGTH);
    for(int i=0;i<MAX_URL_LENGTH;i++)
    {
        if(*((char*)request + st_length + 1 + i) != ' ')
        {
            url[i] = *((char*)request + st_length + 1 + i);
        }
    }

    char content[256];

    method(service_type, sock, url, content);

    return 0;
}

int main(int argc, char *argv[])
{
    int serversock = -1, acceptsock = -1;
    int port;
    struct sockaddr_in acceptaddr;
    socklen_t sockaddr_size = sizeof(acceptaddr);
    if(argv[1] != NULL)
    {
        port = atoi(argv[1]);
    }
    else
    {
        port = 1234;
    }

    if(port<1024 || port>65535)
    {
        cerr << "error: wrong port provided\n";
        return -1;
    }

    if( (serversock = startup(port)) < 0)
    {
        cerr << "error: fail in startup\n";
        return -2;
    }
    cout << "server is running at port: " << port << " ..." << endl << endl;

    while(1)
    {
        acceptsock = accept(serversock, (struct sockaddr*)&acceptaddr, &sockaddr_size);
        if(acceptsock == -1)
        {
            cerr << "error: fail in accept\n";
            close(serversock);
            return -3;
        }
        cout << "accept a connect from "<< inet_ntoa(acceptaddr.sin_addr) << ":" << acceptaddr.sin_port << endl;
        request_process(acceptsock);
    }
    close(serversock);
    return 0;
}
