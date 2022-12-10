#ifndef CODE_SERVEUR_H
#define CODE_SERVEUR_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> /* close */
#include <netdb.h> /* gethostbyname */

#define PORT         4343
#define MAX_CLIENTS     10
#define BUFFERSIZE    1024

#define closesocket(s) close(s)
typedef int SOCKET;

#include "client.h"

// typedef struct node {
//     struct Client *client;
//     struct node *next;
// } *p_node;

// typedef struct list {
//     struct node *head;
// } *p_list;

void startServer(int port);
int initConnectionS(int port);
void endConnection(int sock);
int readClient(char *buffer, SOCKET sock);
void writeClient(const char *buffer, SOCKET sock);
void sendToAllClients(Client *clients, int nbClient, char *buffer);
void removeClient(Client *clients, Client thisClient, int* nbClient);
void clearAllSocket(Client *clients, int actual);

// p_node create_node(int client_id, char *client_name, p_node next);
// p_list create_list();
// int destroy_list(p_list list);
// int belongs(p_list list, p_client client);
// int add_element(p_list list, int client_id, char *client_name);
// int remove_element(p_list list, int client_id);
// int list_size(p_list list);
// void print_list(p_list list);
// p_node last_element(p_list list);

void message_from_client(fd_set myFD, Client* allClients, int nbClient, char* buffer);

#endif //CODE_SERVEUR_H