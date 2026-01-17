// UNAPI functionality definitions
// tangent.space 2025

#ifndef TCP_H
#define TCP_H

#define TCPIP_TCP_OPEN 13
#define TCPIP_TCP_SEND 17
#define TCPIP_TCP_CLOSE 14
#define TCPIP_TCP_ABORT 15
#define TCPIP_TCP_STATE 16
#define TCPIP_WAIT 29

#define ERR_OK 0
#define ERR_NO_CONN 11
#define ERR_BUFFER 13

#define TCP_STATE_ESTABLISHED 4

extern unsigned char params[13];

int init_tcp_connection();
int send_tcp_data(const void *data, unsigned int length);
int close_tcp_connection();

#endif
