#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LOG_LEN 512

#define DEBUG_LOG_TO_FILE(msg, file_path) do {                          \
    char log_buffer[MAX_LOG_LEN] = {0};                                 \
    strcat(log_buffer, "[MSG] "); strcat(log_buffer, msg);              \
    strcat(log_buffer, " | File: "); strcat(log_buffer, __FILE__);      \
    strcat(log_buffer, " | Func: "); strcat(log_buffer, __func__);      \
    char line_str[10]; sprintf(line_str, "%d", __LINE__);               \
    strcat(log_buffer, " | Line: "); strcat(log_buffer, line_str);      \
    strcat(log_buffer, " | Date: "); strcat(log_buffer, __DATE__);      \
    strcat(log_buffer, " | Time: "); strcat(log_buffer, __TIME__);      \
    strcat(log_buffer, "\n");                                           \
    FILE *fp = fopen(file_path, "a");                                   \
    if (fp) {                                                           \
        fputs(log_buffer, fp);                                          \
        fclose(fp);                                                     \
    } else {                                                            \
        perror("fopen");                                                \
    }                                                                   \
} while(0)

void test_log() {

    DEBUG_LOG_TO_FILE("Sending hello..", "log.txt");
}

int main() {
    DEBUG_LOG_TO_FILE("Main started", "log.txt");
    test_log();
    DEBUG_LOG_TO_FILE("Main finished", "log.txt");
    return 0;
}
