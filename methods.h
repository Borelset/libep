#ifndef METHODS_H
#define METHODS_H

char* itoa(int num, char* addr);
int method_get(int epollfd, int fd, char* url, char* content);
int method_post(int epollfd, int fd, char* url, char* content);
int method_unimplement(int epollfd, int fd, char* url, char* content);
int method_wrongmethod(int epollfd, int fd, char* url, char* content);
int method(int epollfd, int fd, char* method_type, char* url, char* content);
int request_process(char* request, int epollfd, int fd);

struct methodnode
{
    char name[10];
    int (*handle)(int, int, char*, char*);
    struct methodnode* next;
};

#endif // METHODS_H
