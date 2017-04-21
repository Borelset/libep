#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <iostream>
#include "route.h"
#include "log.h"

#define SOURCE_PATH "./source/"

extern _log logout;

struct routenode routelist[] =
{
    {"/",                   "index.html",           &routelist[1]   },
    {"/index",              "index.html",           &routelist[2]   },
    {"/unimplement_method", "unimplement.html",     &routelist[3]   },
    {"/wrong_method",       "wrong.html",           &routelist[4]   },
    {"",                    "not_found.html",       nullptr         }
};

int open_file(char* path)
{
    int pagefd = open(path, O_RDONLY);
    return pagefd;
}

struct url_node url_analysis(char* url)
{
    struct url_node result;

    char* p_file = strchr(url, '?');
    if(p_file == nullptr)
    {
        strcpy(result.file, url);
        return result;
    }
    int file_length = p_file - url;
    memcpy(result.file, url, file_length);
    result.file[file_length] = '\0';

    char* p_query = strchr(url, '#');
    if(p_query == nullptr)
    {
        strcpy(result.query_string, p_file+1);
        return result;
    }

    strcpy(result.query_string, p_file+1);
    return result;

    /*
    int query_length = p_query - p_file;
    memcpy(result.query_string, url+1, query_length);
    result.query_string[file_length+query_length] = '\0';

    strcpy(result.fragment, p_query+1);

    std::cout << "com url_a\n";
    return result;
    */
}

int route(char* url)
{
    struct url_node url_processed = url_analysis(url);
    logout << "file: " << url_processed.file << "\t|query: " << url_processed.query_string << "\t|fragment: " << url_processed.fragment << logout;
    struct routenode* p = routelist;
    int pagefd = -1;
    char path[256];
    while(1)
    {
        if(!strcmp(p->url, url_processed.file) || p->next == nullptr)
        {
            strcpy(path, SOURCE_PATH);
            strcat(path, p->path);
            pagefd = open_file(path);
            break;
        }
        p = p->next;
    }
    if(!strcmp(url_processed.file, "/"))
    {
        return pagefd;
    }

    int route_result = pagefd;
    strcpy(path, SOURCE_PATH);
    strcat(path, url_processed.file);
    pagefd = open_file(path);
    if(pagefd == -1)
    {
        return route_result;
    }
    else
    {
        return pagefd;
    }
}
