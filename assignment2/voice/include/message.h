#ifndef MESSAGE_H
#define MESSAGE_H

typedef unsigned char uint8;

typedef struct Message {
    int msgtype ; //GROUPMESSAGE, USERMESSAGE
    char name[20];
    char recipient_id[20];
    char group_id[20];
    unsigned long long timestamp;
    uint8 msg[100];
} message;


typedef struct VoiceMessage {
    int msgtype ; //GROUPMESSAGE, USERMESSAGE
    char name[20];
    char recipient_id[20];
    char group_id[20];
    unsigned long long timestamp;
    uint8 msg[1024];
} voice;


struct Init {
    char user_id[20];
    char group_id[20];
};

#endif /* !MESSAGE_H */
