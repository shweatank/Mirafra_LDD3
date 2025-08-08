#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INPUT 100

#define LOG_INFO(message)  printf("[INFO]  %s:%d %s() - %s\n", __FILE__, __LINE__, __func__, message)
#define LOG_WARN(message)  printf("[WARN]  %s:%d %s() - %s\n", __FILE__, __LINE__, __func__, message)
#define LOG_ERROR(message) printf("[ERROR] %s:%d %s() - %s\n", __FILE__, __LINE__, __func__, message)

// Macro to log message to file
#define LOG_TO_FILE(user, message) do { \
    FILE *logFile = fopen("log.txt", "a"); \
    if (logFile) { \
        fprintf(logFile, "User: %s\nMessage: %s\nFile: %s\nFunction: %s\nLine: %d\nDate: %s\nTime: %s\n\n", \
                user, message, __FILE__, __func__, __LINE__, __DATE__, __TIME__); \
        fclose(logFile); \
    } else { \
        fprintf(stderr, "ERROR: Could not open log file.\n"); \
    } \
} while(0)

int main() {
    char user[MAX_INPUT];
    char message[MAX_INPUT];

    printf("Enter your name: ");
    fgets(user, MAX_INPUT, stdin);
    user[strcspn(user, "\n")] = '\0';  // Remove newline

    printf("Enter your message: ");
    fgets(message, MAX_INPUT, stdin);
    message[strcspn(message, "\n")] = '\0';  // Remove newline

    LOG_INFO(message);
    LOG_WARN(message);
    LOG_TO_FILE(user, message);
   
    printf("Message logged to 'log.txt'.\n");

    return 0;
}
