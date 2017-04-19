#include <iostream>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include "methods.h"

#define source_path "/home/borelset/server_source"

struct methodnode methodlist[] =
{
    {"GET",     method_get,           &methodlist[1]},
    {"POST",    method_post,          &methodlist[2]},
    {"HEAD",    method_unimplement,   &methodlist[3]},
    {"OPTIONS", method_unimplement,   &methodlist[4]},
    {"PUT",     method_unimplement,   &methodlist[5]},
    {"DELETE",  method_unimplement,   &methodlist[6]},
    {"TARCE",   method_unimplement,   nullptr       }
};

int open_file(char* path)
{
    int pagefd = -1;
    pagefd = open(path, O_RDONLY);
    return pagefd;
}

int not_found(int sock)
{
    const char* buffer =  "HTTP/1.1 404 NOT FOUND\r\n"
                          "Server: my httpserver 1.0\r\n"
                          "Content-type: text/html\r\n"
                          "\r\n"
                          "<html>"
                          "<head><title>NOT FOUND</title></head>\r\n"
                          "<body><p>The page your applied is not found.</p></body>\r\n"
                          "</html>";
    send(sock, buffer, strlen(buffer), 0);
    return 0;
}

int method_get(int sock, char* url)
{
    std::cout << "this is GET method with a url: " << url << std::endl;
    char buffer[100] =  "HTTP/1.1 200 OK\r\n"
                        "Server: my httpserver 1.0\r\n"
                        "Content-type: text/html\r\n"
                        "\r\n";
    send(sock, buffer, strlen(buffer), 0);

    char path[100];
    strcpy(path, source_path);
    strcat(path, url);

    int pagefd = open_file(path);
    if(pagefd < 0)
    {
        not_found(sock);
        return -1;
    }

    while(read(pagefd, buffer, 100) != 0)
    {
        send(sock, buffer, strlen(buffer), 0);
    }

    close(pagefd);
    return 0;
}

int method_post(int sock, char* url)
{

}

int method_unimplement(int sock, char* url)
{

}

int method_wrongmethod(int sock, char* url)
{
    const char* buffer =  "HTTP/1.1 400 BAD REQUEST\r\n"
                          "Server: my httpserver 1.0\r\n"
                          "Content-type: text/html\r\n"
                          "\r\n"
                          "<html>"
                          "<head><title>bad request</title></head>\r\n"
                          "<body><p>Your browser sent a bad request, such as a POST without a Content-Length.</p></body>\r\n"
                          "</html>";
    send(sock, buffer, strlen(buffer), 0);
    return 0;
}

int method(char* method_type, int sock, char* url)
{
    struct methodnode* p = methodlist;
    while(p != NULL)
    {
        if(!strcmp(p->name, method_type))
        {
            p->handle(sock, url);
            return 0;
        }
        p = p->next;
    }
    method_wrongmethod(sock, url);
    return -1;
}
