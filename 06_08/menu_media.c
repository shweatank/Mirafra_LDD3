#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <errno.h>
#include <time.h>
#include <pthread.h>
#include <linux/videodev2.h>

#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>

#define PCM_DEVICE "default"
#define CHANNELS 1
#define SAMPLE_RATE 44100
#define BITS_PER_SAMPLE 16
#define SECONDS 5
#define PCM_FORMAT SND_PCM_FORMAT_S16_LE

#define WIDTH 640
#define HEIGHT 480
#define FRAME_COUNT 180  // ~6 seconds at 30 FPS

#define FPS 30
#define RECORD_SECONDS 5
#define VIDEO_FRAME_COUNT (FPS * RECORD_SECONDS)
#define AUDIO_SAMPLE_RATE 44100
#define AUDIO_CHANNELS 1
#define AUDIO_BITS_PER_SAMPLE 16
#define AUDIO_FORMAT SND_PCM_FORMAT_S16_LE

//==============================================================
void video(void){
	    int fd = open("/dev/video0", O_RDWR);
    if (fd == -1) {
        perror("Opening video device");
        return ;
    }

    // Set video format
    struct v4l2_format fmt = {0};
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = WIDTH;
    fmt.fmt.pix.height = HEIGHT;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    fmt.fmt.pix.field = V4L2_FIELD_NONE;
    if (ioctl(fd, VIDIOC_S_FMT, &fmt) < 0) {
        perror("Setting Pixel Format");
        return ;
    }

    // Request buffer
    struct v4l2_requestbuffers req = {0};
    req.count = 4;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
    if (ioctl(fd, VIDIOC_REQBUFS, &req) < 0) {
        perror("Requesting Buffer");
        return ;
    }

    // Map buffers
    void *buffers[4];
    struct v4l2_buffer buf = {0};

    for (int i = 0; i < 4; i++) {
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;
        if (ioctl(fd, VIDIOC_QUERYBUF, &buf) < 0) {
            perror("Querying Buffer");
            return ;
        }
        buffers[i] = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);
        if (buffers[i] == MAP_FAILED) {
            perror("Memory Mapping Failed");
            return ;
        }
    }

    // Queue buffers
    for (int i = 0; i < 4; i++) {
        buf.index = i;
        if (ioctl(fd, VIDIOC_QBUF, &buf) < 0) {
            perror("Queue Buffer");
            return ;
        }
    }

    // Start streaming
    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(fd, VIDIOC_STREAMON, &type) < 0) {
        perror("Stream On");
        return ;
    }

    FILE *out = fopen("video.yuv", "wb");
    if (!out) {
        perror("File open");
        return ;
    }

    printf("Recording video...\n");

    for (int i = 0; i < FRAME_COUNT; i++) {
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;

        if (ioctl(fd, VIDIOC_DQBUF, &buf) < 0) {
            perror("Dequeue Buffer");
            continue;
        }

        fwrite(buffers[buf.index], buf.bytesused, 1, out);

        if (ioctl(fd, VIDIOC_QBUF, &buf) < 0) {
            perror("Requeue Buffer");
            return ;
        }
    }

    printf("Done recording.\n");

    fclose(out);

    // Stop streaming
    if (ioctl(fd, VIDIOC_STREAMOFF, &type) < 0) {
        perror("Stream Off");
        return ;
    }

    for (int i = 0; i < 4; i++) {
        munmap(buffers[i], buf.length);
    }

    close(fd);
    
}
//===========================================================


