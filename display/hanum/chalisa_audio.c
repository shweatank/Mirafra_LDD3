#include <stdio.h>              // Standard I/O functions
#include <stdlib.h>             // For exit(), malloc(), free()
#include <string.h>             // String manipulation functions
#include <unistd.h>             // For read(), write(), close(), sleep()
#include <arpa/inet.h>          // For socket operations and sockaddr_in
#include <fcntl.h>              // For open(), O_RDWR flags
#include <alsa/asoundlib.h>     // ALSA sound library for audio playback
#include <termios.h>            // For UART serial port settings
#include <errno.h>              // Error number macros

// Macro definitions for server port, file name, UART device, and buffer size
#define AUDIO_FILE "chalisa.wav"
#define BUFFER_SIZE 4096

void play_audio(const char *filename) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        perror("fopen");
        return;
    }

    fseek(fp, 44, SEEK_SET);  // Skip WAV file header (44 bytes)

    snd_pcm_t *pcm;  // PCM handle
    // Open default PCM playback device
    if (snd_pcm_open(&pcm, "default", SND_PCM_STREAM_PLAYBACK, 0) < 0) {
        fprintf(stderr, "Error opening PCM device\n");
        fclose(fp);
        return;
    }

    // Set PCM parameters: 16-bit little endian, mono, 44100Hz, 0.5s latency
    if (snd_pcm_set_params(pcm,
        SND_PCM_FORMAT_S16_LE,
        SND_PCM_ACCESS_RW_INTERLEAVED,
        1, 92000, 2, 500000) < 0) {
        fprintf(stderr, "Error setting PCM params\n");
        snd_pcm_close(pcm);
        fclose(fp);
        return;
    }

    short buffer[BUFFER_SIZE];  // Short = 16-bit PCM samples
    int frames;

    // Read audio samples from file and write to PCM device
    while (!feof(fp)) {
        size_t len = fread(buffer, 1, sizeof(buffer), fp);
        frames = snd_pcm_writei(pcm, buffer, len / 2);  // len/2 since each frame = 2 bytes
        if (frames < 0)
            frames = snd_pcm_recover(pcm, frames, 0);
    }

    // Finalize audio playback
    snd_pcm_drain(pcm);
    snd_pcm_close(pcm);
    fclose(fp);
    printf("Playback complete.\n");
}


int main() {
        play_audio(AUDIO_FILE);
	return 0;
}


