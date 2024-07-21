#include "test.h"
#include "../src/protocol.h"

// Function to test if value is in range
int test_range(const char *value, float lower_bound, float upper_bound);

// Open a non-blocking tcp socket
int connect_to_tcp_port(int port);

// Non-blocking read from a TCP socket
int read_tcp_last_line(int sockfd, char *buf, int bufsize);

void close_tcp_socket(int sockfd);

// Data buffer for assertions
char data_buffer[DATA_SIZE];

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
