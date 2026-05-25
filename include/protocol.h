#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

#define PORT 12345
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024
#define NAME_LEN 50

typedef enum
{
    MSG_CHAT,
    MSG_EXIT
} MessageType;

typedef struct
{
    MessageType type;
    char sender_name[NAME_LEN];
    char message[BUFFER_SIZE];
    char timestamp[30];
} ChatPacket;

void get_current_timestamp(char *buffer);
void serialize_packet(ChatPacket *pkt, char *buffer);
void deserialize_packet(char *buffer, ChatPacket *pkt);

#endif