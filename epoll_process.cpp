#include "epoll_process.h"
#include "methods.h"
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <iostream>

#define EPOLLSIZE 256
#define BUFFER_LENGTH 1024
#define FD_SIZE 1024

using namespace std;

int filefd = -1;
char buf[BUFFER_LENGTH];

void add_event(int epollfd, int fd, int state)
{
    struct epoll_event ev;
    ev.events = state;
    ev.data.fd = fd;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
}

void delete_event(int epollfd, int fd, int state)
{
    struct epoll_event ev;
    ev.events = state;
    ev.data.fd = fd;
    epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, &ev);
}

void modify_event(int epollfd, int fd, int state)
{
    struct epoll_event ev;
    ev.events = state;
    ev.data.fd = fd;
    epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &ev);
}

void accept_process(int epollfd, int listenfd)
{
    int clifd;
    struct sockaddr_in cliaddr;
    socklen_t sockaddr_length = sizeof(sockaddr);
    clifd = accept(listenfd, (struct sockaddr*)&cliaddr, &sockaddr_length);
    if(clifd == -1)
    {
        perror("error: accept ");
    }
    else
    {
        cout << "accept a connection from: " << inet_ntoa(cliaddr.sin_addr) << ":" << cliaddr.sin_port << endl;
        add_event(epollfd, clifd, EPOLLIN);
    }
}

void recv_process(int epollfd, int fd)
{
    int read_flag = recv(fd, buf, BUFFER_LENGTH, 0);
    if(read_flag == -1)
    {
        perror("error: recv ");
        close(fd);
        delete_event(epollfd, fd, EPOLLIN);
    }
    else if(read_flag == 0)
    {
        close(fd);
        delete_event(epollfd, fd, EPOLLIN);
    }
    else
    {
        request_process(buf, epollfd, fd);
    }
}

void send_active(int epollfd, int fd, char* head, int pagefd)
{
    strcpy(buf, head);
    filefd = pagefd;
    modify_event(epollfd, fd, EPOLLOUT);
}

void send_process(int epollfd, int fd)
{
    int send_flag = send(fd, buf, strlen(buf), 0);
    if(send_flag == -1)
    {
        perror("error: send head ");
        close(fd);
        close(filefd);
        filefd = -1;
        delete_event(epollfd, fd, EPOLLOUT);
        return;
    }

    struct stat statbuf;
    fstat(filefd, &statbuf);
    int file_length = statbuf.st_size;
    int send_file_flag = sendfile(fd, filefd, NULL, file_length);
    if(send_file_flag == -1)
    {
        perror("error: sendfile ");
        close(fd);
        close(filefd);
        filefd = -1;
        delete_event(epollfd, fd, EPOLLOUT);
        return;
    }

    close(filefd);
    filefd = -1;
    modify_event(epollfd, fd, EPOLLIN);
    memset(buf, '\0', BUFFER_LENGTH);
}

void epoll_process(int epollfd, struct epoll_event* event, int num, int listenfd)
{
    int event_fd = -1;
    for(int i=0; i<num; i++)
    {
        event_fd = event[i].data.fd;
        if((event_fd == listenfd) && (event[i].events & EPOLLIN))
        {
            accept_process(epollfd, listenfd);
        }
        else if(event[i].events & EPOLLIN)
        {
            recv_process(epollfd, event_fd);
        }
        else if(event[i].events & EPOLLOUT)
        {
            send_process(epollfd, event_fd);
        }
    }
}

void start_epoll(int listenfd)
{
    int epollfd;
    struct epoll_event events[EPOLLSIZE];
    int ret = -1;
    memset(buf, '\0', BUFFER_LENGTH);

    epollfd = epoll_create(FD_SIZE);
    add_event(epollfd, listenfd, EPOLLIN);

    while(1)
    {
        ret = epoll_wait(epollfd, events, EPOLLSIZE, -1);
        epoll_process(epollfd, events, ret, listenfd);
    }

    close(epollfd);
}
