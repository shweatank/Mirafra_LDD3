#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>

#define PROC_FILE "/proc/mykbd"
#define BUFFER_SIZE 4096

static volatile int running = 1;

// Signal handler for graceful shutdown
void signal_handler(int sig)
{
    running = 0;
    printf("\nShutting down keyboard monitor...\n");
}

// Function to clear screen
void clear_screen()
{
    printf("\033[2J\033[H");
}

// Function to get current time string
char* get_current_time()
{
    static char time_str[64];
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm_info);
    return time_str;
}

// Function to read and display keyboard events
/*void display_keyboard_events()
{
    FILE *file;
    char line[256];
    int line_count = 0;

    file = fopen(PROC_FILE, "r");
    if (!file) {
        perror("Failed to open " PROC_FILE);
        return;
    }

    printf("=== Smart Keyboard Activity Logger ===\n");
    printf("Time: %s\n", get_current_time());
    printf("Reading from: %s\n\n", PROC_FILE);

    // Skip first two header lines from kernel's /proc/mykbd
    fgets(line, sizeof(line), file);  // "Keyboard Activity Log"
    fgets(line, sizeof(line), file);  // "Timestamp\tScancode\tCPU Idle\tI/O Wait"

    // Print neat formatted table header
    printf("+--------------+--------+------------+-----------+\n");
    printf("|  Timestamp   |  Key   | CPU Idle   | I/O Wait  |\n");
    printf("+--------------+--------+------------+-----------+\n");

    while (fgets(line, sizeof(line), file)) {
        unsigned long timestamp, idle, iowait;
        //char key[32] = "";
          char key[32];
        // parse line with tabs or spaces
        if (sscanf(line, "%lu %10s %lu %lu", &timestamp, key, &idle, &iowait) == 4) {
            // Print a row in formatted table
          if (strcmp(key, " ") == 0) {
    strcpy(key, "SPACE");
}
printf("| %-12lu | %-6s | %-10lu | %-9lu |\n", timestamp, key, idle, iowait);

	    // printf("| %-12lu | %-6s | %-10lu | %-9lu |\n", timestamp, key, idle, iowait);
        } else {
            // If key is empty (space or non-printable), use "-"
            if (sscanf(line, "%lu %lu %lu", &timestamp, &idle, &iowait) == 3) {
                printf("| %-12lu | %-6s | %-10lu | %-9lu |\n", timestamp, "-", idle, iowait);
            }
        }

        line_count++;
        if (line_count > 100) {
            printf("| ... (truncated, showing last 50 events)       |\n");
            break;
        }
    }

    printf("+--------------+--------+------------+-----------+\n");

    fclose(file);
}*/
void display_keyboard_events()
{
    FILE *file;
    char line[256];
    int line_count = 0;

    file = fopen(PROC_FILE, "r");
    if (!file) {
        perror("Failed to open " PROC_FILE);
        return;
    }

    printf("=== Smart Keyboard Activity Logger ===\n");
    printf("Time: %s\n", get_current_time());
    printf("Reading from: %s\n\n", PROC_FILE);

    // Skip first two header lines
    fgets(line, sizeof(line), file);
    fgets(line, sizeof(line), file);

    printf("+--------------+--------+------------+-----------+\n");
    printf("|  Timestamp   |  Key    | CPU Idle | I/O Wait |\n");
    printf("+--------------+--------+------------+-----------+\n");

    while (fgets(line, sizeof(line), file)) {
        char *token;
        unsigned long timestamp = 0, idle = 0, iowait = 0;
        char key[32] = "";

        token = strtok(line, " \t\n");
        if (!token)
            continue;  // this continue is now inside the while loop

        timestamp = strtoul(token, NULL, 10);

        token = strtok(NULL, " \t\n");
        if (token)
            strncpy(key, token, sizeof(key));
        else
            strcpy(key, "-");

        token = strtok(NULL, " \t\n");
        if (token)
            idle = strtoul(token, NULL, 10);

        token = strtok(NULL, " \t\n");
        if (token)
            iowait = strtoul(token, NULL, 10);

        // Handle space and unknown key
        if (strcmp(key, " ") == 0)
            strcpy(key, "SP");
        else if (strcmp(key, "-") == 0)
            strcpy(key, "UN");

        printf("| %-12lu | %5s\t | %5lu\t | %5lu\t |\n", timestamp, key, idle, iowait);

        line_count++;
        if (line_count >= 50) {
            printf("| ... (truncated, showing last 50 events)       |\n");
            break;
        }
    }

    printf("+--------------+--------+------------+-----------+\n");

    fclose(file);
}




