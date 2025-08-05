#include <stdio.h>
#include <stdlib.h>

FILE *log_file = NULL;

#define LOG_MESSAGE(format, ...) \
    do { \
        if (log_file) { \
            fprintf(log_file, "[%s %s] %s:%s:%d - " format "\n", __DATE__, __TIME__,__func__, __FILE__, __LINE__, ##__VA_ARGS__); \
            fflush(log_file); \
        } \
    } while (0)

int main() {
    log_file = fopen("log.txt", "a");
    if (!log_file) {
        perror("Unable to open log file");
        return EXIT_FAILURE;
    }

    int value = 10;
    LOG_MESSAGE("This is a simple log message.");
    LOG_MESSAGE("The value of 'value' is: %d", value);

    fclose(log_file);
    return 0;
}

