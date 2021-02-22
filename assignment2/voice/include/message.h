#ifndef MESSAGE_H
#define MESSAGE_H

typedef unsigned char uint8;
#define BUFSIZE 1024

typedef struct Message {
    int msgtype ; //GROUPMESSAGE, USERMESSAGE
    char user_id[20];
    char recipient_id[20];
    char group_id[20];
    unsigned long long timestamp;
    uint8 msg[100];
} message;


typedef struct VoiceMessage {
    char name[20];
    int msgtype; // 0 for group, 1 for personal
    char recipient_id[20];
    char group_id[20];
    unsigned long long timestamp;
    unsigned char msg[BUFSIZE];
    int voice_or_text; // 0 for voice, 1 for text 
} voice;


struct Init {
    char user_id[20];
    char group_id[20];
};

#endif /* !MESSAGE_H */
