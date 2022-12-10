#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>

int creat_login(char* username, char* port, char* adress){
    serv_addr.sin_addr.s_addr = inet_addr(adress);
    int nport = (int)port;
    serv_addr.sin_port = htons(port); //specify port
}