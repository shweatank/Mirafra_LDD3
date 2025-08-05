#include <stdio.h>
#include <time.h>

// Macro to print compile-time and run-time info
#define DEBUG_LOG() do { \
    time_t now = time(NULL); \
    struct tm *tm_info = localtime(&now); \
    char rt_time_buf[26]; \
    strftime(rt_time_buf, sizeof(rt_time_buf), "%Y-%m-%d %H:%M:%S", tm_info); \
    printf("Log - File: %s | Function: %s | Line: %d\n", \
           __FILE__, __func__, __LINE__); \
    printf("      Compile Time: %s %s | Run Time: %s\n", \
           __DATE__, __TIME__, rt_time_buf); \
} while(0)

void testFunction() {
    DEBUG_LOG();
}

int main() {
    DEBUG_LOG();
    testFunction();
    return 0;
}

