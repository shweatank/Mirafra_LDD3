#include <stdio.h>

// Macro to log basic debug info
#define DEBUG_LOG() \
    printf("Log - File: %s | Function: %s | Line: %d | Date: %s | Time: %s\n", \
           __FILE__, __func__, __LINE__, __DATE__, __TIME__)

void testFunction() {
    DEBUG_LOG();  // This will print file, function, line, date, time
}

int main() {
    DEBUG_LOG();  // Called from main
    testFunction();  // Called from another function
    return 0;
}
