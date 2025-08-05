#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    FILE *fp;
    int value;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <0|1>\n", argv[0]);
        return 1;
    }

    value = atoi(argv[1]);
    if (value != 0 && value != 1) {
        fprintf(stderr, "Only 0 (stop) or 1 (start) allowed\n");
        return 1;
    }

    fp = fopen("/sys/kernel/blink_sysfs/enabled", "w");
    if (!fp) {
        perror("Failed to open sysfs entry");
        return 1;
    }

    fprintf(fp, "%d", value);
    fclose(fp);

    printf("LED blink set to %d\n", value);
    return 0;
}

