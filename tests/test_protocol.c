#include "test.h"
#include "../src/protocol.h"

// Function to test if value is in range
int test_range(const char *value, float lower_bound, float upper_bound);

// Open a non-blocking tcp socket
int connect_to_tcp_port(int port);

// Non-blocking read from a TCP socket
int read_tcp_last_line(int sockfd, char *buf, int bufsize);

void close_tcp_socket(int sockfd);
udp_socket open_udp_control_socket(int control_udp_port);
void close_udp_socket(udp_socket udp_control_socket);

// Report printer
int print_report(FILE *file, int interval_ms, int sockfd_out1, int sockfd_out2, int sockfd_out3, udp_socket udp_control_socket, int count);

// Data buffer for assertions
char data_buffer[DATA_SIZE];

// Function to check the control conditions
int check_timing_and_control(const char *buffer, long interval_ms);

int test_protocol_read_tcp_last_line(void)
{
    int sockfd, result;
    
    // New a non-blocking tcp socket for bad port
    sockfd = connect_to_tcp_port(TCP_PORT_BAD);
    
    // Wait for the bad connection to fail or timeout
    sleep(1);

    // Check if the connection failure reveals at read
    result = read_tcp_last_line(sockfd, data_buffer, DATA_SIZE);
    close_tcp_socket(sockfd);
    ASSERT_EQ("try reveal bad socket at read", FAILURE, result);
    
    
    
    // Wait and expect for data to arrive
    sockfd = connect_to_tcp_port(TCP_PORT_OUT1);
    usleep(250L * 1000L);
    result = read_tcp_last_line(sockfd, data_buffer, DATA_SIZE);
    close_tcp_socket(sockfd);
    ASSERT_EQ("out1 read", SUCCESS, result);

    // Valid data range check
    sockfd = connect_to_tcp_port(TCP_PORT_OUT1);
    result = test_range(data_buffer, VALID_OUT1_DATA_MIN, VALID_OUT1_DATA_MAX);
    close_tcp_socket(sockfd);
    ASSERT_EQ("out1 data range", SUCCESS, result);

    // Non-blocking repetitive read to get empty data
    sockfd = connect_to_tcp_port(TCP_PORT_OUT1);
    for (int i = 0;i<1000;i++){
        result = read_tcp_last_line(sockfd, data_buffer, DATA_SIZE);
        result = strcmp("--", data_buffer);
        if (result == SUCCESS)
            break;
    }
    close_tcp_socket(sockfd);
    ASSERT_EQ("out1 empty --", SUCCESS, result);

    long long overhead_ms;
    long long start_time_ms = timestamp_ms();
    sockfd = connect_to_tcp_port(TCP_PORT_OUT1);
    for (long i = 0;i<1000L;i++){
        read_tcp_last_line(sockfd, data_buffer, DATA_SIZE);
        usleep(500L); 
    }
    overhead_ms = timestamp_ms() - start_time_ms - 500LL;
    result = (overhead_ms < 10000LL) ? SUCCESS : FAILURE;
    printf("%lld 1k read overhead: %lld ms\n", timestamp_ms(), overhead_ms);
    close_tcp_socket(sockfd);
    ASSERT_EQ("1k read overhead performance", SUCCESS, result);
    return 0;
}

int test_protocol_json_report(void)
{
    // setup stream capture
    char capture_buffer[REPORT_BUFFER_SIZE];
    FILE *stream = fmemopen(capture_buffer, sizeof(capture_buffer), "w");
    if (stream == NULL)
    {
        perror("fmemopen");
        return 1;
    }
    int sockfd_out1 = connect_to_tcp_port(TCP_PORT_OUT1);
    int sockfd_out2 = connect_to_tcp_port(TCP_PORT_OUT2);
    int sockfd_out3 = connect_to_tcp_port(TCP_PORT_OUT3);
    udp_socket control_udp_socket = open_udp_control_socket(CONTROL_UDP_PORT);
    int result = print_report(stream, REPORT_INTERVAL_100MS, sockfd_out1, sockfd_out2, sockfd_out3, control_udp_socket, REPORT_COUNT_100);
    close_tcp_socket(sockfd_out1);
    close_tcp_socket(sockfd_out2);
    close_tcp_socket(sockfd_out3);
    close_udp_socket(control_udp_socket);
    fclose(stream);
    ASSERT_EQ("report print", SUCCESS, result);
    printf("Captured buffer:\n%s\n", capture_buffer);
    result = check_timing_and_control(capture_buffer, REPORT_INTERVAL_100MS);
    ASSERT_EQ("report timing with out1 control effects", SUCCESS, result);
    return 0;
}

int main(void)
{
    RUN_TEST(test_protocol_read_tcp_last_line);
    RUN_TEST(test_protocol_json_report);
    return 0;
}
