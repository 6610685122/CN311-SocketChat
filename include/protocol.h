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
#define MAX_CLIENTS 2
#define BUFFER_SIZE 1024
#define NAME_LEN 50

typedef enum
{
    MSG_CHAT,
    MSG_HW2,
    MSG_HW4_REQ,
    MSG_HW4_RES,
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

int convert_be_to_ad(int be_year);
int extract_and_convert_year(const char *input_msg);

void run_hw2_part1();
void run_hw2_part2(long max_value);

#endif