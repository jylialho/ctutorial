#ifndef TEST_H
#define TEST_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h> // gettimeofday
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

#define SUCCESS 0
#define FAILURE -1

#define ASSERT_EQ(message, expected, actual) \
    if ((expected) != (actual)) { \
        printf("%lld %s:%d: %s: Assertion FAILURE: expected %d, got %d\n", timestamp_ms(), __FILE__, __LINE__, (message), (expected), (actual)); \
        return 1; \
    } else { \
        printf("%lld %s:%d: %s: Assertion SUCCESS: expected %d, got %d\n", timestamp_ms(), __FILE__, __LINE__, (message), (expected), (actual)); \
    } \

#define ASSERT_STR_EQ(message, expected, actual) \
    if (strcmp((expected), (actual)) != 0) { \
        printf("%lld %s:%d: %s: Assertion FAILURE: expected %s, got %s\n", timestamp_ms(), __FILE__, __LINE__, (message), (expected), (actual)); \
        return 1; \
    } else { \
        printf("%lld %s:%d: %s: Assertion SUCCESS: expected %s, got %s\n", timestamp_ms(), __FILE__, __LINE__, (message), (expected), (actual)); \
    } \

#define RUN_TEST(test) \
    do { \
        printf("\nTest %s:\n", #test); \
        int result = test(); \
        if (result == 0) { \
            printf("PASSED\n\n"); \
        } else { \
            printf("FAILED\n\n"); \
        } \
    } while (0)

// Test if a string is a valid float within a range, return 0 if ok, 1 if error
int test_range(const char *value, float lower_bound, float upper_bound) {
    char *endptr;
    float float_value = strtof(value, &endptr);

    // Check if the conversion was successful
    if (endptr == value || *endptr != '\0') {
        return -1; // Error, No float
    }

    // Check if the value is within the range
    if (lower_bound <= float_value && float_value <= upper_bound) {
        return 0; // OK, In range
    } else {
        return -1; // Error, Out of range
    }
}

long long timestamp_ms()
{
    struct timeval te;
    gettimeofday(&te, NULL);                                         // get current time
    long long milliseconds = te.tv_sec * 1000LL + te.tv_usec / 1000; // calculate milliseconds
    return milliseconds;
}

#endif // TEST_H
