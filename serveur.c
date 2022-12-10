//#include "./client.c"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "serveur.h"
#include "fon.h"

// p_node create_node(int client_id, char *client_name, p_node next){
//     p_node node = malloc(sizeof(struct node));
//     p_client client = malloc(sizeof(struct client));

//     node -> client = client;
//     node -> next = next;

//     return node;
// }

// p_list create_list(){
//     p_list list = malloc(sizeof(struct list));
//     list -> head = NULL;
//     return list;
// }

// int destroy_list(p_list list){
//     if(list == NULL){
//         return 0;
//     }
//     p_node current= list -> head;
//     while(current != NULL){
//         p_node next= current -> next;
//         free(current -> client -> name);
//         free(current ->client);
//         //free(current -> client);
//         free(current);
//         current = next;
//     }
//     free(list);
//     return 1;
// }

// int belongs(p_list list, p_client client){
//     p_node current= list -> head;
//     while (current != NULL){
//         if(current -> client -> id == client -> id)
//             return 1;
//         current = current -> next;
//     }
//     return 0;
// }

// int add_element(p_list list, int client_id, char *client_name){
//     p_node current = list -> head;
//     if(list -> head == NULL){
//         list -> head = create_node(client_id, client_name, list -> head);
//         return 1;
//     }
//     while(current -> next != NULL){
//         if(current -> client -> id == client_id)
//             return 0;
//         current = current -> next;
//     }
//     //if(current -> client -> id == client_id) (*Tautology with the condition above*)
//     //    return 0;

//     current -> next = create_node(client_id, client_name, NULL);

//     return 1;
// }

// p_node last_element(p_list list){
//     p_node current = list -> head;
//     while(current -> next != NULL){
//         current = current -> next;
//     }
//     return current;
// }

// int remove_element(p_list list, int client_id){
//     p_node last = NULL;
//     p_node current = list -> head;
//     while(current != NULL) {
//         if(current -> client -> id == client_id){
//             if(last == NULL)
//                 list -> head = current -> next;
//             else
//                 last -> next = current -> next;
//             free(current -> client -> name);
//         //    free(current -> client -> id);
//             free(current -> client);
//             free(current);
//             return 1;
//         }
//         last = current;
//         current = current -> next;
//     }
//     return 0;
// }

// int list_size(p_list list){
//     int size = 0;
//     p_node current = list -> head;
//     while(current != NULL){
//         size++;
//         current = current -> next;
//     }
//     return size;
// }

// void print_list(p_list list){
//     p_node current = list -> head;
//     printf("[");
//     while(current != NULL){
//         printf("%d, %s ", current -> client -> id, current -> client -> name);
//         current = current -> next;
//     }
//     printf("]\n");
// }

