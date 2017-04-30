#ifndef EPOLL_PROCESS_H
#define EPOLL_PROCESS_H

struct process_arg
{
    int epollfd = -1;
    int event_fd = -1;
    process_arg(int _epollfd, int _event_fd)
    {
        epollfd = _epollfd;
        event_fd = _event_fd;
    }
};

void start_epoll(int listenfd);
void send_active(int epollfd, int fd, char* head, int pagefd);

#endif // EPOLL_PROCESS_H
