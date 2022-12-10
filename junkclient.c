




struct sockaddr_in serv_addr;
    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    struct hostent *server;
    char buffer[1024];
    server = gethostbyname(argv[1]);
    int n;

    char name[50];

    bzero((char *)&serv_addr, sizeof(serv_addr));
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);

    if (sock == -1) {
            printf("socket creation failed...\n");
            exit(0);
        } else {
            printf("Socket successfully created..\n");
        }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); //didn't initialise this in the video (it's OK if it's on the same PC, so no need to specify in the argument)
    if(argc==1)
        printf("No Extra Command Line Argument Passed other than program name\n");
    if(argc>=2){
        printf("Your Port will now be generated\n");
        int port = atoi(argv[1]); //use atoi(argv[1]) converts chars to int (doesn't give warning)
        serv_addr.sin_port = htons(port); //specify port !!!!NEVER USE ANY NUMBER BETWEEN 1 - 1024 BCS THESE ARE RESERVED SYSTEM PORTS
        char *name = argv[2];
    }

    printf("Enter your name: ");
    scanf("%s", name);
    
   if(connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == 0){
       printf("connection succesful.......\n");
   } else {
       printf("connection failed......");
       exit(0);
   }

    while(1){
        bzero(buffer, sizeof(buffer));
        fgets(buffer, sizeof(buffer), stdin);
        n = write(sock, buffer, strlen(buffer));
        if(n < 0){
            printf("write failed");
            exit(0);
        }
        bzero(buffer, sizeof(buffer));
        n = read(sock, buffer, sizeof(buffer));
        if(n < 0){
            printf("read failed");
            exit(0);
        }
        printf("Server : %s", buffer);
        int i = strncmp("Bye", buffer, 3);
        if(i==0)
            break;
    }
    close(sock);
    return 0;






    //Server side

int str_starts_with(char *string, char *prefix) {
    return strncmp(prefix, string, strlen(prefix)) == 0 ? 1 : 0;
}

p_client get_client_by_id(p_list clients, int id){
    p_node current = clients -> head;
    while(current != NULL){
        if(current-> client -> id == id)
            return current-> client;
        current = current-> next;
    }
    return NULL;
}

p_client get_client_by_name(p_list clients, char *name){
    p_node current = clients -> head;
    while(current != NULL){
        if(strcmp(current-> client -> name, name) == 0)
            return current-> client;
        current = current-> next;
    }
    return NULL;
}

char *socket_read(int socket_id){
    char c_size;
    h_reads(socket_id, &c_size, sizeof(char));

    int size = (int)c_size;

    char *message = malloc(size * sizeof(char));
    h_reads(socket_id, message, size);

    return message;
}

void socket_write(int socket_id, char *message){
    int size = strlen(message) + 1;
    char c_size = (char) size;

    h_writes(socket_id, &c_size, sizeof(char));
    h_writes(socket_id, message, size);
}

typedef struct listener_args{
    int *p_server_id;
    p_list clients;
    fd_set *p_set_safe;
}*p_listener_args;

void *run_listener(void *arg){
    p_listener_args args = (p_listener_args) arg;
    int server_id = *(args->p_server_id);
    while(*(args -> p_server_id) != -1){
        h_listen(server_id, 1);
        struct sockaddr_in *client_address;
        int client_id = h_accept(server_id, client_address);
        if(client_id != -1){
            socket_write(client_id, "hi");
            char *name = socket_read(client_id);
            add_element(args -> clients, client_id, name);
            FD_SET(client_id, args -> p_set_safe);
            printf("%s est connnecté (ID %d).\n", name, client_id);
        }
    }
    pthread_exit(NULL);
}

void serveur_execute(){

    printf("Starting server\n");
    Sock
    int server_id = h_socket(AF_INET, SOCK_STREAM);

    struct sockaddr_in *serveraddr;
    adr_socket(service, NULL, AF_INET, &serveraddr);

    h_bind(server_id, serveraddr);

    puts("Socket server initialised.");

    fd_set set, set_safe;
    FD_ZERO(&set);
    FD_ZERO(&set_safe);
    int maxsocks = getdtablesize();
    bcopy((char*)&set, (char*)&set_safe, sizeof(set));

    p_list clients = create_list();

    p_listener_args args = malloc(sizeof(struct listener_args));
    args -> p_server_id = &server_id;
    args -> clients = clients;
    args -> p_set_safe = &set_safe;
    pthread_t listener;
    pthread_create(&listener, NULL, run_listener, args);

    puts("En attente de connexions....");

    while (1) {
        p_node current = NULL;

        struct timeval *wait_time = malloc(sizeof(struct timeval));
        wait_time ->tv_sec = 0;
        wait_time ->tv_usec = 100;

        if(select(maxsocks, &set, 0, 0, wait_time) > 0)
            current = clients -> head;
        free(wait_time);
        while(current != NULL){
            int current_id = current->client->id;
            if(FD_ISSET(current_id,&set)){
                char *message = socket_read(current_id);
                if(strcmp(message, "quit") == 0){
                    socket_write(current_id, "bye");
                    h_close(current_id);
					printf("%s (ID %d) est déconnecté.\n", current -> client -> name, current -> client -> id);
					remove_element(clients, current_id);
					FD_CLR(current_id, &set_safe);
					free(message);
					break;
                } else if(strcmp(message, "list") == 0){
                    int size = 5;
                    p_node current = clients -> head;
                    while(current != NULL){
                        if(current -> client -> id != current_id)
                            size += strlen(current -> client -> name) + 1;
                        current = current -> next;
                    }
                    if(size == 5) size++;
                    char *response = malloc(size * sizeof(char));
                    response[0] = 'l';
                    response[1] = 'i';
                    response[2] = 's';
                    response[3] = 't';
                    response[4] = ':';
                    int i = 5;
                    current = clients -> head;
                    while(current != NULL){
                        if(current -> client -> id != current_id){
                            int current_size = strlen(current -> client-> name);
                            for(int n = 0; n<current_size; n++)
                                *(response + i++) = *((current -> client -> name) + n);
                            *(response + i++) = ';';
                        }
                        current = current -> next;
                    }
                    *(response + size - 1) = (char) 0;

                    socket_write(current_id, response);

                    free(response);
                } else if (str_starts_with(message, "msg:")){
                    *strstr(message, ">>") = (char) 0;
                    char *name = message + 4;

                    printf("\n\nReceived messaged for %s\n", name);

                    p_client recipient = get_client_by_name(clients, name);

                    if(recipient == NULL){
                        puts("Recipient is unknown");
                        char *err_msg = malloc((4 + strlen(name) + 1) * sizeof(char));
                        sprintf(err_msg, "rnf:%s", name);
                        socket_write(current_id, err_msg);
                        free(err_msg);
                    } else {
                        char *content = message + 4 + strlen(name) + 2,
                              *sender_name = get_client_by_id(clients, current_id) -> name;
                        int content_size = strlen(content),
                            sender_size = strlen(sender_name);
                        printf("Message is from %s: %s\n", sender_name, content);

                        int response_size = 4 + sender_size + 2 + content_size + 1;
                        char *response = malloc(response_size * sizeof(char));
                        sprintf(response, "msg:%s>>%s", sender_name, content);

                        socket_write(recipient -> id, response);

                        free(response);
                    }
                }
                else
                    socket_write(current_id, "err");
                free(message);
            }
            current = current -> next;
        }
        bcopy((char*) &set_safe, (char*) &set, sizeof(set_safe));
    }

    destroy_list(clients);

    h_close(server_id);

    server_id = -1;

    pthread_join(listener, NULL);
    free(args);

    puts("server closed.");
}