//----------------------------------------------------------
void record(void){
	 snd_pcm_t *pcm_handle;
    snd_pcm_hw_params_t *params;
    FILE *wav_file;
    char *buffer;
    int frames, pcmrc;
    int buffer_size;
    int total_data_size;

    // Open PCM device for recording (capture)
    snd_pcm_open(&pcm_handle, PCM_DEVICE, SND_PCM_STREAM_CAPTURE, 0);

    // Allocate parameters
    snd_pcm_hw_params_malloc(&params);
    snd_pcm_hw_params_any(pcm_handle, params);
    snd_pcm_hw_params_set_access(pcm_handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(pcm_handle, params, PCM_FORMAT);
    snd_pcm_hw_params_set_channels(pcm_handle, params, CHANNELS);
    snd_pcm_hw_params_set_rate(pcm_handle, params, SAMPLE_RATE, 0);

    // Apply parameters
    snd_pcm_hw_params(pcm_handle, params);

    snd_pcm_hw_params_get_period_size(params, (snd_pcm_uframes_t *)&frames, 0);
    buffer_size = frames * CHANNELS * BITS_PER_SAMPLE / 8;
    buffer = (char *) malloc(buffer_size);

    int loops = SAMPLE_RATE * SECONDS / frames;
    total_data_size = loops * buffer_size;

    wav_file = fopen("recorded.wav", "wb");
    write_wav_header(wav_file, SAMPLE_RATE, CHANNELS, BITS_PER_SAMPLE, total_data_size);

    printf("Recording %d seconds...\n", SECONDS);

    for (int i = 0; i < loops; i++) {
        pcmrc = snd_pcm_readi(pcm_handle, buffer, frames);
        if (pcmrc == -EPIPE) {
            snd_pcm_prepare(pcm_handle);
        } else if (pcmrc < 0) {
            fprintf(stderr, "Error reading PCM device: %s\n", snd_strerror(pcmrc));
        } else {
            fwrite(buffer, 1, buffer_size, wav_file);
        }
    }

    printf("Recording complete. Saved to 'recorded.wav'\n");

    fclose(wav_file);
    free(buffer);
    snd_pcm_drain(pcm_handle);
    snd_pcm_close(pcm_handle);
    snd_pcm_hw_params_free(params);
}
void write_wav_header(FILE *file, int sample_rate, short channels, int bits_per_sample, int data_size) {
    int byte_rate = sample_rate * channels * bits_per_sample / 8;
    short block_align = channels * bits_per_sample / 8;

    fwrite("RIFF", 1, 4, file);
    int chunk_size = 36 + data_size;
    fwrite(&chunk_size, 4, 1, file);
    fwrite("WAVE", 1, 4, file);

    // fmt subchunk
    fwrite("fmt ", 1, 4, file);
    int subchunk1_size = 16;
    fwrite(&subchunk1_size, 4, 1, file);
    short audio_format = 1;
    fwrite(&audio_format, 2, 1, file);
    fwrite(&channels, 2, 1, file);
    fwrite(&sample_rate, 4, 1, file);
    fwrite(&byte_rate, 4, 1, file);
    fwrite(&block_align, 2, 1, file);
    fwrite(&bits_per_sample, 2, 1, file);

    // data subchunk
    fwrite("data", 1, 4, file);
    fwrite(&data_size, 4, 1, file);
}

//-------------------------------------------------------------------------

void* record_audio(void* arg) {
record();
   pthread_exit(NULL);
}


void* record_video(void* arg) {
video();
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


//===================================================
void video_audio(){
	pthread_t audio_tid, video_tid;

    printf("Recording audio and video simultaneously for %d seconds...\n", RECORD_SECONDS);
    pthread_create(&audio_tid, NULL, record_audio, NULL);
    pthread_create(&video_tid, NULL, record_video, NULL);

    pthread_join(audio_tid, NULL);
    pthread_join(video_tid, NULL);

    printf("Recording finished. Muxing to output.mp4...\n");
    mux_audio_video();

    printf("Done. Saved as output.mp4\n");
}
//====================================================

void play_audio(void){
	system("aplay recorded.wav");
}

void play_video(){
	system("ffplay -f rawvideo -pixel_format yuyv422 -video_size 640x480 video.yuv");
}

void play_audio_video(){
	system("ffplay output.mp4");
}

int main(){
	int num;
	printf("Enter number to choose a media\n 1)record_audio 2)record_video 3)video & audio 4)play recorded audio_video 5)play_recorded_audio 6)play_recorded_video\n");
	scanf("%d",&num);
	switch(num){
		case 1 : record(); break;
	        case 2 : video(); break;
	        case 3 : video_audio(); break;
		case 4 : play_audio_video(); break;
	        case 5 : play_audio(); break;
		case 6 : play_video(); break;

	        default : printf("Wrong option\n"); 
}
return 0;
}

