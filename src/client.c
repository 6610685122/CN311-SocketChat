#include "client-core.h"

int sock = 0;
char client_name[NAME_LEN];

void *receive_handler(void *arg)
{
    char buffer[sizeof(ChatPacket)];
    ChatPacket pkt;

    while (recv(sock, buffer, sizeof(ChatPacket), 0) > 0)
    {
        deserialize_packet(buffer, &pkt);
        printf("\r[%s] %s: %s\n> ", pkt.timestamp, pkt.sender_name, pkt.message);
        fflush(stdout);
    }

    return NULL;
}

void *send_handler(void *arg)
{
    char buffer[BUFFER_SIZE];
    char pkt_buffer[sizeof(ChatPacket)];
    ChatPacket pkt;

    while (1)
    {
        printf("> ");
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = 0;

        if (strlen(buffer) == 0) continue;

        if (strcmp(buffer, "/exit") == 0)
        {
            pkt.type = MSG_EXIT;
            strcpy(pkt.sender_name, client_name);
            strcpy(pkt.message, "Left the chat");
            get_current_timestamp(pkt.timestamp);
            serialize_packet(&pkt, pkt_buffer);
            send(sock, pkt_buffer, sizeof(ChatPacket), 0);
            exit(0);
        }
        else
        {
            pkt.type = MSG_CHAT;
            strcpy(pkt.sender_name, client_name);
            strcpy(pkt.message, buffer);
            get_current_timestamp(pkt.timestamp);
            serialize_packet(&pkt, pkt_buffer);
            send(sock, pkt_buffer, sizeof(ChatPacket), 0);
        }
    }

    return NULL;
}

int main()
{
    struct sockaddr_in serv_addr;

    printf("Enter your name: ");
    fgets(client_name, NAME_LEN, stdin);
    client_name[strcspn(client_name, "\n")] = 0;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // TODO: Update this IP to your friend's Hamachi IP
    if (inet_pton(AF_INET, "25.20.172.81", &serv_addr.sin_addr) <= 0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }

    pthread_t recv_thread, send_thread;
    pthread_create(&recv_thread, NULL, receive_handler, NULL);
    pthread_create(&send_thread, NULL, send_handler, NULL);

    pthread_join(recv_thread, NULL);
    pthread_join(send_thread, NULL);

    return 0;
}