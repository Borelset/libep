#ifndef ROUTE_H
#define ROUTE_H

struct url_node
{
    char file[256] = {'\0'};
    char query_string[256] = {'\0'};
    char fragment[256] = {'\0'};
};

struct routenode
{
    char url[256];
    char path[256];
    struct routenode* next;
};

int open_file(char* path);
int route(char* url);

#endif // ROUTE_H
