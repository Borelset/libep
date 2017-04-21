#ifndef EPOLL_PROCESS_H
#define EPOLL_PROCESS_H

void start_epoll(int listenfd);
void send_active(int epollfd, int fd, char* head, int pagefd);

#endif // EPOLL_PROCESS_H
