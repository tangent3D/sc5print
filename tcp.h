// UNAPI functionality definitions
// tangent.space 2025

#ifndef TCP_H
#define TCP_H

// Function declarations

int init_tcp_connection();
int send_tcp_data(const void *data, unsigned int length);
int close_tcp_connection();

#endif