#ifndef METHODS_H
#define METHODS_H

int methodnode_list_init();
int method_get(int sock, char* url);
int method_post(int sock, char* url);
int method_unimplement(int sock, char* url);
int method_wrongmethod(int sock, char* url);
int method(char* method_type, int sock, char* url);

struct methodnode
{
    char name[10];
    int (*handle)(int, char*);
    struct methodnode* next;
};

#endif // METHODS_H
