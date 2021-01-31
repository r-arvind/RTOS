
#include <stdio.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <unistd.h>  


#define MAX 80 
#define PORT 8080 
  
// Function designed for chat between client and server. 
void func(int sock_desc) 
{ 
    char buff[MAX]; 
    int n; 
    // infinite loop for chat 
    for (;;) { 
        bzero(buff, MAX); 
  
        // read the message from client and copy it in buffer 
        read(sock_desc, buff, sizeof(buff)); 
        // print buffer which contains the client contents 
        printf("From client: %s\t To client : ", buff); 
        bzero(buff, MAX); 
        n = 0; 
        // copy server message in the buffer 
        while ((buff[n++] = getchar()) != '\n') 
            ; 
  
        // and send that buffer to client 
        write(sock_desc, buff, sizeof(buff)); 
  
        // if msg contains "Exit" then server exit and chat ended. 
        if (strncmp("exit", buff, 4) == 0) { 
            printf("Server Exit...\n"); 
            break; 
        } 
    } 
} 
  
// Driver function 
int main(int argc, char **argv) 
{ 

    if(argc == 1){
        printf("Please supply a port number.\nExiting....\n");
        exit(0);
    }
    char *portno = argv[1];
    int sock_desc, conn_desc, len; 
    struct sockaddr_in servaddr, cli; 
  
    // socket create and verification 
    sock_desc = socket(AF_INET, SOCK_STREAM, 0); 


    if (sock_desc == -1) { 
        printf("Unable to create socket !!\nExitting....\n"); 
        exit(0); 
    } else {
        printf("Socket successfully created\n"); 
    }


    memset(&servaddr, 0, sizeof(servaddr));

    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    servaddr.sin_port = htons(atoi(portno)); 
  
    // Binding newly created socket to given IP and verification 
    if ((bind(sock_desc, (struct sockaddr*)&servaddr, sizeof(servaddr))) != 0) { 
        printf("Socket binding Failed\n"); 
        exit(0); 
    } else {
        printf("Socket bound Succesfuly\n"); 
    }
  
    // Now server is ready to listen and verification 
    if ((listen(sock_desc, 5)) != 0) { 
        printf("Listen failed\n Exiting.....\n"); 
        exit(0); 
    } else {
        printf("Server listening on Port %i\n",atoi(portno)); 
    }

    len = sizeof(cli); 
    // Accept the data packet from client and verification 
    conn_desc = accept(sock_desc, (struct sockaddr*)&cli, &len); 
    if (conn_desc < 0) { 
        printf("Client connection failed\n"); 
        exit(0); 
    } else {
        // printf("New client with IP %s Connected\n",inet_ntoa(cli.sin_port)); 

    }
  
    // Function for chatting between client and server 
    func(conn_desc); 
  
    // After chatting close the socket 
    close(sock_desc); 
} 
