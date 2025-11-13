// alert_player.c
#include <alsa/asoundlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

static pthread_t player_thread;
static int playing = 0;

void *play_loop(void *arg) {
    while (playing) {
        system("aplay alert.wav > /dev/null 2>&1");
        sleep(1);
    }
    return NULL;
}

void play_alert() {
    if (playing) return;
    playing = 1;
    pthread_create(&player_thread, NULL, play_loop, NULL);
}

void stop_alert() {
    if (!playing) return;
    playing = 0;
    pthread_cancel(player_thread);
    pthread_join(player_thread, NULL);
}

