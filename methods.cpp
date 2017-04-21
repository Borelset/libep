#include <iostream>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <math.h>
#include "log.h"
#include "methods.h"
#include "route.h"
#include "epoll_process.h"


#define METHODS_TYPE_LENGTH 10
#define URL_LENGTH 256
#define CONTENT_LENGTH 1024
#define HEAD_LENGTH 1024

extern _log logout;

struct methodnode methodlist[] =
{
    {"GET",     method_get,           &methodlist[1]    },
    {"POST",    method_post,          &methodlist[2]    },
    {"HEAD",    method_unimplement,   &methodlist[3]    },
    {"OPTIONS", method_unimplement,   &methodlist[4]    },
    {"PUT",     method_unimplement,   &methodlist[5]    },
    {"DELETE",  method_unimplement,   &methodlist[6]    },
    {"TARCE",   method_unimplement,   &methodlist[7]    },
    {"",        method_wrongmethod,   nullptr           }
};

char methods_type[METHODS_TYPE_LENGTH];
char url[URL_LENGTH];
char content[CONTENT_LENGTH];

char* itoa(int num, char* addr)
{
    sprintf(addr, "%d", num);
    return addr;
}

int head_genarate(int epollfd, int fd, int length, int status, char* dest)
{
    const char head_http[]          =   "HTTP/1.1 ";
    const char head_status_200[]    =   "200 OK\r\n";
    const char head_status_400[]    =   "400 BAD REQUEST\r\n";
    const char head_status_404[]    =   "404 NOT FOUND\r\n";
    const char head_info[]          =   "Server: simple_httpserver 1.2\r\n"
                                        "Content-type: text/html\r\n";
    char head_length[30]            =   "Content-Length: ";
    char length_char[256];
    strcat(head_length, itoa(length, length_char));
    strcat(head_length, "\r\n");

    const char head_end[]           =   "\r\n";

    char buffer[1024];
    strcpy(buffer, head_http);
    if(status == 200)
    {
        strcat(buffer, head_status_200);
    }
    else if(status == 400)
    {
        strcat(buffer, head_status_400);
    }
    else if(status == 404)
    {
        strcat(buffer, head_status_404);
    }
    strcat(buffer, head_info);
    strcat(buffer, head_length);
    strcat(buffer, head_end);

    strcpy(dest, buffer);
    return 0;
}

int method_get(int epollfd, int fd, char* url, char* content)
{
    logout << "receive a GET method request with a url: " << url << logout;

    int pagefd = route(url);
    struct stat statbuf;
    fstat(pagefd, &statbuf);
    int file_length = statbuf.st_size;

    char head[HEAD_LENGTH];
    head_genarate(epollfd, fd, file_length, 200, head);

    send_active(epollfd, fd, head, pagefd);
    return 0;
}

int method_post(int epollfd, int fd, char* url, char* content)
{

}

int method_unimplement(int epollfd, int fd, char* url, char* content)
{
    char unimt_path[] = "/unimplement_method";
    logout << "receive an unimplement method request with a url: " << url << logout;

    int pagefd = route(unimt_path);
    struct stat statbuf;
    fstat(pagefd, &statbuf);
    int file_length = statbuf.st_size;

    char head[HEAD_LENGTH];
    head_genarate(epollfd, fd, file_length, 200, head);

    send_active(epollfd, fd, head, pagefd);

    return 0;
}

int method_wrongmethod(int epollfd, int fd, char* url, char* content)
{
    char wrong_path[] = "/wrong_method";
    logout << "receive an unknown method request with a url: " << url << logout;

    int pagefd = route(wrong_path);
    struct stat statbuf;
    fstat(pagefd, &statbuf);
    int file_length = statbuf.st_size;

    char head[HEAD_LENGTH];
    head_genarate(epollfd, fd, file_length, 400, head);

    send_active(epollfd, fd, head, pagefd);
    return 0;
}

int request_process(char* request, int epollfd, int fd)
{
    memset(methods_type, '\0', METHODS_TYPE_LENGTH);

    for(int i=0;i<METHODS_TYPE_LENGTH;i++)
    {
        if(*((char*)request + i) != ' ')
        {
            methods_type[i] = *((char*)request + i);
        }
    }
    int st_length = strlen(methods_type);

    memset(url, '\0', URL_LENGTH);
    for(int i=0;i<URL_LENGTH;i++)
    {
        if(*((char*)request + st_length + 1 + i) != ' ')
        {
            url[i] = *((char*)request + st_length + 1 + i);
        }
    }

    memset(content, '\0', CONTENT_LENGTH);

    method(epollfd, fd, methods_type, url, content);

    return 0;
}

int method(int epollfd, int fd, char* method_type, char* url, char* content)
{
    struct methodnode* p = methodlist;
    while(1)
    {
        if(!strcmp(p->name, method_type)||p->next == nullptr)
        {
            p->handle(epollfd, fd, url, content);
            return 0;
        }
        p = p->next;
    }
}
