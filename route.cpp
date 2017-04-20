#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <iostream>
#include "route.h"

#define SOURCE_PATH "/home/borelset/server_source/"

struct routenode routelist[] =
{
    {"/",  "index.html",  &routelist[1]},
    {"/index",  "index.html",  nullptr}
};

int open_file(char* path)
{
    int pagefd = -1;
    pagefd = open(path, O_RDONLY);
    return pagefd;
}

int route(char* url)
{
    struct routenode* p = routelist;
    int pagefd = -1;
    while(p != nullptr)
    {
        if(!strcmp(p->url, url))
        {
            char path[256];
            strcpy(path, SOURCE_PATH);
            strcat(path, p->path);
            pagefd = open_file(path);
            return pagefd;
        }
        p = p->next;
    }
    return pagefd;
}
