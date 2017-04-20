#include <iostream>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <math.h>
#include "methods.h"
#include "route.h"

struct methodnode methodlist[] =
{
    {"GET",     method_get,           &methodlist[1]    },
    {"POST",    method_post,          &methodlist[2]    },
    {"HEAD",    method_unimplement,   &methodlist[3]    },
    {"OPTIONS", method_unimplement,   &methodlist[4]    },
    {"PUT",     method_unimplement,   &methodlist[5]    },
    {"DELETE",  method_unimplement,   &methodlist[6]    },
    {"TARCE",   method_unimplement,   nullptr           }
};

char* itoa(int num, char* addr)
{
    sprintf(addr, "%d", num);
    return addr;
}

int head_sender(int sock, int length, int status)
{
    const char head_http[]          =   "HTTP/1.1 ";
    const char head_status_200[]    =   "200 OK\r\n";
    const char head_status_400[]    =   "400 BAD REQUEST\r\n";
    const char head_status_404[]    =   "404 NOT FOUND\r\n";
    const char head_info[]          =   "Server: my httpserver 1.0\r\n"
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

    send(sock, buffer, strlen(buffer), 0);
    return 0;
}

int not_found(int sock)
{
    const char buffer[] = "<html>"
                          "<head><title>NOT FOUND</title></head>\r\n"
                          "<body><p>The page your applied is not found.</p></body>\r\n"
                          "</html>";
    int length = strlen(buffer);

    head_sender(sock, length, 404);
    send(sock, buffer, strlen(buffer), 0);
    return 0;
}

int method_get(int sock, char* url, char* content)
{
    std::cout << "receive a GET method request with a url: " << url << std::endl;

    int pagefd = route(url);
    if(pagefd < 0)
    {
        not_found(sock);
        return -1;
    }
    struct stat statbuf;
    fstat(pagefd, &statbuf);
    int file_length = statbuf.st_size;

    head_sender(sock, file_length, 200);

    char buffer[100];
    while(read(pagefd, buffer, 100) != 0)
    {
        send(sock, buffer, strlen(buffer), 0);
    }

    close(pagefd);
    return 0;
}

int method_post(int sock, char* url, char* content)
{

}

int method_unimplement(int sock, char* url, char* content)
{
    const char buffer[] = "<html>"
                          "<head><title>unimplement method</title></head>\r\n"
                          "<body><p>Method in your request has not been done in my server, sorry ..</p></body>\r\n"
                          "</html>";
    int length = strlen(buffer);

    head_sender(sock, length, 200);
    send(sock, buffer, strlen(buffer), 0);
    return 0;
}

int method_wrongmethod(int sock, char* url, char* content)
{
    const char buffer[] = "<html>"
                          "<head><title>bad request</title></head>\r\n"
                          "<body><p>Your browser sent a bad request, such as a POST without a Content-Length.</p></body>\r\n"
                          "</html>";
    int length = strlen(buffer);

    head_sender(sock, length, 400);
    send(sock, buffer, strlen(buffer), 0);
    return 0;
}

int method(char* method_type, int sock, char* url, char* content)
{
    struct methodnode* p = methodlist;
    while(p != NULL)
    {
        if(!strcmp(p->name, method_type))
        {
            p->handle(sock, url, content);
            return 0;
        }
        p = p->next;
    }
    method_wrongmethod(sock, url, content);
    return -1;
}
