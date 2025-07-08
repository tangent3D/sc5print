// UNAPI functionality definitions
// tangent.space 2025

#ifndef TCP_H
#define TCP_H

#define TCPIP_TCP_OPEN 13
#define TCPIP_TCP_SEND 17
#define TCPIP_TCP_CLOSE 14

// Function declarations

int init_tcp_connection();
int send_tcp_data(const void *data, unsigned int length);
int close_tcp_connection();

#endif