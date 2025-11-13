#include <stdio.h>
#include <stdlib.h>

int main() {
    const char *filename = "test.wav";
    int duration = 5;  // seconds

    // Record audio using arecord
    printf("Recording audio for %d seconds...\n", duration);
    char record_cmd[128];
    snprintf(record_cmd, sizeof(record_cmd),
             "arecord -f cd -t wav -d %d -r 44100 -c 2 -D plughw:1,0 %s", duration, filename);

    if (system(record_cmd) != 0) {
        perror("Recording failed");
        return 1;
    }

    printf("Recording done. Playing back...\n");

    // Play audio using aplay
    char play_cmd[128];
    snprintf(play_cmd, sizeof(play_cmd),
             "aplay -D plughw:0,0 %s", filename);

    if (system(play_cmd) != 0) {
        perror("Playback failed");
        return 1;
    }

    printf("Playback finished.\n");

    return 0;
}

