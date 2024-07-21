#include "test.h"
#include "../src/protocol.h"

// Test if client process is terminated by SIGINT
int test_client2(void)
{
    pid_t pid;
    int status;

    pid = fork();

    if (pid < 0)
    {
        perror("fork failed");
        exit(1);
    }
    else if (pid == 0)
    {
        // Forked process
        exit(report_stdout(REPORT_INTERVAL_20MS, CONTROL_ENABLED));
    }
    else
    {
        // Calling parent process
        sleep(1); // Give the forked process some time to start

        kill(pid, SIGINT); // Send SIGINT to the client1 process

        waitpid(pid, &status, 0); // Wait for the child process to terminate

        int result = 0;
        if (WIFSIGNALED(status) && WTERMSIG(status) == SIGINT)
            result = 1;

        ASSERT_EQ("check SIGINT", 0, result);
    }
    return 0;
}

int main(void)
{
    RUN_TEST(test_client2);
    return 0;
}
