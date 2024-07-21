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
#define CONTROL_UDP_PORT 4000
#define CONTROL_DISABLED 0
#define CONTROL_ENABLED 1
#define CONTROL_PROPAGATION_DELAY 1
#define CONTROL_OPERATION_READ 1
#define CONTROL_OPERATION_WRITE 2
#define CONTROL_OBJECT_OUT1 1
#define CONTROL_OBJECT_OUT2 2
#define CONTROL_OBJECT_OUT3 2
#define CONTROL_OBJECT_OUT1_PROPERTY_FREQUENCY_INDEX 255
#define CONTROL_OBJECT_OUT1_PROPERTY_FREQUENCY_1_HZ 1000 // scaled to mHz
#define CONTROL_OBJECT_OUT1_PROPERTY_FREQUENCY_2_HZ 2000 // scaled to mHz
#define CONTROL_OBJECT_OUT1_PROPERTY_AMPLITUDE_8000 8000 // unscaled, as 8000000 would not fit in 16 bit field
#define CONTROL_OBJECT_OUT1_PROPERTY_AMPLITUDE_4000 4000 // unscaled, as 8000000 would not fit in 16 bit field
#define CONTROL_OBJECT_OUT1_PROPERTY_AMPLITUDE_INDEX 170

// Control message with operation, object, property and value
typedef struct
{
    uint16_t operation;
    uint16_t object;
    uint16_t property;
    uint16_t value;
} control_message;

// Control UDP socket with address
typedef struct {
    int sockfd;
    struct sockaddr_in servaddr;
} udp_socket;

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

/**
 * Opens a UDP control socket on the specified port.
 *
 * @param control_udp_port The port number for the UDP control socket.
 * @return udp_socket socket file descriptor with address data of the opened socket, or -1 if an error occurred.
 */
udp_socket open_udp_control_socket(int control_udp_port);

/**
 * Sends a control message over the specified UDP control socket.
 *
 * @param udp_control_socket The UDP control socket to send the message on.
 * @param msg The control message to send.
 * @return Returns 0 on success, or a negative value on failure.
 */
int send_control_message(udp_socket udp_control_socket, control_message msg);

/**
 * @brief Closes the UDP socket.
 *
 * This function is used to close the UDP socket specified by the `udp_control_socket` parameter.
 *
 * @param udp_control_socket The UDP socket to be closed.
 */
void close_udp_socket(udp_socket udp_control_socket);

#endif // PROTOCOL_H