// Function to parse CPU statistics from /proc/stat
void display_system_stats()
{
    FILE *file;
    char buffer[256];
    unsigned long user, nice, system, idle, iowait, irq, softirq;
    unsigned long total, idle_total;
    double cpu_usage, io_wait_percent;
    
    file = fopen("/proc/stat", "r");
    if (!file) {
        perror("Failed to open /proc/stat");
        return;
    }
    
    if (fgets(buffer, sizeof(buffer), file)) {
        sscanf(buffer, "cpu %lu %lu %lu %lu %lu %lu %lu", 
               &user, &nice, &system, &idle, &iowait, &irq, &softirq);
        
        total = user + nice + system + idle + iowait + irq + softirq;
        idle_total = idle + iowait;
        
        cpu_usage = 100.0 * (total - idle_total) / total;
        io_wait_percent = 100.0 * iowait / total;
        
        printf("\n=== System Statistics ===\n");
        printf("CPU Usage: %.2f%%\n", cpu_usage);
        printf("I/O Wait: %.2f%%\n", io_wait_percent);
        printf("CPU Idle: %.2f%%\n", 100.0 * idle / total);
    }
    
    fclose(file);
}

// Function to check if kernel module is loaded
int check_module_loaded()
{
    FILE *file;
    char line[256];
    int found = 0;
    
    file = fopen("/proc/modules", "r");
    if (!file) {
        perror("Failed to open /proc/modules");
        return 0;
    }
    
    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, "kbd")) {
            found = 1;
            break;
        }
    }
    
    fclose(file);
    return found;
}

// Function to display help
void display_help()
{
    printf("Smart Keyboard Activity Logger - User Space Monitor\n");
    printf("==================================================\n\n");
    printf("Usage: ./kbd_reader [options]\n\n");
    printf("Options:\n");
    printf("  -h, --help     Display this help message\n");
    printf("  -c, --continuous  Continuous monitoring mode\n");
    printf("  -o, --once     Display events once and exit\n");
    printf("  -s, --stats    Display system statistics\n\n");
    printf("Controls:\n");
    printf("  Ctrl+C         Exit the program\n\n");
    printf("Requirements:\n");
    printf("  - Kernel module 'keyboard_monitor' must be loaded\n");
    printf("  - /proc/mykbd interface must be available\n\n");
}

int main(int argc, char *argv[])
{
    int continuous = 0;
    int show_stats = 0;
    int show_once = 0;
    
    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            display_help();
            return 0;
        } else if (strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "--continuous") == 0) {
            continuous = 1;
        } else if (strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--stats") == 0) {
            show_stats = 1;
        } else if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--once") == 0) {
            show_once = 1;
        }
    }
    
    // Set up signal handler
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    // Check if kernel module is loaded
    if (!check_module_loaded()) {
        printf("Error: Kernel module 'keyboard_monitor' is not loaded.\n");
        printf("Please load the module first: sudo insmod keyboard_monitor.ko\n");
        return 1;
    }
    
    // Check if proc file exists
    if (access(PROC_FILE, R_OK) != 0) {
        printf("Error: Cannot access %s\n", PROC_FILE);
        printf("Make sure the kernel module is properly loaded.\n");
        return 1;
    }
    
    printf("Keyboard Monitor User Space Program\n");
    printf("===================================\n\n");
    
    if (show_once) {
        // Display events once and exit
        display_keyboard_events();
        if (show_stats) {
            display_system_stats();
        }
        return 0;
    }
    
    if (continuous) {
        // Continuous monitoring mode
        printf("Starting continuous monitoring mode...\n");
        printf("Press Ctrl+C to exit\n\n");
        
        while (running) {
            clear_screen();
            display_keyboard_events();
            
            if (show_stats) {
                display_system_stats();
            }
            
            printf("\n[Press Ctrl+C to exit]\n");
            sleep(2);  // Update every 2 seconds
        }
    } else {
        // Default: single display
        display_keyboard_events();
        if (show_stats) {
            display_system_stats();
        }
    }
    
    return 0;
}
