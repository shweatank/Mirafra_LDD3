#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <alsa/asoundlib.h>

#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>

#define WIDTH 640
#define HEIGHT 480
#define FPS 30
#define RECORD_SECONDS 5
#define VIDEO_FRAME_COUNT (FPS * RECORD_SECONDS)
#define AUDIO_SAMPLE_RATE 44100
#define AUDIO_CHANNELS 1
#define AUDIO_BITS_PER_SAMPLE 16
#define AUDIO_FORMAT SND_PCM_FORMAT_S16_LE

// Audio thread function
void* record_audio(void* arg) {
    snd_pcm_t *pcm_handle;
    snd_pcm_hw_params_t *params;
    FILE *wav_file;
    char *buffer;
    int frames, pcmrc;
    int buffer_size;
    int loops;

    snd_pcm_open(&pcm_handle, "default", SND_PCM_STREAM_CAPTURE, 0);
    snd_pcm_hw_params_malloc(&params);
    snd_pcm_hw_params_any(pcm_handle, params);
    snd_pcm_hw_params_set_access(pcm_handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(pcm_handle, params, AUDIO_FORMAT);
    snd_pcm_hw_params_set_channels(pcm_handle, params, AUDIO_CHANNELS);
    snd_pcm_hw_params_set_rate(pcm_handle, params, AUDIO_SAMPLE_RATE, 0);
    snd_pcm_hw_params(pcm_handle, params);

    snd_pcm_hw_params_get_period_size(params, (snd_pcm_uframes_t *)&frames, 0);
    buffer_size = frames * AUDIO_CHANNELS * AUDIO_BITS_PER_SAMPLE / 8;
    buffer = malloc(buffer_size);
    loops = AUDIO_SAMPLE_RATE * RECORD_SECONDS / frames;

    wav_file = fopen("audio.wav", "wb");
    short block_align = AUDIO_CHANNELS * AUDIO_BITS_PER_SAMPLE / 8;
    int byte_rate = AUDIO_SAMPLE_RATE * block_align;
    int data_size = loops * buffer_size;
short num_channels = AUDIO_CHANNELS;
int sample_rate = AUDIO_SAMPLE_RATE;
short bits_per_sample = AUDIO_BITS_PER_SAMPLE;


    // Write WAV header
    fwrite("RIFF", 1, 4, wav_file);
    int chunk_size = 36 + data_size;
    fwrite(&chunk_size, 4, 1, wav_file);
    fwrite("WAVEfmt ", 1, 8, wav_file);
    int subchunk1_size = 16, audio_format = 1;
    fwrite(&subchunk1_size, 4, 1, wav_file);
    fwrite(&audio_format, 2, 1, wav_file);
//    fwrite(&AUDIO_CHANNELS, 2, 1, wav_file);
fwrite(&num_channels, 2, 1, wav_file);
  //  fwrite(&AUDIO_SAMPLE_RATE, 4, 1, wav_file);
fwrite(&sample_rate, 4, 1, wav_file);
   // fwrite(&byte_rate, 4, 1, wav_file);
fwrite(&byte_rate, 4, 1, wav_file);
   // fwrite(&block_align, 2, 1, wav_file);
fwrite(&block_align, 2, 1, wav_file);
    //fwrite(&AUDIO_BITS_PER_SAMPLE, 2, 1, wav_file);
fwrite(&bits_per_sample, 2, 1, wav_file);
    fwrite("data", 1, 4, wav_file);
    fwrite(&data_size, 4, 1, wav_file);

    // Capture
    for (int i = 0; i < loops; i++) {
        pcmrc = snd_pcm_readi(pcm_handle, buffer, frames);
        if (pcmrc < 0) snd_pcm_prepare(pcm_handle);
        fwrite(buffer, 1, buffer_size, wav_file);
    }

    fclose(wav_file);
    free(buffer);
    snd_pcm_drain(pcm_handle);
    snd_pcm_close(pcm_handle);
    snd_pcm_hw_params_free(params);
    pthread_exit(NULL);
}

// Video thread function
void* record_video(void* arg) {
    int fd = open("/dev/video0", O_RDWR);
    if (fd < 0) { perror("video open"); pthread_exit(NULL); }

    struct v4l2_format fmt = {0};
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = WIDTH;
    fmt.fmt.pix.height = HEIGHT;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    fmt.fmt.pix.field = V4L2_FIELD_NONE;
    ioctl(fd, VIDIOC_S_FMT, &fmt);

    struct v4l2_requestbuffers req = {0};
    req.count = 4;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
    ioctl(fd, VIDIOC_REQBUFS, &req);

    void *buffers[4];
    struct v4l2_buffer buf = {0};
    int buf_size;

    for (int i = 0; i < 4; i++) {
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;
        ioctl(fd, VIDIOC_QUERYBUF, &buf);
        buffers[i] = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);
        buf_size = buf.length;
    }

    for (int i = 0; i < 4; i++) {
        buf.index = i;
        ioctl(fd, VIDIOC_QBUF, &buf);
    }

    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ioctl(fd, VIDIOC_STREAMON, &type);

    FILE *fout = fopen("video.yuv", "wb");
    for (int i = 0; i < VIDEO_FRAME_COUNT; i++) {
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        ioctl(fd, VIDIOC_DQBUF, &buf);
        fwrite(buffers[buf.index], buf.bytesused, 1, fout);
        ioctl(fd, VIDIOC_QBUF, &buf);
    }

    fclose(fout);
    ioctl(fd, VIDIOC_STREAMOFF, &type);
    for (int i = 0; i < 4; i++) {
        munmap(buffers[i], buf_size);
    }
    close(fd);
    pthread_exit(NULL);
}

// Mux .yuv and .wav to .mp4 using FFmpeg libav
void mux_audio_video() {
    AVFormatContext *out_ctx = NULL;
    AVStream *video_st, *audio_st;
    AVCodecContext *video_codec_ctx, *audio_codec_ctx;
    AVFrame *frame;
    AVPacket pkt;

    system("ffmpeg -y -f rawvideo -pix_fmt yuyv422 -s 640x480 -r 30 -i video.yuv -i audio.wav -c:v libx264 -preset ultrafast -c:a aac output.mp4");
}

int main() {
    pthread_t audio_tid, video_tid;

    printf("Recording audio and video simultaneously for %d seconds...\n", RECORD_SECONDS);
    pthread_create(&audio_tid, NULL, record_audio, NULL);
    pthread_create(&video_tid, NULL, record_video, NULL);

    pthread_join(audio_tid, NULL);
    pthread_join(video_tid, NULL);

    printf("Recording finished. Muxing to output.mp4...\n");
    mux_audio_video();

    printf("Done. Saved as output.mp4\n");
    return 0;
}

