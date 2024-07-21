/**
 * @file protocol.h
 * @brief Header file for the protocol module.
 *
 * This file contains the declarations and definitions for the protocol module.
 * The protocol module is responsible for defining the protocols and data structures
 * used by the application.
 */
#define _POSIX_C_SOURCE 200809L // clock_gettime
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
#define REPORT_COUNT_100 100
#define REPORT_COUNT_UNLIMITED -1
#define REPORT_BUFFER_SIZE 32768
#define REPORT_INTERVAL_100MS 100
#define REPORT_INTERVAL_20MS 20
#define PROTOCOL_BUFFER_SIZE 1024
#define DATA_SIZE 1024
#define CONTROL_UDP_PORT 4000
#define VALID_OUT1_DATA_MAX 8.0
#define VALID_OUT1_DATA_MIN -8.0
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

// Report message with timestamp and 3 float values
typedef struct
{
    long long timestamp;
    float out1;
    float out2;
    float out3;
} report_message;

/**
 * Returns the current timestamp in epoch milliseconds.
 *
 * @return The current timestamp in epoch milliseconds.
 */
long long current_timestamp_ms();

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

/**
 * Sends a report to a file and multiple sockets at a specified interval.
 *
 * This function sends a report to a specified file and multiple sockets at a given interval.
 *
 * @param file The file to which the report will be sent.
 * @param interval_ms The interval, in milliseconds, at which the report will be sent.
 * @param sockfd_out1 The first socket file descriptor to which the report will be sent.
 * @param sockfd_out2 The second socket file descriptor to which the report will be sent.
 * @param sockfd_out3 The third socket file descriptor to which the report will be sent.
 * @param udp_control_socket The UDP control socket to which the report will be sent.
 * @param count The number of reports to be sent.
 * @return Returns 0 on success, -1 on failure.
 */
int print_report(FILE *file, int interval_ms, int sockfd_out1, int sockfd_out2, int sockfd_out3, udp_socket udp_control_socket, int count);

/**
 * Parses a report line and populates the provided report_message structure.
 *
 * @param line The input line to parse.
 * @param message A pointer to the report_message structure to populate.
 * @return Returns 0 on success, or a negative value on failure.
 */
int parse_report_line(const char *line, report_message *message);

/**
 * Checks the timing and control of the given buffer.
 *
 * This function checks the timing and control of the provided buffer
 * based on the specified interval in milliseconds.
 *
 * @param buffer The buffer to check.
 * @param interval_ms The interval in milliseconds.
 * @return Returns 0 if the timing and control are valid, otherwise returns -1.
 */
int check_timing_and_control(const char *buffer, long interval_ms);

/**
 * Sends periodic reports to the standard output.
 *
 * This function sends periodic reports to the standard output at a specified interval.
 *
 * @param interval_ms The interval between each report in milliseconds.
 * @param control_enable Flag indicating whether control is enabled or not.
 *                      If control is enabled, the function will send control messages along with the reports.
 *                      If control is disabled, only the reports will be sent.
 *
 * @return Returns 0 on success, or a negative value if an error occurs.
 */
int report_stdout(int interval_ms, int control_enable);

/**
 * Replaces all occurrences of a specified word in a string with a new word.
 *
 * @param str The input string in which the replacement will be performed.
 * @param oldWord The word to be replaced.
 * @param newWord The new word that will replace the old word.
 * @return A pointer to the modified string.
 */
char *replaceAll(const char *str, const char *oldWord, const char *newWord);

/**
 * @brief Sets up a timer with the specified interval.
 *
 * This function initializes a timer identified by `timer_id` with the specified `interval_ms`.
 *
 * @param timer_id Pointer to the timer identifier.
 * @param interval_ms The interval in milliseconds for the timer.
 */
void setup_timer(timer_t *timer_id, int interval_ms);

/**
 * @brief Handles the timer signal.
 *
 * This function is the handler for the timer signal. It is called when the timer signal is received.
 *
 * @param sig The signal number.
 * @param si A pointer to a siginfo_t structure containing information about the signal.
 * @param uc A pointer to a ucontext_t structure containing the user context.
 */
void timer_handler(int sig, siginfo_t *si, void *uc);

/**
 * Formats a report in the provided report buffer and outputs the formatted report by the specified output variables.
 *
 * @param report_buffer   The report buffer containing the report data.
 * @param buffer_size   The size of the buffer.
 * @param out1          The first output variable to store the formatted report.
 * @param out2          The second output variable to store the formatted report.
 * @param out3          The third output variable to store the formatted report.
 */
void format_report(char *report_buffer, size_t buffer_size, char *out1, char *out2, char *out3);

/**
 * @brief Handles the SIGINT signal for reporting.
 *
 * This function is called when the SIGINT signal is received. It is responsible for handling the signal and performing the necessary actions for reporting.
 *
 * @param sig The signal number.
 */
void handle_report_sigint(int sig);

#endif // PROTOCOL_H
