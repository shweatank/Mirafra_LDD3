#include <stdio.h>
#include <stdlib.h>

#define AUDIO_FILE "audio.wav"
#define VIDEO_FILE "video.avi"
#define AV_FILE    "output.mp4"
#define PCM_DEVICE "hw:0,0"         // Updated for laptop internal mic
#define VIDEO_DEVICE "/dev/video0"
#define RECORD_DURATION 5           // seconds

void record_audio_play() {
    char cmd[512];

    printf("\nRecording audio for %d seconds...\n", RECORD_DURATION);
//    snprintf(cmd, sizeof(cmd),
//             "arecord -D %s -f cd -t wav -d %d %s",
  //           PCM_DEVICE, RECORD_DURATION, AUDIO_FILE);
  snprintf(cmd, sizeof(cmd),
         "arecord -D plughw:0,0 -r 48000 -f S16_LE -c 2 -t wav -d %d %s",
         RECORD_DURATION, AUDIO_FILE);

    printf("Running: %s\n", cmd);  // Debug print
    system(cmd);

    printf("Playing recorded audio...\n");
    snprintf(cmd, sizeof(cmd), "aplay %s", AUDIO_FILE);
    system(cmd);
}

void capture_video_play() {
    char cmd[512];

    printf("\nCapturing video for %d seconds...\n", RECORD_DURATION);
    snprintf(cmd, sizeof(cmd),
             "ffmpeg -f v4l2 -framerate 25 -video_size 640x480 -t %d -i %s -c:v libx264 -y %s",
             RECORD_DURATION, VIDEO_DEVICE, VIDEO_FILE);
    printf("Running: %s\n", cmd);  // Debug print
    system(cmd);

    printf("Playing recorded video...\n");
    snprintf(cmd, sizeof(cmd), "ffplay -autoexit -loglevel quiet %s", VIDEO_FILE);
    system(cmd);
}

void av_capture_sync_play() {
    char cmd[1024];

    printf("\nCapturing audio & video for %d seconds with sync...\n", RECORD_DURATION);
    snprintf(cmd, sizeof(cmd),
        "ffmpeg -f v4l2 -thread_queue_size 512 -i %s "
        "-f alsa -thread_queue_size 512 -i %s "
        "-t %d -c:v libx264 -c:a aac -strict experimental -shortest -y %s",
        VIDEO_DEVICE, PCM_DEVICE, RECORD_DURATION, AV_FILE);
    printf("Running: %s\n", cmd);  // Debug print
    system(cmd);

    printf("Playing synced audio-video...\n");
    snprintf(cmd, sizeof(cmd), "ffplay -autoexit -loglevel quiet %s", AV_FILE);
    system(cmd);
}

int main() {
    int choice;

    while (1) {
        printf("\n====== AV MENU ======\n");
        printf("1. Record Audio & Play (Laptop Mic + Speaker)\n");
        printf("2. Capture Video & Play (Laptop Webcam)\n");
        printf("3. Capture Audio + Video with Sync & Play\n");
        printf("0. Exit\n");
        printf("=====================\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar(); // Clear newline

        switch (choice) {
            case 1:
                record_audio_play();
                break;
            case 2:
                capture_video_play();
                break;
            case 3:
                av_capture_sync_play();
                break;
            case 0:
                printf("Exiting...\n");
                return 0;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }

    return 0;
}

