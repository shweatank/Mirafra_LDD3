#include <stdio.h>         // printf
#include <sys/capability.h> // capabilities API

int main() {
    cap_t caps;

    // Get current process capabilities
    caps = cap_get_proc();
    if (caps == NULL) {
        perror("cap_get_proc failed");
        return 1;
    }

    // Print capabilities in text form
    char *caps_text = cap_to_text(caps, NULL);
    printf("Process capabilities: %s\n", caps_text);

    // Free memory
    cap_free(caps);
    cap_free(caps_text);

    return 0;
}

