#include <stdio.h>
#include <stdlib.h>

// Log macros with different levels
#define LOG_INFO(msg)  printf("[INFO]  %s:%d %s() - %s\n", __FILE__, __LINE__, __func__, msg)
#define LOG_WARN(msg)  printf("[WARN]  %s:%d %s() - %s\n", __FILE__, __LINE__, __func__, msg)
#define LOG_ERROR(msg) printf("[ERROR] %s:%d %s() - %s\n", __FILE__, __LINE__, __func__, msg)

// Log macro with formatted arguments (like printf)
#define LOG_FMT(fmt, ...) printf("[LOG]   %s:%d %s() - " fmt "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__)

void loadData() {
    LOG_INFO("Loading data from disk...");
    // Simulated condition
    int file_found = 0;
    if (!file_found) {
        LOG_WARN("Data file not found. Using defaults.");
    }
}

void processData(int x) {
    LOG_FMT("Processing data: x = %d", x);
    if (x < 0) {
        LOG_ERROR("Invalid input: x must be >= 0");
    }
}

int main() {
    LOG_INFO("Program started");

    loadData();

    processData(10);
    processData(-5);  // This will trigger error log

    LOG_INFO("Program finished");
    return 0;
}
