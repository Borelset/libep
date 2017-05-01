#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include "epoll_process.h"
#include "methods.h"
#include "log.h"
#include "thread_pool.h"

#define EPOLLSIZE 256
#define BUFFER_LENGTH 1024
#define FD_SIZE 1024

using namespace std;

int filefd = -1;
char buf[BUFFER_LENGTH];
extern _log logout;
thread_pool th_p;

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

void* accept_process(void* arg)
{
    struct process_arg* ptr = (struct process_arg*)arg;

    int clifd;
    struct sockaddr_in cliaddr;
    socklen_t sockaddr_length = sizeof(sockaddr);
    clifd = accept(ptr->event_fd, (struct sockaddr*)&cliaddr, &sockaddr_length);
    if(clifd == -1)
    {
        logout << "accept: " << strerror(errno) << logout;
    }
    else
    {
        char logout_buffer[10];
        logout << "accept a connection from: " << inet_ntoa(cliaddr.sin_addr) << ":" << itoa(cliaddr.sin_port, logout_buffer) << logout;
        add_event(ptr->epollfd, clifd, EPOLLIN | EPOLLET);
    }
    delete ptr;
}


void* recv_process(void* arg)
{
    struct process_arg* ptr = (struct process_arg*)arg;

    int read_flag = recv(ptr->event_fd, buf, BUFFER_LENGTH, 0);
    if(read_flag == -1)
    {
        logout << "recv: " << strerror(errno) << logout;
        close(ptr->event_fd);
        delete_event(ptr->epollfd, ptr->event_fd, EPOLLIN | EPOLLET);
    }
    else if(read_flag == 0)
    {
        close(ptr->event_fd);
        delete_event(ptr->epollfd, ptr->event_fd, EPOLLIN | EPOLLET);
    }
    else
    {
        request_process(buf, ptr->epollfd, ptr->event_fd);
    }
    delete ptr;
}

void send_active(int epollfd, int fd, char* head, int pagefd)
{
    strcpy(buf, head);
    filefd = pagefd;
    modify_event(epollfd, fd, EPOLLOUT | EPOLLET);
}

void* send_process(void* arg)
{
    struct process_arg* ptr = (struct process_arg*)arg;

    int send_flag = send(ptr->event_fd, buf, strlen(buf), 0);
    if(send_flag == -1)
    {
        logout << "send head: " << strerror(errno) << logout;
        close(ptr->event_fd);
        close(filefd);
        filefd = -1;
        delete_event(ptr->epollfd, ptr->event_fd, EPOLLOUT | EPOLLET);
        delete ptr;
        return nullptr;
    }

    struct stat statbuf;
    fstat(filefd, &statbuf);
    int file_length = statbuf.st_size;
    int send_file_flag = sendfile(ptr->event_fd, filefd, NULL, file_length);
    if(send_file_flag == -1)
    {
        logout << "sendfile: " << strerror(errno) << logout;
        close(ptr->event_fd);
        close(filefd);
        filefd = -1;
        delete_event(ptr->epollfd, ptr->event_fd, EPOLLOUT | EPOLLET);
        delete ptr;
        return nullptr;
    }

    logout << "send done" << logout;
    close(filefd);
    filefd = -1;
    modify_event(ptr->epollfd, ptr->event_fd, EPOLLIN | EPOLLET);
    memset(buf, '\0', BUFFER_LENGTH);
    delete ptr;
    return nullptr;
}

void epoll_process(int epollfd, struct epoll_event* event, int num, int listenfd)
{
    int event_fd = -1;
    for(int i=0; i<num; i++)
    {
        event_fd = event[i].data.fd;
        struct process_arg* arg = new struct process_arg(epollfd, event_fd);
        if((event_fd == listenfd) && (event[i].events & EPOLLIN))
        {
            th_p.run(accept_process, (void*)arg);
        }
        else if(event[i].events & EPOLLIN)
        {
            th_p.run(recv_process, (void*)arg);
        }
        else if(event[i].events & EPOLLOUT)
        {
            th_p.run(send_process, (void*)arg);
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
    add_event(epollfd, listenfd, EPOLLIN | EPOLLET);

    while(1)
    {
        ret = epoll_wait(epollfd, events, EPOLLSIZE, -1);
        epoll_process(epollfd, events, ret, listenfd);
    }

    close(epollfd);
}
