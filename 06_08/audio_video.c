// play_audio_video.c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

// Thread function to play audio
void* play_audio(void* arg) {
    system("ffplay -nodisp -autoexit recorded.wav");
    return NULL;
}

// Thread function to play video
void* play_video(void* arg) {
    system("ffplay -an -autoexit -f rawvideo -pixel_format yuv420p -video_size 1280x720 video.yuv");

    return NULL;
}

int main() {
    pthread_t audio_thread, video_thread;

    // Create threads
    pthread_create(&audio_thread, NULL, play_audio, NULL);
    pthread_create(&video_thread, NULL, play_video, NULL);

    // Wait for both threads to finish
    pthread_join(audio_thread, NULL);
    pthread_join(video_thread, NULL);

    printf("Playback completed.\n");
    return 0;
}

