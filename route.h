#ifndef ROUTE_H
#define ROUTE_H

struct routenode
{
    char url[256];
    char path[256];
    struct routenode* next;
};

int route(char* url);

#endif // ROUTE_H
