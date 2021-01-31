#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include <signal.h>


#define MAXLENGTH 100

char sendBuffer[MAXLENGTH] = {0};
char recvBuffer[MAXLENGTH] = {0};
int socket_fd;


// Send the data to the server and set the timeout of 20 seconds
void sendMsg(int serv_sock)
{
    int stat;
    memset(sendBuffer, 0, sizeof(sendBuffer));
    printf("Your Message : ");
    // gets(sendBuffer);
    fgets(sendBuffer, MAXLENGTH , stdin);
    sendBuffer[strlen(sendBuffer) - 1] = '\0';
    stat = send(serv_sock, sendBuffer, strlen(sendBuffer), 0);
}


//receive the data from the server
void recvMsg(int serv_sock)
{
    // char reply[MAXLENGTH] = {0};
    int msg;
    memset(recvBuffer, 0, sizeof(recvBuffer));
    msg = recv(serv_sock, recvBuffer, MAXLENGTH, 0);
    printf("Response from server : %s\n" ,recvBuffer);
}


// Function designed for chat between client and server. 
void chat(int sock_desc) 
{ 
    int n; 
    // infinite loop for chat 
    for (;;) { 
        sendMsg(sock_desc);
        recvMsg(sock_desc);
    } 
} 

void terminate(int num){
    close(socket_fd);
    printf("\nClosing Socket. Exiting Application\n");
    exit(0);
}

//main driver program
int main(int argc, char *argv[])
{


    if(argc != 3){
        printf("Incorrect Paramters. Please provide IP and Port. Exiting....");
        exit(0);
    }

    int portno = atoi(argv[2]);
    char *ip = argv[1];

    printf("IP is %s and Port Number is %i", ip, portno);


    int serv_sock, read_size, conn_desc;
    struct sockaddr_in server;
    
    //Create socket
    printf("Create the socket\n");
    serv_sock = socket(AF_INET, SOCK_STREAM, 0);
    socket_fd = serv_sock;
    if(serv_sock == -1){
        printf("Unable to create socket\n");
        exit(0);
    }
    printf("Socket created Successfully\n");
    signal(SIGINT, terminate);


    struct sockaddr_in server_conn;
    memset(&server, 0, sizeof(server));

    server_conn.sin_addr.s_addr = inet_addr(ip); //Local Host
    server_conn.sin_family = AF_INET;
    server_conn.sin_port = htons(portno);
    conn_desc = connect(serv_sock,(struct sockaddr *)&server_conn,sizeof(struct sockaddr_in));


    //Connecting to server
    if (conn_desc < 0)
    {
        printf("Connection failed. Exiting....\n");
        exit(0);
    }
    printf("Sucessfully conected with server\n\n");


    //chat with the server
    chat(serv_sock);

    close(serv_sock);
    return 0;
}

