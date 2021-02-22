#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include "message.h"
#include "audio.h"
#include <errno.h>

int socket_fd;
pthread_t read_thread;
pthread_mutex_t stdinMutex;
voice myMessage;

//receive the data from the server
void *recvMsg()
{
    pa_simple *s = NULL;
    int err;
    uint8 buf[1024];
    /* Create a new playback stream */
    if (!(s = pa_simple_new(NULL, "receiver", PA_STREAM_PLAYBACK, NULL, "playback", &ss, NULL, NULL, &err))) {
        fprintf(stderr, __FILE__ ": pa_simple_new() failed: %s\n", pa_strerror(err));
        if (s){
            pa_simple_free(s);
        }
        return NULL;
    }

	for (;;) {

		voice m;
        ssize_t r;

        if ((r = read(socket_fd, buf, sizeof(buf))) <= 0) {
            if (r == 0) /* EOF */
                break;

            fprintf(stderr, __FILE__": read() failed: %s\n", strerror(errno));
                if (s){
                    pa_simple_free(s);
                }
                return NULL;
        }

        if (pa_simple_write(s, buf, sizeof(buf), &err) < 0) {
            fprintf(stderr, __FILE__": pa_simple_write() failed: %s\n", pa_strerror(err));
            if (s){
                pa_simple_free(s);
                }
            return NULL;
        }
    }
    /* Make sure that every single sample was played */
    if (pa_simple_drain(s, &err) < 0) {
        fprintf(stderr, __FILE__": pa_simple_drain() failed: %s\n", pa_strerror(err));
        if (s){
            pa_simple_free(s);
        }
    return NULL;
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

    strcpy(myMessage.name,myname);

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

    struct Init reg;
    strcpy(reg.user_id, myname);
    send(socket_fd, &reg, sizeof(reg),0);
    printf("Successfully joined the group\n");
    pthread_create(&read_thread, NULL, recvMsg, NULL);
    int type;
    pa_simple *s = NULL;
    int err = 0;
    // char receiver[20];

    if (!(s = pa_simple_new(NULL, myname, PA_STREAM_RECORD, NULL, "record", &ss, NULL, NULL, &err))) {
        fprintf(stderr, __FILE__": pa_simple_new() failed: %s\n", pa_strerror(err));
        if (s) {
            pa_simple_free(s);
            return 0;
        }
    }
    uint8 buf[1024];
    for (;;)
    {
        // pthread_mutex_lock(&stdinMutex);
        // printf("Your Message: ");
        // fgets(myMessage.msg, sizeof(myMessage.msg), stdin);
        // pthread_mutex_unlock(&stdinMutex);
        // printf("Message Type (0 for GROUP or 1 for USER): ");
		// scanf("%d%*c",&type);
        if (pa_simple_read(s, buf, sizeof(buf), &err) < 0) {
            fprintf(stderr, __FILE__": pa_simple_read() failed: %s\n", pa_strerror(err));
            if (s) {
                pa_simple_free(s);
                return 0;
            }
        }

        // myMessage.msgtype = 0;
		// if(type == 1){
			// printf("Enter reciever number: ");
			// scanf("%[^\n]%*c", receiver);
            // strcpy(myMessage.recipient_id,receiver);
		// }
        send(socket_fd, buf, sizeof(buf), 0);
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