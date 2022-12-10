#ifndef CODE_CLIENT_H
#define CODE_CLIENT_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> /* close */
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define closesocket(s) close(s)
typedef int SOCKET;

#define BUFFER_SIZE 1024 

typedef struct
{
    SOCKET sock;
    char name[BUFFER_SIZE];
}Client;

void start(int port);
int initConnection(char* address, int port);
void endConnection(int sock);
int readServer(char *buffer, SOCKET sock );
void writeServer(char *buffer, SOCKET sock );
void welcome(char *name);

int keyboard_input(char* buffer, SOCKET socket);
int message_from_server(char* buffer, SOCKET socket);

#endif //CODE_CLIENT_H