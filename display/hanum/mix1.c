#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdatomic.h>

atomic_int stop_flag = 0;

// Signal handler to stop threads
void handle_signal(int sig) {
    printf("\nReceived signal %d, stopping...\n", sig);
    stop_flag = 1;
}

// Thread: Live streaming via FFmpeg (video + audio)
void* thread_ffmpeg(void* arg) {
    char *argv[] = {
        "ffmpeg",
        "-f", "rawvideo",
        "-pix_fmt", "yuyv422",
        "-s", "1280x720",
        "-r", "10",
        "-i", "/tmp/video_pipe",
        "-f", "s16le",
        "-ar", "44100",
        "-ac", "2",
        "-i", "/tmp/audio_pipe",
        "-c:v", "libx264",
        "-preset", "ultrafast",
        "-crf", "23",
        "-c:a", "aac",
        "-b:a", "128k",
        "-f", "mpegts",
        "tcp://0.0.0.0:1234?listen=1", // live stream TCP
        NULL
    };

    pid_t pid = fork();
    if (pid == 0) {
        execvp(argv[0], argv);
        perror("execvp ffmpeg");
        _exit(1);
    } else if (pid > 0) {
        // Wait until stop_flag is set
        while (!stop_flag) sleep(1);

        // Stop FFmpeg gracefully
        printf("Stopping FFmpeg...\n");
        kill(pid, SIGINT);
        waitpid(pid, NULL, 0);
    } else {
        perror("fork failed");
    }

    return NULL;
}

// Thread: Video player connects to TCP
void* thread_video(void* arg) {
    char *argv[] = {
        "./mp4_fps",
        "tcp://127.0.0.1:1234", // connect to live stream
        NULL
    };

    execvp(argv[0], argv);
    perror("execvp video");
    return NULL;
}

// Thread: Audio player connects to TCP
void* thread_audio(void* arg) {
    char *argv[] = {
        "./audio_mp4",
        "tcp://127.0.0.1:1234", // connect to live stream
        NULL
    };

    execvp(argv[0], argv);
    perror("execvp audio");
    return NULL;
}

int main() {
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    pthread_t t_ffmpeg, t_video, t_audio;

    // Start FFmpeg live stream
    pthread_create(&t_ffmpeg, NULL, thread_ffmpeg, NULL);
    sleep(2); // give FFmpeg time to start TCP listener

    // Start players
    pthread_create(&t_video, NULL, thread_video, NULL);
    pthread_create(&t_audio, NULL, thread_audio, NULL);

    // Wait for threads
    pthread_join(t_ffmpeg, NULL);
    pthread_join(t_video, NULL);
    pthread_join(t_audio, NULL);

    printf("Live streaming stopped.\n");
    return 0;
}

