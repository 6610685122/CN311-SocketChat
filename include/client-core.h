#ifndef CLIENT_CORE_H
#define CLIENT_CORE_H

// ฟังก์ชันหลักสำหรับเริ่มต้นระบบ Client และเชื่อมต่อไปยัง Server
void start_client(const char *server_ip, int port);

// Thread Handler สำหรับรอฟังและรับข้อความที่ส่งมาจาก Server (Read Thread)
void *receive_handler(void *socket_desc);

// ฟังก์ชันสำหรับรอรับ Input จากคีย์บอร์ดแล้วส่งไปยัง Server (Write Thread / Main Loop)
void *send_handler(void *socket_desc);

#endif