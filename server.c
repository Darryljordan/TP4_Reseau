//#include "./client.c"
#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>


int main(){

    struct sockaddr_in serveraddr, clientaddr;

    int sock = socket(AF_INET, SOCK_STREAM, 0);

    if(sock == -1){
        printf("socket creation error.....\n");
        exit(0);
    } else {
        printf("socket creation succesful....\n");
    }

    bzero(&serveraddr, sizeof(serveraddr));

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY); // specify in address
    serveraddr.sin_port = htons(8081); //specify port

    if((bind(sock, (struct sockaddr *)&serveraddr, sizeof(serveraddr))) != 0) {
        printf("socket bind unsuccessful....\n");
        exit(0);
    } else {
        printf("socket bind succesful....\n");
    }

    if((listen(sock,1)) == 0){
        printf("Listen succesful....");
    } else {
        printf("Listen unsuccessful....");
        exit(0);
    }
    int lenclient = sizeof(clientaddr);
    if (accept(sock, (struct sockaddr *)&clientaddr, &(lenclient)) < 0) {
        printf("Failed to accept..");
        exit(0);
    } else {
        printf("Accept successful");
    }

    close(sock);
}