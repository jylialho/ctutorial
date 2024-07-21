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


// Global variable for reporting SIGINT
volatile int report_running = 1;

// Global variable for timer signal
volatile int timer_triggered = 0;

// Global variable for report timestamp
long long report_timestamp;

int report_stdout(int interval_ms, int control_enable)
{
    // Setup TCP sockets
    int sockfd_out1 = connect_to_tcp_port(TCP_PORT_OUT1);
    int sockfd_out2 = connect_to_tcp_port(TCP_PORT_OUT2);
    int sockfd_out3 = connect_to_tcp_port(TCP_PORT_OUT3);

    // UDP Control enable
    udp_socket udp_control_socket;
    if (control_enable > 0)
    {
        udp_control_socket = open_udp_control_socket(CONTROL_UDP_PORT);
    }
    else
    {
        udp_control_socket.sockfd = -1;
    }
    // report with 20 ms interval, terminate with SIGINT
    int result = print_report(stdout, interval_ms, sockfd_out1, sockfd_out2, sockfd_out3, udp_control_socket, REPORT_COUNT_UNLIMITED);
    // Close sockets
    close_tcp_socket(sockfd_out1);
    close_tcp_socket(sockfd_out2);
    close_tcp_socket(sockfd_out3);
    close_udp_socket(udp_control_socket);
    return result;
}

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

void handle_report_sigint(int sig)
{
    // fprintf(stderr, "\nReceived SIGINT (signal %d), exit.\n", sig);
    report_running = 0;
}

void timer_handler(int sig, siginfo_t *si, void *uc)
{
    timer_triggered = 1;
    report_timestamp = current_timestamp_ms();
}

long long current_timestamp_ms()
{
    struct timeval te;
    gettimeofday(&te, NULL);                                                               // get current time
    long long milliseconds = te.tv_sec * 1000LL + (long long)(te.tv_usec + 500L) / 1000LL; // calculate milliseconds
    return milliseconds;
}

void format_report(char *report_buffer, size_t buffer_size, char *out1, char *out2, char *out3)
{
    snprintf(report_buffer, buffer_size, "{\"timestamp\": %lld, \"out1\": \"%s\", \"out2\": \"%s\", \"out3\": \"%s\"}",
             report_timestamp, out1, out2, out3);
}

void setup_timer(timer_t *timer_id, int interval_ms)
{
    struct sigevent sev;
    struct sigaction sa;
    struct itimerspec its;

    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = timer_handler;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGRTMIN, &sa, NULL) == -1)
    {
        // perror("sigaction");
        exit(EXIT_FAILURE);
    }

    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = SIGRTMIN;
    sev.sigev_value.sival_ptr = timer_id;
    if (timer_create(CLOCK_REALTIME, &sev, timer_id) == -1)
    {
        // perror("timer_create");
        exit(EXIT_FAILURE);
    }

    its.it_value.tv_sec = 0;
    its.it_value.tv_nsec = interval_ms * 1000000; // interval_ms in ms
    its.it_interval.tv_sec = 0;
    its.it_interval.tv_nsec = interval_ms * 1000000; // interval_ms in ms

    if (timer_settime(*timer_id, 0, &its, NULL) == -1)
    {
        // perror("timer_settime");
        exit(EXIT_FAILURE);
    }
}

