#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include "message.h"

int socket_fd;
pthread_t read_thread;
message myMessage;

//receive the data from the server
void *recvMsg()
{
    for (;;)
    {
        int msg;
        message m;
        // memset(recvBuffer, 0, sizeof(recvBuffer));
        msg = recv(socket_fd, &m, sizeof(m), 0);
        // printf("\33[2K\r%s : %sYour Message:\n", m.sender ,m.message);
        fprintf(stdout,"\33[2K\r%s : %sYour Message: ", m.sender ,m.message);
        fflush(stdout);
    }
}

void terminate(int num);


//main driver program
int main(int argc, char *argv[])
{

    if (argc != 4)
    {
        printf("Incorrect Paramters. Please provide IP and Port and your name. Exiting....");
        exit(0);
    }

    int portno = atoi(argv[2]);
    char *ip = argv[1];
    char *myname = argv[3];
    strcpy(myMessage.sender,myname);

    printf("IP is %s and Port Number is %i", ip, portno);

    int serv_sock, read_size, conn_desc;
    struct sockaddr_in server;

    //Create socket
    printf("Create the socket\n");
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    // socket_fd = serv_sock;
    if (socket_fd == -1)
    {
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
    conn_desc = connect(socket_fd, (struct sockaddr *)&server_conn, sizeof(struct sockaddr_in));

    //Connecting to server
    if (conn_desc < 0)
    {
        printf("Failed to connect to server. Exiting....\n");
        exit(0);
    }
    printf("Sucessfully conected with server\n\n");

    struct Register reg;
    strcpy(reg.name, myname);
    send(socket_fd, &reg, sizeof(reg),0);
    printf("Successfully joined the group\n");
    pthread_create(&read_thread, NULL, recvMsg, NULL);
    for (;;)
    {
        printf("Your Message: ");
        fgets(myMessage.message, sizeof(myMessage.message), stdin);
        send(socket_fd, &myMessage, sizeof(myMessage), 0);
    }

    // pthread_kill(read_thread, NULL);

}


void terminate(int num) {
    // pthread_kill(read_thread, NULL);
    close(socket_fd);
    printf("\nClosing Socket. Exiting Application\n");
    exit(0);
}