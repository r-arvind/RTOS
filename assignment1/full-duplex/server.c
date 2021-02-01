
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

#define MAXLENGTH 100 
  

char sendBuffer[MAXLENGTH] = {0};
char recvBuffer[MAXLENGTH] = {0};
int socket_fd, connection_fd;

// Send the data to the server and set the timeout of 20 seconds
void *sendMsg()
{
    for(;;) {
    int stat;
    memset(sendBuffer, 0, sizeof(sendBuffer));
    // printf("Enter Message : ");
    fgets(sendBuffer, MAXLENGTH , stdin);
    stat = send(connection_fd, sendBuffer, strlen(sendBuffer), 0);
    }
}


//receive the data from the server
void *recvMsg(){
    for(;;) {
    int msg;
    memset(recvBuffer, 0, sizeof(recvBuffer));
    msg = recv(connection_fd, recvBuffer, MAXLENGTH, 0);
    printf("%s",recvBuffer);
    }
}

void terminate(int num){
    close(socket_fd);
    printf("\nClosing Socket. Exiting Application\n");
    exit(0);
}
  
// Driver function 
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
    if ((listen(socket_fd, 5)) != 0) { 
        printf("Listen failed\n Exiting.....\n"); 
        exit(0); 
    }
    printf("Server listening on Port %i\n",atoi(portno)); 


    len = sizeof(cli); 
    // Accept the data packet from client and verification 
    connection_fd = accept(socket_fd, (struct sockaddr*)&cli, &len); 
    if (connection_fd < 0) { 
        printf("Client connection failed\n"); 
        exit(0); 
    }
    printf("Client Successfully Connected\n\n");


    pthread_create(&read_thread, NULL, recvMsg, NULL);
    pthread_create(&write_thread, NULL, sendMsg, NULL);
    
    pthread_join(read_thread, NULL);
    pthread_join(write_thread, NULL);

} 