void startServer(int port)
{
    printf("Starting server\n");
    SOCKET socketSer = initConnectionS(port);
    printf("Initialisation done\n");

    char buffer[BUFFERSIZE]; //buffer for message
    int nbClient = 0;    //index
    int maxFD = socketSer;

    /* let"s create an array to contain all the clients  see struct in client.h*/
    Client allClients[MAX_CLIENTS]; 

    /* let's create a File Descriptor */
    fd_set myFD; //

    while(1)
    {
        printf("in loop of server \n");
        int i = 0;
        //each time, need to empty the set
        FD_ZERO(&myFD);

        //the 2 next lines add what we want to control (keyboard + socket)
        /* add STDIN_FILENO */
        FD_SET(STDIN_FILENO, &myFD); //STDIN_FILENO is the file Descriptor for stdin
        /* add the socket */
        FD_SET(socketSer, &myFD);
        /* add socket of each client */
        for(i = 0; i < nbClient; i++)
        {
            FD_SET(allClients[i].sock, &myFD);
            // printf("Client %d = %s \t + \t  socket: %d\n", i, allClients[i].name, allClients[i].sock);
        }
        printf("Prepare to listen to select, socket = %d \n", maxFD);
        //we listen to the file Descriptor
        int returnSelect = select(maxFD + 1, &myFD, NULL, NULL, NULL);
        // printf("Select returned:  %d \n", returnSelect);
        if(returnSelect == -1)
        {
            perror("select()");
            exit(errno);
        }

        /* from keyboard */
        if(FD_ISSET(STDIN_FILENO, &myFD))
        {
            /* exit the app when using keyboard on server*/
            break;
        }
        else if(FD_ISSET(socketSer, &myFD)) //flag meaning that something happened from the socket
        {
            printf("New client recognised \n");
            /* new client */
            struct sockaddr_in csin = { 0 };
            size_t sinsize = sizeof csin;
            int csock = accept(socketSer, (struct sockaddr*)&csin, (socklen_t*)&sinsize); //We accept the connection
            if(csock == SOCKET_ERROR)
            {
                perror("accept()"); //in case of error
                continue;
            }

            /* receiving the client's pseudo */
            int receivingPseudo = readClient(buffer, csock);
            if(receivingPseudo == -1)
            {
                /* disconnected */
                continue;
            }

            /* calculate the new max file descriptor ? */
            if(maxFD < csock){
                maxFD = csock; //for select
            }

            //let's add the new file descriptor
            FD_SET(csock, &myFD);
            //we add the new Socket to the client list
            Client c = { csock };
            //the name in the buffer is put in c.name
            //char * strncpy( char * destination, const char * source, size_t length );
            strncpy(c.name, buffer, BUFFERSIZE - 1);
            //add to the client list of server
            allClients[nbClient] = c;
            nbClient++;
        }
        else // a client sent a message
        {
            message_from_client(myFD, allClients, nbClient, buffer);
        }
    }
    //if we leave the while loop,
    clearAllSocket(allClients, nbClient); //Stop all socket
    endConnection(socketSer); //close the socket
}


void message_from_client(fd_set myFD, Client* allClients, int nbClient, char* buffer) {
    Client thisClient;

    // store current client
    for (int i = 0; i < nbClient; i++) {
        if(FD_ISSET(allClients[i].sock, &myFD)) {
            Client thisClient = allClients[i];
        }
    }

    readClient(buffer, thisClient.sock);

    if (!strncmp(buffer, "list", 4)) { // client asked for client list
        sprintf(buffer, "%d", nbClient);

        writeClient(buffer, thisClient.sock);

        char tmpBuff[8];
        readClient(tmpBuff, thisClient.sock);

        for (int i = 0; i < nbClient; i++) {
            strcpy(buffer, allClients[i].name);
            writeClient(buffer, thisClient.sock);
            readClient(tmpBuff, thisClient.sock);
        }

    } else if (!strncmp(buffer, "quit", 4)) { //client has disconnected
        printf("client quiting\n");
        closesocket(thisClient.sock); //we close the socket
        removeClient(allClients, thisClient, &nbClient); //call the function the remove the client from the client List

        // we should be able to send to all clients since quiting client is no more
        strcpy(buffer, "server");
        sendToAllClients(allClients, nbClient, buffer);

        strcpy(buffer, thisClient.name);
        strcpy(buffer + strlen(thisClient.name), " has disconnected");
        sendToAllClients(allClients, nbClient, buffer);

    } else { // client sent a message

        // extract destination name
        int firstSpaceIndex = 0;
        for (int i = 0; i < strlen(buffer); i++) {
            if (buffer[i] == ' ') {
                firstSpaceIndex = i;
                break;
            }
        }

        char destName[BUFFERSIZE];
        strcpy(destName, buffer);
        destName[firstSpaceIndex] = 0;

        // delete destination from buffer
        int totalMessageLength = strlen(buffer);
        strcpy(buffer, buffer + firstSpaceIndex + 1); // shift message left to get rid of the name prefixe
        buffer[totalMessageLength - firstSpaceIndex] = 0;

        // find destination socket
        SOCKET destSocket = 0;
        for (int i = 0; i < nbClient; i++) {
            if (!strcmp(destName, allClients[i].name)) {
                destSocket = allClients[i].sock;
            }
        }

        if (destSocket != 0) {
            // send source name and message
            writeClient(thisClient.name, destSocket);

            char tmpBuff[8];
            readClient(tmpBuff, destSocket); // ping pong protocole a la TCP

            writeClient(buffer, destSocket);

        } else {
            printf("ERROR : unknown destination, ignoring message\n");
        }


    }
}

