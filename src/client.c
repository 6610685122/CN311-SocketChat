<<<<<<< HEAD
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include "../include/client_core.h"
#include "../include/protocol.h" // ดึงข้อมูลโครงสร้างแพ็กเกจกลางมาจากคนที่ 3

int main(int argc, char *argv[]) {
    // ตรวจสอบการกรอก IP และ Port ผ่าน Command Line
    if (argc < 3) {
        printf("วิธีใช้งาน: %s <Server IP> <Port>\n", argv[0]);
        return 1;
    }

    const char *server_ip = argv[1];
    int port = atoi(argv[2]);

    // เริ่มทำงานระบบ Client
    start_client(server_ip, port);

    return 0;
}

void start_client(const char *server_ip, int port) {
    int sock_fd;
    struct sockaddr_in server_addr;
    pthread_t recv_thread;

    // 1. สร้าง Socket (อิงจากความรู้ HW4)
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd == -1) {
        perror("ไม่สามารถสร้าง Socket ได้");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(server_ip);
    server_addr.sin_port = htons(port);

    // 2. เชื่อมต่อไปยัง Server
    if (connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("การเชื่อมต่อล้มเหลว");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }
    printf("--- เชื่อมต่อเข้าสู่ Chatroom สำเร็จ ---\n");
    printf("พิมพ์ข้อความแล้วกด Enter เพื่อส่ง หรือพิมพ์ 'exit' เพื่อออกจากโปรแกรม\n\n");

    // 3. (HW2 ประยุกต์) แตก Thread มารอรับข้อความจาก Server แยกต่างหาก
    // โยน sock_fd เข้าไปเพื่อให้ Thread นั้นใช้ recv() ได้
    if (pthread_create(&recv_thread, NULL, receive_handler, (void *)&sock_fd) < 0) {
        perror("ไม่สามารถสร้าง Thread สำหรับรับข้อความได้");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }

    // 4. ให้ Main Thread ทำหน้าที่รอรับคีย์บอร์ดและส่งข้อความ (ไม่ต้องแตก Thread เพิ่ม)
    send_handler(&sock_fd);

    // ปิด Socket เมื่อหลุดลูปการส่ง
    close(sock_fd);
    printf("ออกจากห้องแชทแล้ว\n");
}

void *receive_handler(void *socket_desc) {
    int sock = *(int *)socket_desc;
    char buffer[2048]; // ขนาดสมมติตามข้อตกลง protocol
    int read_size;

    // ลูปเพื่อรอรับข้อมูลจาก Server ตลอดเวลา
    while ((read_size = recv(sock, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[read_size] = '\0';
        
        /* 🔥 เทคนิคแก้ปัญหาท้าทาย: Terminal Screen Management
           \r    = ย้ายเคอร์เซอร์กลับไปต้นบรรทัดปัจจุบัน
           \33[2K = ล้างข้อความในบรรทัดปัจจุบันทั้งหมด
           วิธีนี้จะเคลียร์คำว่า "You: [กำลังพิมพ์...]" ออกไปชั่วคราว เพื่อพ่นข้อความใหม่จากเพื่อน
        */
        printf("\r\33[2K"); 
        
        // แสดงข้อความที่ได้รับจากเพื่อน (จุดนี้ต้องทำงานร่วมกับฟังก์ชันแกะแพ็กเกจของคนที่ 3)
        printf("%s\n", buffer);
        
        // พ่น Prompt คำว่า "You: " กลับขึ้นมาใหม่ เพื่อให้ผู้ใช้พิมพ์ต่อได้ทันที
        printf("You: ");
        fflush(stdout);
    }

    if (read_size == 0) {
        printf("\n[ระบบ] Server ปิดการเชื่อมต่อ\n");
    } else if (read_size == -1) {
        perror("recv ล้มเหลว");
    }

    exit(0); // ถ้าเซิร์ฟเวอร์หลุด ให้จบโปรแกรมทั้งหมด
}

void *send_handler(void *socket_desc) {
    int sock = *(int *)socket_desc;
    char input_buf[2048];

    while (1) {
        printf("You: ");
        fflush(stdout);
        
        if (fgets(input_buf, sizeof(input_buf), stdin) == NULL) {
            break;
        }

        // ตัดตัวขึ้นบรรทัดใหม่ (\n) ที่ติดมาจาก fgets ออก
        input_buf[strcspn(input_buf, "\n")] = 0;

        // ตรวจสอบคำสั่งออกจากโปรแกรม
        if (strcmp(input_buf, "exit") == 0) {
            break;
        }

        // ตรวจสอบกรณีส่งข้อความว่าง
        if (strlen(input_buf) == 0) {
            continue;
        }

        // ส่งข้อความไปยัง Server (จุดนี้ต้องทำงานร่วมกับฟังก์ชันแพ็กข้อมูลของคนที่ 3 เช่นกัน)
        if (send(sock, input_buf, strlen(input_buf), 0) < 0) {
            perror("ส่งข้อความล้มเหลว");
            break;
        }
    }
    return NULL;
=======
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

        if (pkt.type == MSG_HW4_RES)
        {
            printf("\n[Server] Converted Year (AD): %s\n", pkt.message);
            printf("[System] Sending 'Bye' to server...\n");

            ChatPacket bye_pkt;
            bye_pkt.type = MSG_EXIT;
            strcpy(bye_pkt.sender_name, client_name);
            strcpy(bye_pkt.message, "Bye");
            get_current_timestamp(bye_pkt.timestamp);
            serialize_packet(&bye_pkt, buffer);
            send(sock, buffer, sizeof(ChatPacket), 0);

            printf("[System] Disconnecting...\n");
            exit(0);
        }
        else
        {
            printf("\r[%s] %s: %s\n> ", pkt.timestamp, pkt.sender_name, pkt.message);
            fflush(stdout);
        }
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
        else if (strcmp(buffer, "/hw2") == 0)
        {
            run_hw2_part1();
            run_hw2_part2(100000000); // MAX_VALUE > 100,000,000
        }
        else if (strncmp(buffer, "/hw4 ", 5) == 0)
        {
            pkt.type = MSG_HW4_REQ;
            strcpy(pkt.sender_name, client_name);
            strcpy(pkt.message, buffer + 5);
            get_current_timestamp(pkt.timestamp);
            serialize_packet(&pkt, pkt_buffer);
            send(sock, pkt_buffer, sizeof(ChatPacket), 0);
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

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
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
>>>>>>> origin/protocol
}