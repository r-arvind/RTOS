#ifndef MESSAGE_H
#define MESSAGE_H

typedef struct Message {
    int message_type ; //GROUPMESSAGE, USERMESSAGE
    char sender[20];
    char receiver[20];
    char message[100];
} message;

struct Register {
    char name[20];
};

#endif /* !MESSAGE_H */
