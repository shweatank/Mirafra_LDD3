// record_av_parallel.c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

void *record_audio(void *arg) {
    system("./record_audio");  // ALSA recording binary
    return NULL;
}

void *record_video(void *arg) {
    system("./record_video");  // V4L2 video capture binary
    return NULL;
}

int main() {
    pthread_t audio_thread, video_thread;

    pthread_create(&audio_thread, NULL, record_audio, NULL);
    pthread_create(&video_thread, NULL, record_video, NULL);

    pthread_join(audio_thread, NULL);
    pthread_join(video_thread, NULL);

    // Merge both using ffmpeg
    printf("Merging audio and video...\n");
    system("ffmpeg -y -f rawvideo -pixel_format yuyv422 -video_size 640x480 -framerate 30 -i video.yuv "
           "-i audio.wav -c:v libx264 -preset fast -crf 23 -c:a aac output.mp4");

    printf("Recording complete: output.mp4\n");
    return 0;
}

