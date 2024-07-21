/**
 * @file protocol.c
 * @brief This file contains the implementation of the protocol module.
 */
#include "protocol.h"

// Control message 1 Hz frequency
control_message ctrl_msg_o3h_f1hz = {CONTROL_OPERATION_WRITE,
                                     CONTROL_OBJECT_OUT1,
                                     CONTROL_OBJECT_OUT1_PROPERTY_FREQUENCY_INDEX,
                                     CONTROL_OBJECT_OUT1_PROPERTY_FREQUENCY_1_HZ};

// Control message 8000 amplitude
control_message ctrl_msg_o3h_a8k = {CONTROL_OPERATION_WRITE,
                                    CONTROL_OBJECT_OUT1,
                                    CONTROL_OBJECT_OUT1_PROPERTY_AMPLITUDE_INDEX,
                                    CONTROL_OBJECT_OUT1_PROPERTY_AMPLITUDE_8000};

// Control message 2 Hz frequency
control_message ctrl_msg_o3l_f2hz = {CONTROL_OPERATION_WRITE,
                                     CONTROL_OBJECT_OUT1,
                                     CONTROL_OBJECT_OUT1_PROPERTY_FREQUENCY_INDEX,
                                     CONTROL_OBJECT_OUT1_PROPERTY_FREQUENCY_2_HZ};

// Control message 4000 amplitude
control_message ctrl_msg_o3l_a4k = {CONTROL_OPERATION_WRITE,
                                    CONTROL_OBJECT_OUT1,
                                    CONTROL_OBJECT_OUT1_PROPERTY_AMPLITUDE_INDEX,
                                    CONTROL_OBJECT_OUT1_PROPERTY_AMPLITUDE_4000};

int connect_to_tcp_port(int port)
{
    int sockfd;
    struct sockaddr_in serv_addr;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        // perror("socket");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Set socket to non-blocking mode
    int flags = fcntl(sockfd, F_GETFL, 0);
    if (flags < 0)
    {
        // perror("fcntl(F_GETFL)");
        close(sockfd);
        return -1;
    }

    if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) < 0)
    {
        // perror("fcntl(F_SETFL)");
        close(sockfd);
        return -1;
    }

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        if (errno != EINPROGRESS)
        {
            // perror("connect");
            close(sockfd);
            return -1;
        }
    }

    return sockfd;
}

int read_tcp_last_line(int sockfd, char *buf, int bufsize)
{
    static char internal_buffer[PROTOCOL_BUFFER_SIZE];
    ssize_t read_count = 0;
    ssize_t bytes_received = 0;
    // Default data when no data is received
    buf[0] = '-';
    buf[1] = '-';
    buf[2] = '\0';

    // Read all data from the socket
    while ((read_count = recv(sockfd, internal_buffer, PROTOCOL_BUFFER_SIZE - 1, 0)) > 0)
    {
        if (read_count > PROTOCOL_BUFFER_SIZE - 1)
        {
            // fprintf(stderr, "read count error");
            return -1;
        }
        bytes_received = read_count;
        internal_buffer[bytes_received] = '\0'; // Null-terminate the buffer
    }
    if (read_count < 0 && errno != EWOULDBLOCK)
    {
        // perror("recv");
        return -1;
    }

    // At least two bytes received and a newline is needed for a valid line
    if (bytes_received)
    {
        char *line_start = internal_buffer;
        char *next_newline = line_start;
        // Find the last line
        while ((next_newline = strchr(line_start, '\n')) != NULL)
        {
            size_t line_length = strcspn(line_start, "\n");
            if ((line_length > 0) && (line_length < PROTOCOL_BUFFER_SIZE - 1))
            {
                strncpy(buf, line_start, line_length);
                buf[line_length] = '\0';
                // printf("%lld Data size: %ld last: %s\n", current_timestamp_ms(), bytes_received, buf);
            }
            else
            {
                // fprintf(stderr, "line length error");
                return -1;
            }
            line_start = next_newline + 1;
            if (line_start > internal_buffer + PROTOCOL_BUFFER_SIZE - 2)
                return 0;
        }
        return 0; // Data received successfully
    }
    return 0; // No data received yet, try again later
}

void close_tcp_socket(int sockfd)
{
    close(sockfd);
}

udp_socket open_udp_control_socket(int control_udp_port)
{
    udp_socket new_udp_socket;
    new_udp_socket.sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (new_udp_socket.sockfd < 0)
    {
        // perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
    memset(&new_udp_socket.servaddr, 0, sizeof(new_udp_socket.servaddr));
    new_udp_socket.servaddr.sin_family = AF_INET;
    new_udp_socket.servaddr.sin_port = htons(control_udp_port);
    new_udp_socket.servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    return new_udp_socket;
}

int send_control_message(udp_socket udp_control_socket, control_message msg)
{
    // Convert fields to big-endian
    msg.operation = htons(msg.operation);
    msg.object = htons(msg.object);
    msg.property = htons(msg.property);
    msg.value = htons(msg.value);
    return sendto(udp_control_socket.sockfd, &msg, sizeof(msg), 0, (const struct sockaddr *)&udp_control_socket.servaddr, sizeof(udp_control_socket.servaddr));
}

void close_udp_socket(udp_socket udp_control_socket)
{
    close(udp_control_socket.sockfd);
}
