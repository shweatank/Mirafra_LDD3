
//main.c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

void *audio_thread(void *arg)
{
    // Run audio playback program
    system("./play_audio");
    return NULL;
}

void *video_thread(void *arg)
{
    // Run video playback program (frames from ./frames directory)
    system("./lcd_player");
    return NULL;
}

int main() 
{
    pthread_t t1, t2;

    // Start both threads
    pthread_create(&t1, NULL, audio_thread, NULL);
    pthread_create(&t2, NULL, video_thread, NULL);

    // Wait for both to finish
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("Audio + Video playback finished.\n");
    return 0;
}

