#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <iostream>
#include "route.h"

#define SOURCE_PATH "./pages/"

struct routenode routelist[] =
{
    {"/",       "index.html",   &routelist[1]   },
    {"/index",  "index.html",   nullptr         }
};

int open_file(char* path)
{
    int pagefd = -1;
    pagefd = open(path, O_RDONLY);
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
    std::cout << "file: " << url_processed.file << " |query: " << url_processed.query_string << " |fragment: " << url_processed.fragment << std::endl;
    struct routenode* p = routelist;
    int pagefd = -1;
    while(p != nullptr)
    {
        if(!strcmp(p->url, url_processed.file))
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
