/*
Jamile Lima Leite
Kemgne Nasah Darryl
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "client.h"
#include "fon.h"



void start(int port);

void welcome(char *name){
    printf("\n\n\nWelcome dear user: %s !\n\n\n", name);
    puts("TO SEND A MESSAGE, ENTER THE NAME OF THE RECEIVER FOLLOWED BY THE MESSAGE");
    puts("TO SHOW THE LIST OF CONNECTED USERS, WRITE: list");
    puts("TO QUIT, WRITE: quit");
    puts("FINALLY TO SHOW THIS AGAIN, WRITE: help\n\n");
}



void start(int port){
    printf("starting client\n");
    
    SOCKET socket = initConnection("127.0.0.1", port);
    printf("Client initialisation done\n");
    //message buffer
    char buffer[BUFFER_SIZE];

    //creating the file descriptor
    fd_set myFD;
    char name[200];
    printf("Please input your name\n");
    scanf("%s", name);
    printf("Sending client name to server");
    writeServer(name, socket);
    welcome(name);

    while (1) {
        //always empty the set
        FD_ZERO(&myFD);

        //adds what we want to control (keyboard + socket)
        FD_SET(STDIN_FILENO, &myFD); //STDIN_FILENO is the file Descriptor for stdin
        //add the socket
        FD_SET(socket, &myFD); 

        printf("listening on select \n");
        //we listen to the file descriptor
        int returnSelect = select(socket + 1, &myFD, NULL, NULL, NULL);
        // printf("select listenned, returned: %d\n", returnSelect);
        if( returnSelect == -1)
        { //on a une erreur dans le select si on a un -1
            printf("select()\n");
            exit(errno);
        }

        /* from keyboard */
        if(FD_ISSET(STDIN_FILENO, &myFD)) //flag meaning that something happened from the keyboard
        {

            if (!keyboard_input(buffer, socket)) {
                break;
            }

        }
        else if(FD_ISSET(socket, &myFD)) //flag meaning that something happened from the socket
        {
            if (!message_from_server(buffer, socket)) {
                break;
            }
        }
    }
    endConnection(socket);
}

int keyboard_input(char* buffer, SOCKET socket) {
    fgets(buffer, BUFFER_SIZE - 1, stdin); //read on stdin
    // following code puts \0 at the end of the input
    char *pt_buffer = NULL;
    pt_buffer = strstr(buffer, "\n");
    if(pt_buffer != NULL) {
        *pt_buffer = 0;
    } else {
        buffer[BUFFER_SIZE - 1] = 0;
    }

    if (buffer[0] != 0) {
        if (strncmp("list", buffer, 4) == 0) { // user wants to see the list of clients
            writeServer(buffer, socket);

            readServer(buffer, socket);
            writeServer(buffer, socket);
            int nbClient = atoi(buffer);

            printf("List of connected clients : \n");
            for (int i = 0; i < nbClient; i++) {
                readServer(buffer, socket);
                writeServer(buffer, socket);

                printf("\t %s \n", buffer);
            }

            return 1;

        } else if (strncmp("quit", buffer, 4) == 0) { // user wants to quit the connection
            printf("quiting\n");
            writeServer(buffer, socket);

            return 0;

        } else { // user is sending a message
            writeServer(buffer, socket);

            return 1;
        }
    } else {
        printf("ERROR : please use correct input \n");
        return 1;
    }

}

int message_from_server(char* buffer, SOCKET socket) {
    int n = readServer(buffer, socket); // reading the senders name
    writeServer(buffer, socket);  // ping pong protocole a la TCP
    if(n == 0) //nothing to read from the server
    {
        printf("Nothing to read !\n");
        return 0; // return false
    }

    printf("Message from %s :\n", buffer);

    n = readServer(buffer, socket); // reading the senders message
    if(n == 0) //nothing to read from the server
    {
        printf("Nothing to read !\n");
        return 0; // return false
    }

    printf("%s\n", buffer);

    return 1; // return true
}

int initConnection(char* address, int port)
{
    SOCKET socketClient = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in sin;
    socklen_t taille = sizeof(struct sockaddr);
    struct hostent *hostinfo;

    if(socketClient == INVALID_SOCKET)
    {
        printf("socket()\n");
        exit(errno);
    }

    sin.sin_addr.s_addr = inet_addr(address); //set the adress ip, now hostname are not available
    sin.sin_port = htons(port); //set the port for the socket
    sin.sin_family = AF_INET; //type of connexion, not very usefull
    // printf("Conf result: adresse: %d \n port: %d \n", sin.sin_addr.s_addr,  sin.sin_port);

    int connectionResult = connect(socketClient,(struct sockaddr*) &sin, taille); //try to connect
    if( connectionResult == SOCKET_ERROR)
    {
        printf("connect()\n");
        exit(errno);
    }

    return socketClient; //if connexion is successfull, return the socket
}


void endConnection(int socket)
{
    closesocket(socket); //to close the connection with the socket gave as a parameter
}

int readServer(char *buffer, SOCKET socket)
{
    // printf("Read from server: \n");
    int nbByteRead = 0;
    nbByteRead = recv(socket, buffer, BUFFER_SIZE - 1, 0); //store in the buffer what's received and return the number of byte read
    //also store in the buffer the result
    if(nbByteRead < 0)
    {
        perror("recv()");
        exit(errno);
    }

    buffer[nbByteRead] = 0;

    return nbByteRead;
}

void writeServer(char *buffer, SOCKET socket)
{
    // printf("Write to server:\n ");
    // printf("buffer to Send: %.*s\n",BUFFER_SIZE - 1,buffer);
    int nbByteSent = send(socket, buffer, strlen(buffer), 0); //send what is stored in the buffer
    // printf("NBByte sent: %d\n", nbByteSent);
    if( nbByteSent < 0)
    {
        perror("send()");
        exit(errno);
    }

}



int main(int argc, char **argv)
{
    if(argc < 1)
    {
        printf("arguments manquant : %s [port]\n", argv[0]);
        return EXIT_FAILURE;
    }
    //TODO test if arguments have the good type


    start(atoi(argv[1]));


    return EXIT_SUCCESS;
}