int check_timing_and_control(const char *buffer, long interval_ms)
{
    report_message report_messages[REPORT_COUNT_100];
    int report_count = 0;
    char *line;
    char buffer_copy[REPORT_BUFFER_SIZE];

    // Copy the buffer to a modifiable string
    strncpy(buffer_copy, buffer, sizeof(buffer_copy));
    buffer_copy[sizeof(buffer_copy) - 1] = '\0';

    // Split the buffer into lines and parse each line
    line = strtok(buffer_copy, "\n");
    while (line != NULL)
    {
        if (parse_report_line(line, &report_messages[report_count]))
        {
            report_count++;
        }
        line = strtok(NULL, "\n");
    }

    // Variables to track the state of out3
    int out3_is_5 = 0;
    int out3_is_0 = 0;

    // Condition initialization
    int out1_control_effect_f1a8_met = 0;
    int out1_control_effect_f2a4_met = 1;
    int timing_met = 1;

    // Time between consequent reports
    long long report_interval_ms = 0;

    // Check the conditions from the reports
    for (int i = 0; i < report_count - CONTROL_PROPAGATION_DELAY; i++)
    {
        report_interval_ms = report_messages[i + 1].timestamp;                  // t1
        report_interval_ms = report_interval_ms - report_messages[i].timestamp; // t0
        if ((report_interval_ms > interval_ms + 10) || (report_interval_ms < interval_ms - 10))
        {
            timing_met = 0;
            printf("%lld count: %d interval: %ld report: %lld i: %d t1: %lld t0: %lld\n", current_timestamp_ms(), report_count, interval_ms, report_interval_ms, i, report_messages[i + 1].timestamp, report_messages[i].timestamp);
        }

        if (report_messages[i].out3 == 5.0)
        {
            out3_is_5 = 1;
            out3_is_0 = 0;
        }
        else if (report_messages[i].out3 == 0.0)
        {
            out3_is_0 = 1;
            out3_is_5 = 0;
        }
        // default amplitude is 5.0, expecting to see larger output when out3 is at 5
        if (out3_is_5 && ((report_messages[i + CONTROL_PROPAGATION_DELAY].out1 > 5.0) || (report_messages[i + CONTROL_PROPAGATION_DELAY].out1 < -5.0))) 
        {
            out1_control_effect_f1a8_met = 1;
        }
        // default amplitude is 5.0, not expecting to see larger output when out3 is at 0
        if (out3_is_0 && ((report_messages[i + CONTROL_PROPAGATION_DELAY].out1 > 5.0) || (report_messages[i + CONTROL_PROPAGATION_DELAY].out1 < -5.0))) // default amplitude is 5.0
        {
            out1_control_effect_f2a4_met = 0;
        }
    }
    printf("%lld report timing met: %d out1 control effect f1a8 met: %d out1 control effect f2a4 met: %d\n", current_timestamp_ms(), timing_met, out1_control_effect_f1a8_met, out1_control_effect_f2a4_met);
    return (timing_met && out1_control_effect_f1a8_met && out1_control_effect_f2a4_met) ? 0 : -1;
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

char *replaceAll(const char *str, const char *oldWord, const char *newWord)
{
    char *result;
    int i, cnt = 0;
    int newWlen = strlen(newWord);
    int oldWlen = strlen(oldWord);

    // Counting the number of times old word occur in the string
    for (i = 0; str[i] != '\0'; i++)
    {
        if (strstr(&str[i], oldWord) == &str[i])
        {
            cnt++;
            i += oldWlen - 1;
        }
    }

    // Making new string of enough length
    result = (char *)malloc(i + cnt * (newWlen - oldWlen) + 1);

    i = 0;
    while (*str)
    {
        // Compare the substring with the result
        if (strstr(str, oldWord) == str)
        {
            strcpy(&result[i], newWord);
            i += newWlen;
            str += oldWlen;
        }
        else
            result[i++] = *str++;
    }

    result[i] = '\0';
    return result;
}

int parse_report_line(const char *line, report_message *message)
{
    char *line_float = replaceAll(line, "--", "nan");
    return sscanf(line_float, "{\"timestamp\": %lld, \"out1\": \"%f\", \"out2\": \"%f\", \"out3\": \"%f\"}",
                  &message->timestamp, &message->out1, &message->out2, &message->out3) == 4;
}

int print_report(FILE *file, int interval_ms, int sockfd_out1, int sockfd_out2, int sockfd_out3, udp_socket udp_control_socket, int count)
{
    signal(SIGINT, handle_report_sigint);

    timer_t timer_id;
    setup_timer(&timer_id, interval_ms);

    char buffer1[DATA_SIZE], buffer2[DATA_SIZE], buffer3[DATA_SIZE];
    char report_buffer[REPORT_BUFFER_SIZE];

    int first_call = 1;

    double previous_out3_value = -DBL_MAX; // Negative max as initial value

    while (report_running)
    {
        if (timer_triggered)
        {
            timer_triggered = 0;
            read_tcp_last_line(sockfd_out1, buffer1, sizeof(buffer1));
            read_tcp_last_line(sockfd_out2, buffer2, sizeof(buffer2));
            read_tcp_last_line(sockfd_out3, buffer3, sizeof(buffer3));
            if (first_call)
            {
                first_call = 0;
            }
            else
            {
                if (count > 0)
                    count--;
                if (count == 0)
                    break; // Done
                format_report(report_buffer, sizeof(report_buffer), buffer1, buffer2, buffer3);
                fprintf(file, "%s\n", report_buffer);
            }

            // Send control message only if port defined and out3 value crosses threshold
            if ((strcmp(buffer3, "--") != 0) && (udp_control_socket.sockfd > 0))
            {
                double out3_value = atof(buffer3);
                if ( ((previous_out3_value == -DBL_MAX) || (previous_out3_value < 3.0)) && (out3_value >= 3.0))
                {
                    // TODO missing error handling
                    send_control_message(udp_control_socket, ctrl_msg_o3h_f1hz);
                    send_control_message(udp_control_socket, ctrl_msg_o3h_a8k);
                    previous_out3_value = out3_value;
                }
                else if (((previous_out3_value == -DBL_MAX) || (previous_out3_value >= 3.0)) && out3_value < 3.0)
                {
                    send_control_message(udp_control_socket, ctrl_msg_o3l_f2hz);
                    send_control_message(udp_control_socket, ctrl_msg_o3l_a4k);
                    previous_out3_value = out3_value;
                }
                
            }
        }
        pause(); // Wait for signals
    }
    timer_delete(timer_id);
    return 0;
}
