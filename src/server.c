#include "server-core.h"
#include <signal.h>

int client_fds[MAX_CLIENTS];
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;
int client_count = 0;

void broadcast_message(ChatPacket *pkt, int sender_fd)
{
    char buffer[sizeof(ChatPacket)];
    serialize_packet(pkt, buffer);

    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (client_fds[i] != -1 && client_fds[i] != sender_fd)
        {
            if (send(client_fds[i], buffer, sizeof(ChatPacket), 0) < 0)
            {
                perror("Broadcast failed");
            }
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void remove_client(int fd)
{
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (client_fds[i] == fd)
        {
            client_fds[i] = -1;
            client_count--;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void *handle_client(void *arg)
{
    ClientInfo *c_info = (ClientInfo *)arg;
    int fd = c_info->socket;
    char buffer[sizeof(ChatPacket)];
    ChatPacket pkt;

    printf("[Server] Client %d connected (FD: %d)\n", c_info->client_id, fd);

    while (recv(fd, buffer, sizeof(ChatPacket), 0) > 0)
    {
        deserialize_packet(buffer, &pkt);

        if (pkt.type == MSG_EXIT)
        {
            printf("[Server] Client %s disconnected\n", pkt.sender_name);
            break;
        }
        else if (pkt.type == MSG_HW4_REQ)
        {
            printf("[Server] HW4 Request from %s: %s\n", pkt.sender_name, pkt.message);

            // Send acknowledgement
            ChatPacket ack_pkt;
            ack_pkt.type = MSG_CHAT;
            strcpy(ack_pkt.sender_name, "Server");
            strcpy(ack_pkt.message, "Server received");
            get_current_timestamp(ack_pkt.timestamp);
            serialize_packet(&ack_pkt, buffer);
            send(fd, buffer, sizeof(ChatPacket), 0);

            // Convert year
            int ad_year = extract_and_convert_year(pkt.message);
            ChatPacket res_pkt;
            res_pkt.type = MSG_HW4_RES;
            strcpy(res_pkt.sender_name, "Server");
            sprintf(res_pkt.message, "%d", ad_year);
            get_current_timestamp(res_pkt.timestamp);
            serialize_packet(&res_pkt, buffer);
            send(fd, buffer, sizeof(ChatPacket), 0);
        }
        else
        {
            printf("[%s] %s: %s\n", pkt.timestamp, pkt.sender_name, pkt.message);
            broadcast_message(&pkt, fd);
        }
    }

    close(fd);
    remove_client(fd);
    free(c_info);
    return NULL;
}

int main()
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    for (int i = 0; i < MAX_CLIENTS; i++)
        client_fds[i] = -1;

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
    {
        perror("Setsockopt failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0)
    {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("[Server] Listening on port %d...\n", PORT);

    while (1)
    {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
        {
            perror("Accept failed");
            continue;
        }

        pthread_mutex_lock(&clients_mutex);
        if (client_count < MAX_CLIENTS)
        {
            int index = -1;
            for (int i = 0; i < MAX_CLIENTS; i++)
            {
                if (client_fds[i] == -1)
                {
                    index = i;
                    break;
                }
            }
            client_fds[index] = new_socket;
            client_count++;

            ClientInfo *c_info = malloc(sizeof(ClientInfo));
            c_info->socket = new_socket;
            c_info->address = address;
            c_info->client_id = index + 1;

            pthread_t thread_id;
            pthread_create(&thread_id, NULL, handle_client, (void *)c_info);
            pthread_detach(thread_id);
        }
        else
        {
            printf("[Server] Max clients reached. Rejecting connection.\n");
            close(new_socket);
        }
        pthread_mutex_unlock(&clients_mutex);
    }

    return 0;
}