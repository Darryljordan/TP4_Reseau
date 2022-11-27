//#include "server.c"
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>

//#define struct sockaddr *addr;

//int Socket(){

//}

int main(){
    struct sockaddr_in serv_addr;
    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (sock == -1) {
            printf("socket creation failed...\n");
            exit(0);
        } else {
            printf("Socket successfully created..\n");
        }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8081); //beware of this 81
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

   if(connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == 0){
       printf("connection succesful.......");
   } else {
       printf("connection failed......");
       exit(0);
   }


    
}