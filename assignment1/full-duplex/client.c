#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>

#define MAXLENGTH 100

char sendBuffer[MAXLENGTH] = {0};
char recvBuffer[MAXLENGTH] = {0};
int socket_fd;

// Send the data to the server and set the timeout of 20 seconds
void *sendMsg()
{
    for (;;)
    {
        int stat;
        memset(sendBuffer, 0, sizeof(sendBuffer));
        // printf("Your Message : ");
        fgets(sendBuffer, MAXLENGTH, stdin);
        stat = send(socket_fd, sendBuffer, strlen(sendBuffer), 0);
    }
}

//receive the data from the server
void *recvMsg()
{
    for (;;)
    {
        int msg;
        memset(recvBuffer, 0, sizeof(recvBuffer));
        msg = recv(socket_fd, recvBuffer, MAXLENGTH, 0);
        printf("%s", recvBuffer);
        // printf("Your Message : ");
    }
}

void terminate(int num)
{
    close(socket_fd);
    printf("\nClosing Socket. Exiting Application\n");
    exit(0);
}

//main driver program
int main(int argc, char *argv[])
{

    if (argc != 3)
    {
        printf("Incorrect Paramters. Please provide IP and Port. Exiting....");
        exit(0);
    }

    int portno = atoi(argv[2]);
    char *ip = argv[1];

    printf("IP is %s and Port Number is %i", ip, portno);

    int serv_sock, read_size, conn_desc;
    struct sockaddr_in server;
    pthread_t read_thread, write_thread;

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
        printf("Connection failed. Exiting....\n");
        exit(0);
    }
    printf("Sucessfully conected with server\n\n");

    pthread_create(&read_thread, NULL, recvMsg, NULL);
    pthread_create(&write_thread, NULL, sendMsg, NULL);

    pthread_join(read_thread, NULL);
    pthread_join(write_thread, NULL);

}
