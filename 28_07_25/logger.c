#include <stdio.h>
#include <stdlib.h>

// Log macros with different levels
#define LOG_INFO(msg)  printf("[INFO]  %s:%d %s() - %s\n", __FILE__, __LINE__, __func__, msg)
#define LOG_WARN(msg)  printf("[WARN]  %s:%d %s() - %s\n", __FILE__, __LINE__, __func__, msg)
#define LOG_ERROR(msg) printf("[ERROR] %s:%d %s() - %s\n", __FILE__, __LINE__, __func__, msg)

// Log macro with formatted arguments (like printf)
#define DEBUG_LOG() \
    printf("Log - File: %s | Function: %s | Line: %d | Date: %s | Time: %s\n", \
           __FILE__, __func__, __LINE__, __DATE__, __TIME__)
#define LOG_FMT(fmt, ...) printf("[LOG]   %s:%d %s() - " fmt "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__)

void loadData() {
    LOG_INFO("Loading data from disk...");
    // Simulated condition
    int file_found = 0;
    if (!file_found) {
        LOG_WARN("Data file not found. Using defaults.");
    }
}


int main() {
    LOG_INFO("Program started");
    DEBUG_LOG();
    loadData();


    LOG_INFO("Program finished");
    return 0;
}
