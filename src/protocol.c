#include "protocol.h"

void get_current_timestamp(char *buffer)
{
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(buffer, 30, "%H:%M:%S", t);
}

void serialize_packet(ChatPacket *pkt, char *buffer)
{
    memcpy(buffer, pkt, sizeof(ChatPacket));
}

void deserialize_packet(char *buffer, ChatPacket *pkt)
{
    memcpy(pkt, buffer, sizeof(ChatPacket));
}