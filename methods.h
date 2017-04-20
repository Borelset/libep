#ifndef METHODS_H
#define METHODS_H

int methodnode_list_init();
int method_get(int sock, char* url, char* content);
int method_post(int sock, char* url, char* content);
int method_unimplement(int sock, char* url, char* content);
int method_wrongmethod(int sock, char* url, char* content);
int method(char* method_type, int sock, char* url, char* content);

struct methodnode
{
    char name[10];
    int (*handle)(int, char*, char*);
    struct methodnode* next;
};

#endif // METHODS_H