void clearAllSocket(Client *clients, int nbClient)
{
    printf("Clearing all socket\n");
    for(int i = 0; i < nbClient; i++)
    {
        closesocket(clients[i].sock); //for each client, close the socket
    }
}

void removeClient(Client *clients, Client thisClient, int* nbClient)
{
    int to_remove = 0;

    for (int i = 0; i < *nbClient; i++) {
        if (clients[i].sock == thisClient.sock) {
            to_remove = 1;
        }
    }
    /* we remove the client in the array */
    memmove(clients + to_remove, clients + to_remove + 1, (*nbClient - to_remove - 1) * sizeof(Client));
    /* one client is removed */
    (*nbClient)--;
}

void sendToAllClients(Client *clients, int nbClient, char *buffer)
{
    for(int i = 0; i < nbClient; i++)
    {
        writeClient(buffer, clients[i].sock); //send message to the client with index i
    }
}

int initConnectionS(int port)
{
    //a part is like client.c
    SOCKET socketServ = socket(AF_INET, SOCK_STREAM, 0); //create a new Socket
    struct sockaddr_in sin;

    if(socketServ == INVALID_SOCKET){
        perror("socket()");
        exit(errno);
    }

    sin.sin_addr.s_addr = htonl(INADDR_ANY);//link the socket with all the interface available
    sin.sin_port = htons(port); //set the port for the socket
    sin.sin_family = AF_INET; //type of connexion, not very usefull
    // printf("Conf result: adresse: %d \n port: %d \n", sin.sin_addr.s_addr,  sin.sin_port);

    int resultBind = bind(socketServ,(struct sockaddr_in*) &sin, sizeof sin);
    // printf("Bind done\n");
    if( resultBind == SOCKET_ERROR)
    {
        perror("bind()"); //error if the bind doesn't work
        exit(errno);
    }
    int resultListen = listen(socketServ, MAX_CLIENTS); //make the socket listenning
    if( resultListen == SOCKET_ERROR)
    {
        perror("listen()"); //Error if listen doesn't work
        exit(errno);
    }
    printf("Socket  = %d\n", socketServ);
    return socketServ;
}

/*
close the socked passed as parameter
*/
void endConnection(int sock)
{
    printf("Ending connection\n");
    closesocket(sock);
}

int readClient(char *buffer, SOCKET sock)
{
    // printf("Prepare to read from client\n");
    int n = recv(sock, buffer, BUFFERSIZE - 1, 0); //put in the buffer the message

    if( n < 0)
    {
        perror("recv()");
        n = 0;
    }
    // printf("read: ");
    // printf("%.*s",BUFFERSIZE - 1,buffer);
    // printf("\n");
    //like in client
    buffer[n] = 0;
    // printf("Read from client done\n");
    //empty the buffer:


    return n;
}

void writeClient(const char *buffer, SOCKET sock)
{
    // printf("Prepare to write to client\n");
    //like in client
    int n = send(sock, buffer, strlen(buffer), 0);
    if(n < 0)
    {
        perror("send()"); //in case of error
        exit(errno);
    }
    // printf("Write to client done\n");
}


int main(int argc, char *argv[]){

     if(argc < 1)
    {
        printf("arguments manquant : %s [port]\n", argv[0]);
        return EXIT_FAILURE;
    }
    //TODO test if arguments have the good type

    startServer(atoi(argv[1]));


    return EXIT_SUCCESS;
}