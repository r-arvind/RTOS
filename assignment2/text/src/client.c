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
pthread_mutex_t stdinMutex;
message myMessage;

//receive the data from the server
void *recvMsg()
{
    for (;;)
    {
        int msg;
        message m;
        msg = recv(socket_fd, &m, sizeof(m), 0);
        fprintf(stdout,"\33[2K\r%s : %sYour Message: ", m.sender ,m.message);
        fflush(stdout);
    }
}

void terminate(int num);
void flush_stdin()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

//main driver program
int main(int argc, char *argv[])
{

    if (argc != 4){
        printf("Incorrect Paramters. Only %i provided.\nPlease provide <Server IP> <Port> <Your Name> <Message Type (group - 0/user - 1)> <Receiver>.\n Exiting....\n",argc);
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
    int type;
    char receiver[20];
    for (;;)
    {
        // pthread_mutex_lock(&stdinMutex);
        printf("Your Message: ");
        if(fgets(myMessage.message, sizeof(myMessage.message), stdin) == NULL){
            continue;
        }
        // pthread_mutex_unlock(&stdinMutex);
        printf("Message Type (0 for GROUP or 1 for USER): ");
		scanf("%d%*c",&type);
        myMessage.message_type = type;
		if(type == 1){
			printf("Enter reciever number: ");
			scanf("%[^\n]%*c", receiver);
            strcpy(myMessage.receiver,receiver);
		}
        if(strcmp(myMessage.message,"\n") == 0){
            continue;
        }
        send(socket_fd, &myMessage, sizeof(myMessage), 0);
    }

    // pthread_kill(read_thread, NULL);

}


void terminate(int num) {
    // pthread_kill(read_thread, NULL);
    // pthread_mutex_lock(&stdinMutex);
    char mode;
    printf("\nPress 'e' to exit or 'c' to change chat mode : \n");
    scanf("%c", &mode);

    if(mode == 'e'){
    close(socket_fd);
    printf("\nClosing Socket. Exiting Application\n");
    exit(0);
    }
    // if(mode == 'c'){
    //     int type;
    //     char receiver[20];
    //     printf("Message Type (0 for GROUP or 1 for USER): ");
	// 	scanf("%d%*c",&type);
    //     myMessage.message_type = type;
	// 	if(type == 1){
	// 		printf("Enter reciever number: ");
	// 		fgets(receiver,sizeof(receiver),stdin);
    //         receiver[strcspn(receiver, "\n")] = 0;
    //         strcpy(myMessage.receiver,receiver);
	// 	}
    //     // flush_stdin();
    //     // pthread_mutex_unlock(&stdinMutex);

    // }

}