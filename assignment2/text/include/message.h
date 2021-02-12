#ifndef MESSAGE_H
#define MESSAGE_H

struct Message {
    int message_type ; //GROUPMESSAGE, GROUPQUERY
    char sender[20];
    char receiver[20];
    char message[100];
};

struct Register {
    char name[20];
};

#endif /* !MESSAGE_H */
