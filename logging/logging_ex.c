#include <stdio.h>
#include <time.h>
#include<string.h>
void log_message(const char *message) {
    FILE *logfile = fopen("log.txt", "a");  // Open file in append mode
    if (logfile == NULL) {
        perror("Failed to open log file");
        return;
    }

    time_t now = time(NULL);                 // Get current time
    char *timestamp = ctime(&now);           // Convert to string

    timestamp[strcspn(timestamp, "\n")] = 0; // Remove newline from timestamp

    fprintf(logfile, "[%s] %s\n", timestamp, message); // Write log
    fclose(logfile);                        // Close file
}

int main() {
    log_message("Program started");
    log_message("Performing some task...");
    log_message("Program ended");

    printf("Logs written to log.txt\n");
    return 0;
}

