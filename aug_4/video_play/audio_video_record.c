// record_av.c
#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include <sys/mman.h>
#include <errno.h>

#define WIDTH 640
#define HEIGHT 480
#define FRAME_COUNT 90
#define AUDIO_DURATION_SEC 3
#define AUDIO_RATE 44100
#define AUDIO_CHANNELS 1
#define AUDIO_FORMAT SND_PCM_FORMAT_S16_LE
#define AUDIO_BUFFER_SIZE 44100 * 2 * AUDIO_DURATION_SEC

void *record_audio(void *arg) {
    FILE *audio = fopen("audio.wav", "wb");
    if (!audio) {
        perror("Audio file open");
        return NULL;
    }

    snd_pcm_t *pcm_handle;
    snd_pcm_hw_params_t *params;
    snd_pcm_open(&pcm_handle, "default", SND_PCM_STREAM_CAPTURE, 0);
    snd_pcm_hw_params_malloc(&params);
    snd_pcm_hw_params_any(pcm_handle, params);
    snd_pcm_hw_params_set_access(pcm_handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(pcm_handle, params, AUDIO_FORMAT);
    snd_pcm_hw_params_set_channels(pcm_handle, params, AUDIO_CHANNELS);
    snd_pcm_hw_params_set_rate(pcm_handle, params, AUDIO_RATE, 0);
    snd_pcm_hw_params(pcm_handle, params);
    snd_pcm_prepare(pcm_handle);

    short buffer[1024];
    int frames, total_frames = AUDIO_DURATION_SEC * AUDIO_RATE;

    for (int i = 0; i < total_frames / 1024; ++i) {
        frames = snd_pcm_readi(pcm_handle, buffer, 1024);
        fwrite(buffer, sizeof(short), frames, audio);
    }

    snd_pcm_drain(pcm_handle);
    snd_pcm_close(pcm_handle);
    fclose(audio);
    puts("Audio recording done.");
    return NULL;
}

void *record_video(void *arg) {
    int fd = open("/dev/video0", O_RDWR);
    if (fd == -1) {
        perror("Opening video device");
        return NULL;
    }

    struct v4l2_format fmt = {0};
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = WIDTH;
    fmt.fmt.pix.height = HEIGHT;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    fmt.fmt.pix.field = V4L2_FIELD_NONE;
    if (ioctl(fd, VIDIOC_S_FMT, &fmt) < 0) {
        perror("Setting Pixel Format");
        return NULL;
    }

    struct v4l2_requestbuffers req = {0};
    req.count = 4;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
    if (ioctl(fd, VIDIOC_REQBUFS, &req) < 0) {
        perror("Requesting Buffer");
        return NULL;
    }

    void *buffers[4];
    struct v4l2_buffer buf = {0};

    for (int i = 0; i < 4; i++) {
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;
        ioctl(fd, VIDIOC_QUERYBUF, &buf);
        buffers[i] = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);
    }

    for (int i = 0; i < 4; i++) {
        buf.index = i;
        ioctl(fd, VIDIOC_QBUF, &buf);
    }

    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ioctl(fd, VIDIOC_STREAMON, &type);

    FILE *out = fopen("video.yuv", "wb");
    for (int i = 0; i < FRAME_COUNT; i++) {
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        ioctl(fd, VIDIOC_DQBUF, &buf);
        fwrite(buffers[buf.index], buf.bytesused, 1, out);
        ioctl(fd, VIDIOC_QBUF, &buf);
    }
    fclose(out);

    ioctl(fd, VIDIOC_STREAMOFF, &type);
    for (int i = 0; i < 4; i++) {
        munmap(buffers[i], buf.length);
    }
    close(fd);
    puts("Video recording done.");
    return NULL;
}

int main() {
    pthread_t audio_thread, video_thread;

    pthread_create(&audio_thread, NULL, record_audio, NULL);
    pthread_create(&video_thread, NULL, record_video, NULL);

    pthread_join(audio_thread, NULL);
    pthread_join(video_thread, NULL);

    puts("Audio and Video recording completed.");
    return 0;
}

