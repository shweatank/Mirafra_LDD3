#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <alsa/asoundlib.h>

// ---------------- WAV Header ----------------
typedef struct {
    char riff[4];
    unsigned int overall_size;
    char wave[4];
    char fmt_chunk_marker[4];
    unsigned int length_of_fmt;
    unsigned short format_type;
    unsigned short channels;
    unsigned int sample_rate;
    unsigned int byterate;
    unsigned short block_align;
    unsigned short bits_per_sample;
    char data_chunk_header[4];
    unsigned int data_size;
} WAVHeader;

// ---------------- Shared Buffer ----------------
#define BUF_SIZE (64 * 1024)  // 64 KB
char buffer[BUF_SIZE];
size_t bytes_in_buffer = 0;
int playback_done = 0;

pthread_mutex_t lock;
pthread_cond_t cond;

snd_pcm_t *pcm_handle;
int frame_size;

// ---------------- Reader Thread ----------------
void *reader_thread(void *arg) {
    FILE *fp = (FILE *)arg;

    while (!feof(fp)) {
        pthread_mutex_lock(&lock);

        // Wait until buffer is empty
        while (bytes_in_buffer != 0)
            pthread_cond_wait(&cond, &lock);

        // Read new data
        bytes_in_buffer = fread(buffer, 1, BUF_SIZE, fp);

        // Signal playback thread
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&lock);

        if (bytes_in_buffer == 0)
            break;  // EOF
    }

    pthread_mutex_lock(&lock);
    playback_done = 1;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&lock);

    return NULL;
}

// ---------------- Playback Thread ----------------
void *playback_thread(void *arg) {
    while (1) {
        pthread_mutex_lock(&lock);

        // Wait for data
        while (bytes_in_buffer == 0 && !playback_done)
            pthread_cond_wait(&cond, &lock);

        if (bytes_in_buffer == 0 && playback_done) {
            pthread_mutex_unlock(&lock);
            break; // Finished
        }

        // Copy data to local buffer
        size_t bytes_to_play = bytes_in_buffer;
        char temp_buf[BUF_SIZE];
        memcpy(temp_buf, buffer, bytes_to_play);
        bytes_in_buffer = 0;

        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&lock);

        // Write to ALSA device
        int frames = bytes_to_play / frame_size;
        int pcm = snd_pcm_writei(pcm_handle, temp_buf, frames);

        if (pcm == -EPIPE) {
            fprintf(stderr, "XRUN occurred\n");
            snd_pcm_prepare(pcm_handle);
        } else if (pcm < 0) {
            fprintf(stderr, "ALSA write error: %s\n", snd_strerror(pcm));
        }
    }

    snd_pcm_drain(pcm_handle);
    return NULL;
}

// ---------------- Main ----------------
int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <wav file>\n", argv[0]);
        return -1;
    }

    const char *filename = argv[1];
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        perror("File open failed");
        return -1;
    }

    WAVHeader header;
    fread(&header, sizeof(WAVHeader), 1, fp);

    if (strncmp(header.riff, "RIFF", 4) || strncmp(header.wave, "WAVE", 4)) {
        printf("Invalid WAV file!\n");
        fclose(fp);
        return -1;
    }

    printf("Playing: %s\n", filename);
    printf("Channels: %d, Sample Rate: %d Hz, Bits: %d\n",
           header.channels, header.sample_rate, header.bits_per_sample);

    // ALSA setup
    snd_pcm_hw_params_t *params;
    snd_pcm_uframes_t frames;
    unsigned int rate = header.sample_rate;
    int dir, pcm;

    pcm = snd_pcm_open(&pcm_handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
    if (pcm < 0) {
        fprintf(stderr, "ERROR: Can't open PCM device. %s\n", snd_strerror(pcm));
        fclose(fp);
        return -1;
    }

    snd_pcm_hw_params_alloca(&params);
    snd_pcm_hw_params_any(pcm_handle, params);
    snd_pcm_hw_params_set_access(pcm_handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);

    snd_pcm_format_t format = (header.bits_per_sample == 16)
                              ? SND_PCM_FORMAT_S16_LE
                              : SND_PCM_FORMAT_U8;

    snd_pcm_hw_params_set_format(pcm_handle, params, format);
    snd_pcm_hw_params_set_channels(pcm_handle, params, header.channels);
    snd_pcm_hw_params_set_rate_near(pcm_handle, params, &rate, 0);
    snd_pcm_hw_params(pcm_handle, params);
    snd_pcm_hw_params_get_period_size(params, &frames, &dir);

    frame_size = header.channels * (header.bits_per_sample / 8);

    pthread_mutex_init(&lock, NULL);
    pthread_cond_init(&cond, NULL);

    pthread_t reader, player;
    pthread_create(&reader, NULL, reader_thread, fp);
    pthread_create(&player, NULL, playback_thread, NULL);

    pthread_join(reader, NULL);
    pthread_join(player, NULL);

    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&cond);

    fclose(fp);
    snd_pcm_close(pcm_handle);

    printf("Playback finished.\n");
    return 0;
}
