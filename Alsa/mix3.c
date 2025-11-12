#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdatomic.h>
#include <unistd.h>
#include <pthread.h>

atomic_int stop_flag = 0;

// Signal handler to stop program
void handle_signal(int sig) {
    printf("\nReceived signal %d, stopping...\n", sig);
    stop_flag = 1;
}

// Thread: FFmpeg receiver and splitter
void* ffmpeg_thread(void* arg) {
    // Bash command to receive TCP stream, split to video & audio programs
    char *cmd ="bash -c 'ffmpeg -probesize 5000000 -analyzeduration 10000000 -fflags nobuffer "
                "-i tcp://192.168.0.165:1234 -c copy -f mpegts - "
                "| tee >(./mp4_fps /dev/stdin) > /dev/null'";/*>(./audio_mp4 /dev/stdin) >/dev/null'";*/

    printf("Starting FFmpeg receiver...\n");

    if (fork() == 0) {
        execl("/bin/bash", "bash", "-c", cmd, (char*)NULL);
        perror("execl ffmpeg");
        _exit(1);
    }

    // Wait until stop_flag is set
    while (!stop_flag) sleep(1);

    printf("FFmpeg thread exiting...\n");
    return NULL;
}

int main() {
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    pthread_t t_ffmpeg;

    // Start FFmpeg thread
    pthread_create(&t_ffmpeg, NULL, ffmpeg_thread, NULL);

    // Wait for FFmpeg thread
    pthread_join(t_ffmpeg, NULL);

    printf("Program terminated.\n");
    return 0;
}
