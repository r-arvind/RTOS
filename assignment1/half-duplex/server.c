
#include <stdio.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <signal.h>
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <unistd.h>  


#define MAXLENGTH 100 
  

char sendBuffer[MAXLENGTH] = {0};
char recvBuffer[MAXLENGTH] = {0};
int socket_fd = 0;

// Send the data to the server and set the timeout of 20 seconds
void sendMsg(int serv_sock)
{
    int stat;
    memset(sendBuffer, 0, sizeof(sendBuffer));
    printf("Enter Message : ");
    fgets(sendBuffer, MAXLENGTH , stdin);
    sendBuffer[strlen(sendBuffer) - 1] = '\0';
    stat = send(serv_sock, sendBuffer, strlen(sendBuffer), 0);
}


//receive the data from the server
void recvMsg(int serv_sock){
    // char reply[MAXLENGTH] = {0};
    int msg;
    memset(recvBuffer, 0, sizeof(recvBuffer));
    msg = recv(serv_sock, recvBuffer, MAXLENGTH, 0);
    printf("Response from client: %s\n",recvBuffer);
}

// Function designed for chat between client and server. 
void chat(int sock_desc){ 
    int n; 
    // infinite loop for chat 
    for (;;) { 
        recvMsg(sock_desc);
        sendMsg(sock_desc);
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
  
    // socket create and verification 
    sock_desc = socket(AF_INET, SOCK_STREAM, 0); 
    socket_fd = sock_desc;

    if (sock_desc == -1) { 
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
    if ((bind(sock_desc, (struct sockaddr*)&servaddr, sizeof(servaddr))) != 0) { 
        printf("Socket binding Failed\n"); 
        exit(0); 
    }    
    printf("Socket bound Succesfuly\n"); 
  
    // Now server is ready to listen and verification 
    if ((listen(sock_desc, 5)) != 0) { 
        printf("Listen failed\n Exiting.....\n"); 
        exit(0); 
    }
    printf("Server listening on Port %i\n",atoi(portno)); 


    len = sizeof(cli); 
    // Accept the data packet from client and verification 
    conn_desc = accept(sock_desc, (struct sockaddr*)&cli, &len); 
    if (conn_desc < 0) { 
        printf("Client connection failed\n"); 
        exit(0); 
    }
    printf("Client Successfully Connected\n\n");
    // Function for chatting between client and server 
    chat(conn_desc); 
  
    // After chatting close the socket 
    close(sock_desc); 
} 

