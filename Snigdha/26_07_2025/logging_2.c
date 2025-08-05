#include <stdio.h>
#include <time.h>

// Macro that accepts a custom message
#define DEBUG_LOG(msg) do { \
    time_t now = time(NULL); \
    struct tm *tm_info = localtime(&now); \
    char rt_time_buf[26]; \
    strftime(rt_time_buf, sizeof(rt_time_buf), "%Y-%m-%d %H:%M:%S", tm_info); \
    FILE *fp = fopen("debug.log", "a"); \
    if (fp) { \
        fprintf(fp, "[%s] %s - File: %s | Function: %s | Line: %d\n", \
                rt_time_buf, msg, __FILE__, __func__, __LINE__); \
        fprintf(fp, "           Compile Time: %s %s\n\n", __DATE__, __TIME__); \
        fclose(fp); \
    } \
    printf("[%s] %s - File: %s | Function: %s | Line: %d\n", \
           rt_time_buf, msg, __FILE__, __func__, __LINE__); \
    printf("           Compile Time: %s %s\n\n", __DATE__, __TIME__); \
} while(0)

void testFunction() {
    DEBUG_LOG("Inside Functio");
}

int main() {
    DEBUG_LOG("main");
    testFunction();
    return 0;
}

