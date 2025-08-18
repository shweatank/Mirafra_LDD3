#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <alsa/asoundlib.h>

#define VIDEO_DEVICE "/dev/video0"
#define AUDIO_DEVICE "default"
#define WIDTH 640
#define HEIGHT 480
#define DURATION_SECONDS 5
#define FRAME_RATE 10  // 10 fps
#define AUDIO_RATE 44100
#define AUDIO_CHANNELS 1
#define AUDIO_FORMAT SND_PCM_FORMAT_S16_LE

int running = 1;

void* record_video(void* arg) {
    int fd = open(VIDEO_DEVICE, O_RDWR);
    if (fd < 0) { perror("Video open"); return NULL; }

    struct v4l2_format fmt = {0};
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = WIDTH;
    fmt.fmt.pix.height = HEIGHT;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    fmt.fmt.pix.field = V4L2_FIELD_NONE;
    if (ioctl(fd, VIDIOC_S_FMT, &fmt) < 0) { perror("Set format"); close(fd); return NULL; }

    struct v4l2_requestbuffers req = {0};
    req.count = 1;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
    if (ioctl(fd, VIDIOC_REQBUFS, &req) < 0) { perror("Request buffers"); close(fd); return NULL; }

    struct v4l2_buffer buf = {0};
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index = 0;
    ioctl(fd, VIDIOC_QUERYBUF, &buf);

    void *start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);
    if (start == MAP_FAILED) { perror("mmap"); close(fd); return NULL; }

    FILE *fout = fopen("video.yuv", "wb");
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ioctl(fd, VIDIOC_STREAMON, &type);

    int total_frames = DURATION_SECONDS * FRAME_RATE;
    for (int i = 0; i < total_frames && running; i++) {
        ioctl(fd, VIDIOC_QBUF, &buf);
        ioctl(fd, VIDIOC_DQBUF, &buf);
        fwrite(start, buf.bytesused, 1, fout);
        usleep(1000000 / FRAME_RATE); // ~10 fps
    }

    ioctl(fd, VIDIOC_STREAMOFF, &type);
    munmap(start, buf.length);
    fclose(fout);
    close(fd);
    printf("Video capture done.\n");
    return NULL;
}

void* record_audio(void* arg) {
    snd_pcm_t *handle;
    snd_pcm_hw_params_t *params;
    snd_pcm_uframes_t frames = 1024;
    int rc;

    rc = snd_pcm_open(&handle, AUDIO_DEVICE, SND_PCM_STREAM_CAPTURE, 0);
    if (rc < 0) { fprintf(stderr, "Audio open: %s\n", snd_strerror(rc)); return NULL; }

    snd_pcm_hw_params_alloca(&params);
    snd_pcm_hw_params_any(handle, params);
    snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(handle, params, AUDIO_FORMAT);
    snd_pcm_hw_params_set_channels(handle, params, AUDIO_CHANNELS);
    unsigned int rate = AUDIO_RATE;
    snd_pcm_hw_params_set_rate_near(handle, params, &rate, NULL);
    snd_pcm_hw_params_set_period_size_near(handle, params, &frames, NULL);
    snd_pcm_hw_params(handle, params);

    int size = frames * AUDIO_CHANNELS * 2;
    char *buffer = malloc(size);
    FILE *raw = fopen("audio.raw", "wb");

    int loops = (AUDIO_RATE * DURATION_SECONDS) / frames;
    for (int i = 0; i < loops && running; i++) {
        rc = snd_pcm_readi(handle, buffer, frames);
        if (rc > 0)
            fwrite(buffer, AUDIO_CHANNELS * 2, rc, raw);
    }

    fclose(raw);
    free(buffer);
    snd_pcm_drain(handle);
    snd_pcm_close(handle);

    // Add WAV header
    FILE *wav = fopen("audio.wav", "wb");
    raw = fopen("audio.raw", "rb");

    fseek(raw, 0, SEEK_END);
    int data_size = ftell(raw);
    fseek(raw, 0, SEEK_SET);

    short bits_per_sample = 16;
    short num_channels = AUDIO_CHANNELS;
    int byte_rate = rate * num_channels * bits_per_sample / 8;
    short block_align = num_channels * bits_per_sample / 8;

    fwrite("RIFF", 1, 4, wav);
    int chunk_size = 36 + data_size;
    fwrite(&chunk_size, 4, 1, wav);
    fwrite("WAVE", 1, 4, wav);
    fwrite("fmt ", 1, 4, wav);
    int subchunk1_size = 16;
    short audio_format = 1;
    fwrite(&subchunk1_size, 4, 1, wav);
    fwrite(&audio_format, 2, 1, wav);
    fwrite(&num_channels, 2, 1, wav);
    fwrite(&rate, 4, 1, wav);
    fwrite(&byte_rate, 4, 1, wav);
    fwrite(&block_align, 2, 1, wav);
    fwrite(&bits_per_sample, 2, 1, wav);
    fwrite("data", 1, 4, wav);
    fwrite(&data_size, 4, 1, wav);

    char tmp[1024];
    while (!feof(raw)) {
        size_t r = fread(tmp, 1, sizeof(tmp), raw);
        fwrite(tmp, 1, r, wav);
    }

    fclose(raw);
    fclose(wav);
    remove("audio.raw");

    printf("Audio capture done.\n");
    return NULL;
}

int main() {
    pthread_t video_thread, audio_thread;

    printf("Starting simultaneous video and audio recording for %d seconds...\n", DURATION_SECONDS);

    pthread_create(&video_thread, NULL, record_video, NULL);
    pthread_create(&audio_thread, NULL, record_audio, NULL);

    pthread_join(video_thread, NULL);
    pthread_join(audio_thread, NULL);

    printf("Recording completed. Files:\n - video.yuv\n - audio.wav\n");
    return 0;
}

