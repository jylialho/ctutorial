/**
 * @file protocol.h
 * @brief Header file for the protocol module.
 *
 * This file contains the declarations and definitions for the protocol module.
 * The protocol module is responsible for defining the protocols and data structures
 * used by the application.
 */
#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/time.h> // timeval
#include <float.h>    // DBL_MAX

#define TCP_PORT_BAD 1
#define TCP_PORT_OUT1 4001
#define TCP_PORT_OUT2 4002
#define TCP_PORT_OUT3 4003
#define PROTOCOL_BUFFER_SIZE 1024
#define DATA_SIZE 1024
/**
 * Connects to a TCP port.
 *
 * @param port The port number to connect to.
 * @return Returns an socket file descriptor or -1 on error.
 */
int connect_to_tcp_port(int port);

/**
 * Reads the last line from a TCP socket.
 *
 * @param sockfd The socket file descriptor.
 * @param buf The buffer to store the read line.
 * @param bufsize The size of the buffer.
 * @return 0 on success, or -1 if an error occurred.
 */
int read_tcp_last_line(int sockfd, char *buf, int bufsize);

/**
 * Closes a TCP socket.
 *
 * @param sockfd The file descriptor of the socket to be closed.
 */
void close_tcp_socket(int sockfd);
#endif // PROTOCOL_H
