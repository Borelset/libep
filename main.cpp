#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include "epoll_process.h"

#define EPOLLSIZE 256
#define BUFFER_LENGTH 256

using namespace std;

int socket_init(int port)
{
    int listenfd;
    if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("error: socket ");
        return -1;
    }

    struct sockaddr_in serveraddr;
    memset(&serveraddr, 0, sizeof(sockaddr_in));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(port);
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(listenfd, (struct sockaddr*)&serveraddr, sizeof(sockaddr)) < -1)
    {
        perror("error: bind ");
        return -2;
    }

    return listenfd;
}

int main(int argc, char *argv[])
{
    int port = 1234;
    if(argv[1] != NULL)
    {
        port = atoi(argv[1]);
    }
    int sockfd = socket_init(port);
    listen(sockfd, 256);
    cout << "server is running at port: " << port << " ..." << endl;
    start_epoll(sockfd);
    close(sockfd);
    return 0;
}
