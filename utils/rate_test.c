#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <signal.h>

#define SERVER_PORT 4001
#define BUFFER_SIZE 1024

int sock;

void error_handling(const char *message)
{
    perror(message);
    exit(1);
}

double get_time_diff(struct timeval start, struct timeval end)
{
    return (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
}

void handle_signal(int sig)
{
    if (sock != -1)
    {
        close(sock);
    }
    printf("Terminated by signal %d\n", sig);
    exit(0);
}

int main()
{
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    int str_len;
    struct timeval start_time, end_time;
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);
    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        error_handling("socket() error");
    }
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(SERVER_PORT);

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        error_handling("connect() error");
    }

    gettimeofday(&start_time, NULL);
    int line_count = 0;

    while ((str_len = read(sock, buffer, BUFFER_SIZE - 1)) != 0)
    {
        if (str_len == -1)
        {
            error_handling("read() error");
        }

        buffer[str_len] = 0;
        char *line = strtok(buffer, "\n");
        while (line != NULL)
        {
            gettimeofday(&end_time, NULL);
            double time_diff = get_time_diff(start_time, end_time);
            printf("Line %d: %f seconds\n", line_count++, time_diff);
            start_time = end_time;

            line = strtok(NULL, "\n");
        }
    }

    close(sock);
    return 0;
}
