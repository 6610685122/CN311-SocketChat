#ifndef CLIENT_CORE_H
#define CLIENT_CORE_H

#include "protocol.h"

void *receive_handler(void *arg);
void *send_handler(void *arg);

#endif