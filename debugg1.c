#include <stdio.h>

// Define a logging macro
#define LOG_MESSAGE(format, ...) \
    fprintf(stdout, "[%s %s] %s:%d - " format "\n", __DATE__, __TIME__, __FILE__, __LINE__, ##__VA_ARGS__)

int main() {
    int value = 10;
    LOG_MESSAGE("This is a simple log message.");
    LOG_MESSAGE("The value of 'value' is: %d", value);
    return 0;
}
