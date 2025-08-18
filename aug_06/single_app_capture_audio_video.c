#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <alsa/asoundlib.h>

#define VIDEO_DEVICE "/dev/video0"
#define AUDIO_DEVICE "default"

#define WIDTH 640
#define HEIGHT 480
#define VIDEO_BUFFER_SIZE (WIDTH * HEIGHT * 2)

#define AUDIO_RATE 44100
#define AUDIO_CHANNELS 1
#define AUDIO_FORMAT SND_PCM_FORMAT_S16_LE
#define AUDIO_DURATION 5
#define AUDIO_BUFFER_SIZE (AUDIO_RATE * AUDIO_CHANNELS * 2 * AUDIO_DURATION)

void write_wav_header(FILE *f, int sample_rate, int channels, int data_size) {
    int byte_rate = sample_rate * channels * 2;
    int block_align = channels * 2;

    fwrite("RIFF", 1, 4, f);
    int chunk_size = 36 + data_size;
    fwrite(&chunk_size, 4, 1, f);
    fwrite("WAVE", 1, 4, f);
    fwrite("fmt ", 1, 4, f);

    int subchunk1_size = 16;
    short audio_format = 1;
    short num_channels = channels;
    short bits_per_sample = 16;

    fwrite(&subchunk1_size, 4, 1, f);
    fwrite(&audio_format, 2, 1, f);
    fwrite(&num_channels, 2, 1, f);
    fwrite(&sample_rate, 4, 1, f);
    fwrite(&byte_rate, 4, 1, f);
    fwrite(&block_align, 2, 1, f);
    fwrite(&bits_per_sample, 2, 1, f);

    fwrite("data", 1, 4, f);
    fwrite(&data_size, 4, 1, f);
}

int main() {
    printf("Capturing 5 seconds of audio and video...\n");

    // --- Open Video Device ---
    int video_fd = open(VIDEO_DEVICE, O_RDWR);
    if (video_fd < 0) {
        perror("Failed to open video device");
        return 1;
    }

    // --- Set Video Format ---
    struct v4l2_format video_fmt = {0};
    video_fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    video_fmt.fmt.pix.width = WIDTH;
    video_fmt.fmt.pix.height = HEIGHT;
    video_fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    video_fmt.fmt.pix.field = V4L2_FIELD_NONE;

    if (ioctl(video_fd, VIDIOC_S_FMT, &video_fmt) < 0) {
        perror("Failed to set video format");
        close(video_fd);
        return 1;
    }

    // --- Request Buffers ---
    struct v4l2_requestbuffers req = {0};
    req.count = 1;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    if (ioctl(video_fd, VIDIOC_REQBUFS, &req) < 0) {
        perror("Failed to request buffers");
        close(video_fd);
        return 1;
    }

    // --- Query Buffer ---
    struct v4l2_buffer buf = {0};
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index = 0;

    if (ioctl(video_fd, VIDIOC_QUERYBUF, &buf) < 0) {
        perror("Failed to query buffer");
        close(video_fd);
        return 1;
    }

    void *video_buffer = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, video_fd, buf.m.offset);
    if (video_buffer == MAP_FAILED) {
        perror("Failed to mmap video buffer");
        close(video_fd);
        return 1;
    }

    // --- Queue Buffer ---
    if (ioctl(video_fd, VIDIOC_QBUF, &buf) < 0) {
        perror("Failed to queue buffer");
        return 1;
    }

    // --- Stream On ---
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(video_fd, VIDIOC_STREAMON, &type) < 0) {
        perror("Failed to start video stream");
        return 1;
    }

    // --- Open ALSA Audio Capture ---
    snd_pcm_t *pcm_handle;
    snd_pcm_hw_params_t *hw_params;
    snd_pcm_open(&pcm_handle, AUDIO_DEVICE, SND_PCM_STREAM_CAPTURE, 0);
    snd_pcm_hw_params_malloc(&hw_params);
    snd_pcm_hw_params_any(pcm_handle, hw_params);
    snd_pcm_hw_params_set_access(pcm_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(pcm_handle, hw_params, AUDIO_FORMAT);
    snd_pcm_hw_params_set_channels(pcm_handle, hw_params, AUDIO_CHANNELS);
    snd_pcm_hw_params_set_rate(pcm_handle, hw_params, AUDIO_RATE, 0);
    snd_pcm_hw_params(pcm_handle, hw_params);
    snd_pcm_hw_params_free(hw_params);
    snd_pcm_prepare(pcm_handle);

    // --- Buffers ---
    FILE *video_out = fopen("video.yuv", "wb");
    FILE *audio_out = fopen("audio.wav", "wb");
    int total_frames = AUDIO_DURATION * 25;

    char *audio_buffer = malloc(AUDIO_BUFFER_SIZE);
    int audio_captured = 0;

    // Write dummy WAV header (to be updated later)
    write_wav_header(audio_out, AUDIO_RATE, AUDIO_CHANNELS, 0);

    for (int i = 0; i < total_frames; i++) {
        // Audio: capture 1 frame of audio (1764 bytes for 44100Hz/mono/16-bit @ 25fps)
        snd_pcm_readi(pcm_handle, audio_buffer + audio_captured, AUDIO_RATE / 25);
        audio_captured += AUDIO_RATE * 2 / 25;

        // Video: dequeue buffer
        if (ioctl(video_fd, VIDIOC_DQBUF, &buf) == 0) {
            fwrite(video_buffer, 1, VIDEO_BUFFER_SIZE, video_out);
            ioctl(video_fd, VIDIOC_QBUF, &buf); // Requeue
        } else {
            perror("Video read");
        }

        usleep(40000); // ~25fps
    }

    // Write captured audio
    fseek(audio_out, 44, SEEK_SET);
    fwrite(audio_buffer, 1, audio_captured, audio_out);

    // Fix WAV header
    fseek(audio_out, 0, SEEK_SET);
    write_wav_header(audio_out, AUDIO_RATE, AUDIO_CHANNELS, audio_captured);

    // Cleanup
    fclose(video_out);
    fclose(audio_out);
    free(audio_buffer);
    munmap(video_buffer, buf.length);
    snd_pcm_close(pcm_handle);
    close(video_fd);

    printf("✅ Capture complete.\n\n");
    printf("🎞️ Use this command to combine audio and video:\n");
    printf("ffmpeg -f rawvideo -pixel_format yuyv422 -video_size 640x480 -framerate 25 -i video.yuv -i audio.wav -c:v libx264 -c:a aac output.mp4\n");
    return 0;
}

