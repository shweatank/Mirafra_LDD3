#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include <sys/mman.h>
#include <alsa/asoundlib.h>
#include <errno.h>

#define WIDTH 640
#define HEIGHT 480
#define FRAME_RATE 30
#define RECORD_SECONDS 5
#define AUDIO_DEVICE "hw:0,7"

#define VIDEO_FILE "video.yuv"
#define AUDIO_FILE "audio.raw"

void *record_video(void *arg) {
    int fd = open("/dev/video0", O_RDWR);
    if (fd == -1) {
        perror("Opening video device");
        pthread_exit(NULL);
    }

    struct v4l2_format fmt = {0};
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = WIDTH;
    fmt.fmt.pix.height = HEIGHT;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    fmt.fmt.pix.field = V4L2_FIELD_NONE;
    if (ioctl(fd, VIDIOC_S_FMT, &fmt) < 0) {
        perror("Setting Pixel Format");
        close(fd);
        pthread_exit(NULL);
    }

    struct v4l2_requestbuffers req = {0};
    req.count = 4;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
    if (ioctl(fd, VIDIOC_REQBUFS, &req) < 0) {
        perror("Requesting Buffer");
        close(fd);
        pthread_exit(NULL);
    }

    void *buffers[4];
    struct v4l2_buffer buf = {0};
    for (int i = 0; i < 4; i++) {
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;
        if (ioctl(fd, VIDIOC_QUERYBUF, &buf) < 0) {
            perror("Querying Buffer");
            close(fd);
            pthread_exit(NULL);
        }
        buffers[i] = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);
        if (buffers[i] == MAP_FAILED) {
            perror("Memory Mapping Failed");
            close(fd);
            pthread_exit(NULL);
        }
    }

    for (int i = 0; i < 4; i++) {
        buf.index = i;
        if (ioctl(fd, VIDIOC_QBUF, &buf) < 0) {
            perror("Queue Buffer");
            close(fd);
            pthread_exit(NULL);
        }
    }

    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(fd, VIDIOC_STREAMON, &type) < 0) {
        perror("Stream On");
        close(fd);
        pthread_exit(NULL);
    }

    FILE *out = fopen(VIDEO_FILE, "wb");
    if (!out) {
        perror("File open");
        close(fd);
        pthread_exit(NULL);
    }

    int total_frames = FRAME_RATE * RECORD_SECONDS;
    for (int i = 0; i < total_frames; i++) {
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;

        if (ioctl(fd, VIDIOC_DQBUF, &buf) < 0) {
            perror("Dequeue Buffer");
            continue;
        }

        fwrite(buffers[buf.index], buf.bytesused, 1, out);

        if (ioctl(fd, VIDIOC_QBUF, &buf) < 0) {
            perror("Requeue Buffer");
            break;
        }
    }

    fclose(out);
    ioctl(fd, VIDIOC_STREAMOFF, &type);
    for (int i = 0; i < 4; i++) {
        munmap(buffers[i], buf.length);
    }
    close(fd);
    printf("Video recording complete.\n");
    pthread_exit(NULL);
}

void *record_audio(void *arg) {
    snd_pcm_t *handle;
    snd_pcm_hw_params_t *params;
    int rc, dir;
    unsigned int rate = 44100;
    snd_pcm_uframes_t frames = 32;
    int channels = 1;
    int seconds = RECORD_SECONDS;

    rc = snd_pcm_open(&handle, AUDIO_DEVICE, SND_PCM_STREAM_CAPTURE, 0);
    if (rc < 0) {
        fprintf(stderr, "unable to open pcm device: %s\n", snd_strerror(rc));
        pthread_exit(NULL);
    }

    snd_pcm_hw_params_malloc(&params);
    snd_pcm_hw_params_any(handle, params);
    snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_channels(handle, params, channels);
    snd_pcm_hw_params_set_rate_near(handle, params, &rate, &dir);
    snd_pcm_hw_params_set_period_size_near(handle, params, &frames, &dir);
    snd_pcm_hw_params(handle, params);

    snd_pcm_hw_params_get_period_size(params, &frames, &dir);
    int size = frames * channels * 2;
    char *buffer = (char *) malloc(size);

    snd_pcm_hw_params_get_period_time(params, &rate, &dir);
    FILE *fp = fopen(AUDIO_FILE, "wb");
    if (!fp) {
        perror("audio file open");
        pthread_exit(NULL);
    }

    int loops = (seconds * 1000000) / rate;
    while (loops-- > 0) {
        rc = snd_pcm_readi(handle, buffer, frames);
        if (rc == -EPIPE) {
            snd_pcm_prepare(handle);
        } else if (rc < 0) {
            fprintf(stderr, "read error: %s\n", snd_strerror(rc));
        }
        fwrite(buffer, 1, size, fp);
    }

    fclose(fp);
    snd_pcm_drain(handle);
    snd_pcm_close(handle);
    free(buffer);
    snd_pcm_hw_params_free(params);

    printf("Audio recording complete.\n");
    pthread_exit(NULL);
}

int main() {
    pthread_t video_thread, audio_thread;

    printf("Recording video and audio for %d seconds...\n", RECORD_SECONDS);

    pthread_create(&video_thread, NULL, record_video, NULL);
    pthread_create(&audio_thread, NULL, record_audio, NULL);

    pthread_join(video_thread, NULL);
    pthread_join(audio_thread, NULL);

    printf("Recording finished. Files saved:\n");
    printf("  Video: %s\n", VIDEO_FILE);
    printf("  Audio: %s\n", AUDIO_FILE);
    system("ffmpeg -y -f rawvideo -pixel_format yuyv422 -video_size 640x480 -framerate 30 -i video.yuv "
           "-i audio.wav -c:v libx264 -preset fast -crf 23 -c:a aac output.mp4");

    printf("Recording complete: output.mp4\n");
    return 0;
}

