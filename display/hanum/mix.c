#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

// global filename
char str[256];

void *play_video(void *arg) {
    char *argv[] = {"./mp4_fps", str, NULL};
    if (fork() == 0) {
        execvp(argv[0], argv);
        perror("execvp video"); // if exec fails
        _exit(1);
    }
    wait(NULL); // wait for video child to finish
    return NULL;
}

void *play_audio(void *arg) {
    char *argv[] = {"./audio_mp4", str, NULL};
    if (fork() == 0) {
        execvp(argv[0], argv);
        perror("execvp audio");
        _exit(1);
    }
    wait(NULL); // wait for audio child
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <video_file>\n", argv[0]);
        return 1;
    }

    strncpy(str, argv[1], sizeof(str)-1);
    str[sizeof(str)-1] = '\0';

    pthread_t audio_thread, video_thread;

    pthread_create(&audio_thread, NULL, play_audio, NULL);
    pthread_create(&video_thread, NULL, play_video, NULL);

    pthread_join(audio_thread, NULL);
    pthread_join(video_thread, NULL);

    return 0;
}

