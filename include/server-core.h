#ifndef SERVER_CORE_H
#define SERVER_CORE_H

#include "protocol.h"

typedef struct
{
    int socket;
    struct sockaddr_in address;
    int client_id;
} ClientInfo;

void *handle_client(void *arg);
void broadcast_message(ChatPacket *pkt, int sender_fd);
void remove_client(int fd);

#endif