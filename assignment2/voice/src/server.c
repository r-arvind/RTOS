
#include <stdio.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <signal.h>
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <unistd.h>  
#include <pthread.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include "message.h"
#include "audio.h"
#include <sys/time.h>


#define MAX_MEMBERS 10        //total clients that can be handled
// #define MAX_GROUPS_NUM 10           //max number of groups that can be created in the server
// #define MAX_GROUP_SIZE 100          //number of members in a group
#define MAILBOX_SIZE 1000               //size of message queue
#define POSTMAN_COUNT 3                 //Postman reads messages from the message queue and delivers it to it's respective user

// resource handlers
int members_socks[MAX_MEMBERS] = {0};
char members[MAX_MEMBERS][20] = {0};
// char groupNames[MAX_GROUPS_NUM][20];
// int connectionCount[MAX_GROUPS_NUM];
int memberCount = 0;
int socket_fd = 0;  
// int connection_fd = 0;


pthread_mutex_t memberRegistrationMutex;
pthread_t clientHandlerThreads[MAX_MEMBERS];

// message recvMessage;


void terminate(int num);
void *clientHandler(void *sock_fd);
void printMembers(void);


int main(int argc, char **argv){ 

    if(argc == 1){
        printf("Please supply a port number.\nExiting....\n");
        exit(0);
    }
    char *portno = argv[1];
    int sock_desc, conn_desc, len; 
    struct sockaddr_in servaddr, cli; 
    pthread_t read_thread, write_thread;
  
    // socket create and verification 
    socket_fd = socket(AF_INET, SOCK_STREAM, 0); 

    if (socket_fd == -1) { 
        printf("Unable to create socket !!\nExitting....\n"); 
        exit(0); 
    } 
    
    printf("Socket successfully created\n"); 
    signal(SIGINT, terminate);

    memset(&servaddr, 0, sizeof(servaddr));

    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    servaddr.sin_port = htons(atoi(portno)); 
  
    // Binding newly created socket to given IP and verification 
    if ((bind(socket_fd, (struct sockaddr*)&servaddr, sizeof(servaddr))) != 0) { 
        printf("Socket binding Failed\n"); 
        exit(0); 
    }    
    printf("Socket bound Succesfuly\n"); 
  
    // Now server is ready to listen and verification 
    if ((listen(socket_fd, 10)) != 0) { 
        printf("Listen failed\n Exiting.....\n"); 
        exit(0); 
    }
    printf("Server listening on Port %i\n\n",atoi(portno)); 


    len = sizeof(cli); 
    // Accept the data packet from client and verification 

    while(1){
        int *connection_fd = malloc(sizeof(int));
        *connection_fd = accept(socket_fd, (struct sockaddr*)&cli, &len); 
        printf("Member Count : %i\n",memberCount+1);
        if (*connection_fd < 0) { 
            printf("Client connection failed\n"); 
            exit(0); 
        }
        // printf("Client Successfully Connected\n");
        pthread_create(&clientHandlerThreads[memberCount], NULL, clientHandler ,(void *)connection_fd);
        // printMembers();
    }
    // pthread_create(&write_thread, NULL, sendMsg, NULL);    

} 

void *clientHandler(void *socket_fd){
        int client_fd = *(int *)socket_fd;
        struct Init reg; 
        recv(client_fd, &reg,sizeof(reg),0);
	printf("1--------------------\n"); 
       pthread_mutex_lock(&memberRegistrationMutex);
        if(memberCount >= MAX_MEMBERS){
            printf("Rejected Client %s. Maximum Capacity Reached.\n", reg.user_id);
            close(client_fd);
            pthread_cancel(clientHandlerThreads[memberCount]);
        }
        printf("%s Joined the Group!\n",reg.user_id);
        strcpy(members[memberCount], reg.user_id);
        printf("2--------------------\n");
	members_socks[memberCount++] = client_fd;
        pthread_mutex_unlock(&memberRegistrationMutex);
	printf("3--------------------\n");
        voice recvMessage;
        unsigned long t;	// uint8 buf[1024];
        while(recv(client_fd, &recvMessage,sizeof(recvMessage),0)) {
            t = (unsigned long)time(NULL);
		    if(recvMessage.msgtype == 0){
                printf("Group Message from %s\n %lu \n",recvMessage.name, t - recvMessage.timestamp);
                for(int j=0;j<memberCount;j++){
                    if(strcmp(members[j], recvMessage.name) != 0){
                        send(members_socks[j], &recvMessage, sizeof(recvMessage),0); 
                    }
                }
            } else {
                //printf("Direct Message from %s to %s\n",recvMessage.name,recvMessage.recipient_id);
                for(int j=0;j<memberCount;j++){
                    if(strcmp(members[j], recvMessage.recipient_id) == 0){
                        send(members_socks[j], &recvMessage, sizeof(recvMessage),0); 
                    }
                }
            }

        }
        
        printf("%s left the group\n",reg.user_id);
        pthread_mutex_lock(&memberRegistrationMutex);
        int exitedMember;
        for(int k=0;k<MAX_MEMBERS;k++){
            if(strcmp(reg.user_id, members[k]) == 0){
                exitedMember = k;
                break;
            }
        }
        for(exitedMember;exitedMember<memberCount;exitedMember++){
            members_socks[exitedMember] = members_socks[exitedMember + 1];
            strcpy(members[exitedMember],members[exitedMember + 1]);
        }
        memberCount--;
        pthread_mutex_unlock(&memberRegistrationMutex);
}

void terminate(int num){
    for(int i=0;i<memberCount;i++){
        pthread_kill(clientHandlerThreads[i],SIGKILL);
    }
    for(int i=0;i<memberCount;i++){
        close(members_socks[i]);
    }
    close(socket_fd);
    printf("\nClosing Socket. Exiting Application\n");
    exit(0);
}
  

void printMembers(void){
    pthread_mutex_lock(&memberRegistrationMutex);
    printf("Members : ");
    for(int k=0;k<memberCount+1;k++){
        printf("%s ",members[k]);
        fflush(stdout);
    }
    printf("\n");
    pthread_mutex_unlock(&memberRegistrationMutex);
}
