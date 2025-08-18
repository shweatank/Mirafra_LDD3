/*
 * av_sync_capture_play.c
 *
 * Menu options:
 * 1. Capture audio (WAV) and video (YUV) in sync
 * 2. Play captured audio and video in sync
 * 3. Exit
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>
#include <fcntl.h>
#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <alsa/asoundlib.h>
#include <SDL2/SDL.h>

#define AUDIO_FILE "audio.wav"
#define VIDEO_FILE "video.yuv"
#define WIDTH 640
#define HEIGHT 480
#define AUDIO_DURATION 5 // seconds

volatile int capturing = 1;

void *capture_audio(void *arg) {
    FILE *audio_fp = fopen(AUDIO_FILE, "wb");
    snd_pcm_t *capture_handle;
    snd_pcm_hw_params_t *hw_params;
    int err;
    unsigned int rate = 44100;
    snd_pcm_format_t format = SND_PCM_FORMAT_S16_LE;
    int channels = 2;
    int seconds = AUDIO_DURATION;
    int frames_per_buffer = 1024;

    err = snd_pcm_open(&capture_handle, "default", SND_PCM_STREAM_CAPTURE, 0);
    if (err < 0) {
        perror("snd_pcm_open");
        pthread_exit(NULL);
    }

    snd_pcm_hw_params_malloc(&hw_params);
    snd_pcm_hw_params_any(capture_handle, hw_params);
    snd_pcm_hw_params_set_access(capture_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(capture_handle, hw_params, format);
    snd_pcm_hw_params_set_rate_near(capture_handle, hw_params, &rate, 0);
    snd_pcm_hw_params_set_channels(capture_handle, hw_params, channels);
    snd_pcm_hw_params(capture_handle, hw_params);
    snd_pcm_hw_params_free(hw_params);
    snd_pcm_prepare(capture_handle);

    int buffer_size = frames_per_buffer * channels * 2;
    char *buffer = (char *)malloc(buffer_size);
    int total_frames = rate * seconds;

    // Write WAV header placeholder
    fseek(audio_fp, 44, SEEK_SET);

    for (int i = 0; i < total_frames / frames_per_buffer && capturing; i++) {
        snd_pcm_readi(capture_handle, buffer, frames_per_buffer);
        fwrite(buffer, 1, buffer_size, audio_fp);
    }

    // Write actual WAV header
    int data_chunk_size = total_frames * channels * 2;
    fseek(audio_fp, 0, SEEK_SET);
    fwrite("RIFF", 1, 4, audio_fp);
    int file_size = data_chunk_size + 36;
    fwrite(&file_size, 4, 1, audio_fp);
    fwrite("WAVEfmt ", 1, 8, audio_fp);
    int fmt_size = 16;
    short audio_format = 1;
    fwrite(&fmt_size, 4, 1, audio_fp);
    fwrite(&audio_format, 2, 1, audio_fp);
    fwrite(&channels, 2, 1, audio_fp);
    fwrite(&rate, 4, 1, audio_fp);
    int byte_rate = rate * channels * 2;
    fwrite(&byte_rate, 4, 1, audio_fp);
    short block_align = channels * 2;
    fwrite(&block_align, 2, 1, audio_fp);
    short bits_per_sample = 16;
    fwrite(&bits_per_sample, 2, 1, audio_fp);
    fwrite("data", 1, 4, audio_fp);
    fwrite(&data_chunk_size, 4, 1, audio_fp);

    fclose(audio_fp);
    snd_pcm_drain(capture_handle);
    snd_pcm_close(capture_handle);
    free(buffer);
    pthread_exit(NULL);
}

void *capture_video(void *arg) {
    int fd = open("/dev/video0", O_RDWR);
    if (fd < 0) {
        perror("open video device");
        pthread_exit(NULL);
    }

    struct v4l2_format fmt;
    memset(&fmt, 0, sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = WIDTH;
    fmt.fmt.pix.height = HEIGHT;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;

    ioctl(fd, VIDIOC_S_FMT, &fmt);

    int frame_size = WIDTH * HEIGHT * 2;
    char *buffer = malloc(frame_size);
    FILE *video_fp = fopen(VIDEO_FILE, "wb");

    for (int i = 0; i < AUDIO_DURATION * 30 && capturing; i++) {
        read(fd, buffer, frame_size);
        fwrite(buffer, 1, frame_size, video_fp);
        usleep(33333); // ~30fps
    }

    fclose(video_fp);
    close(fd);
    free(buffer);
    pthread_exit(NULL);
}

void play_audio_video() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

    // Setup video window
    SDL_Window *win = SDL_CreateWindow("Playback", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, 0);
    SDL_Texture *tex = SDL_CreateTexture(ren, SDL_PIXELFORMAT_YUY2, SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);

    // Setup audio
    SDL_AudioSpec wav_spec;
    Uint32 wav_length;
    Uint8 *wav_buffer;
    SDL_LoadWAV(AUDIO_FILE, &wav_spec, &wav_buffer, &wav_length);
    SDL_OpenAudio(&wav_spec, NULL);
    SDL_PauseAudio(0);

    // Play video frames
    FILE *video_fp = fopen(VIDEO_FILE, "rb");
    int frame_size = WIDTH * HEIGHT * 2;
    char *frame = malloc(frame_size);

    for (int i = 0; i < AUDIO_DURATION * 30; i++) {
        fread(frame, 1, frame_size, video_fp);
        SDL_UpdateTexture(tex, NULL, frame, WIDTH * 2);
        SDL_RenderClear(ren);
        SDL_RenderCopy(ren, tex, NULL, NULL);
        SDL_RenderPresent(ren);
        SDL_Delay(33);
    }

    SDL_CloseAudio();
    SDL_FreeWAV(wav_buffer);
    fclose(video_fp);
    SDL_DestroyTexture(tex);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
}

int main() {
    int choice;
    while (1) {
        printf("\nMenu:\n");
        printf("1. Capture audio + video (in sync)\n");
        printf("2. Play audio + video (in sync)\n");
        printf("3. Exit\n> ");
        scanf("%d", &choice);

        if (choice == 1) {
            capturing = 1;
            pthread_t audio_thread, video_thread;
            pthread_create(&audio_thread, NULL, capture_audio, NULL);
            pthread_create(&video_thread, NULL, capture_video, NULL);
            pthread_join(audio_thread, NULL);
            pthread_join(video_thread, NULL);
            printf("Capture complete.\n");
        } else if (choice == 2) {
            printf("Playing audio and video in sync...\n");
            play_audio_video();
        } else if (choice == 3) {
            break;
        } else {
            printf("Invalid option. Try again.\n");
        }
    }
    return 0;
}
