#include <stdio.h>      // printf
#include <time.h>       // time, localtime, strftime

int main() {
    time_t raw_time;
    struct tm *timeinfo;
    char buffer[80];

    time(&raw_time);                 // Get current time
    timeinfo = localtime(&raw_time); // Convert to local time

    // Format as "YYYY-MM-DD HH:MM:SS"
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);

    printf("Formatted time: %s\n", buffer);
    return 0;
}

