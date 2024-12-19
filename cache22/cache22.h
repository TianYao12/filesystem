#ifndef CACHE22
#define CACHE22

#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <errno.h>
#include <stddef.h>
#include <stdarg.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define HOST "127.0.0.1"
#define PORT "12345"
#define log(x, ...) printf(x, __VA_ARGS__)


typedef unsigned int int32;
typedef unsigned short int int16;
typedef unsigned char int8;

struct s_client {
    int s;
    char ip[16];
    int16 port;
};

typedef struct s_client Client;
typedef int32 (*Callback)(Client*, int8*, int8*);

struct s_commandHandler {
    int8* cmd;
    Callback handler;
};
typedef struct s_commandHandler CommandHandler;
int32 handleHello(Client*, int8*, int8*);
void zero(int8*, int16);
void childLoop(Client *);
void mainLoop(int);
int initServer(int16);
int main(int, char**);

#endif