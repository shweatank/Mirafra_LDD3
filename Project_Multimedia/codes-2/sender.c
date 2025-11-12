#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <alsa/asoundlib.h>

#define LCD_W 176
#define LCD_H 220

struct buffer 
{ 
    void *start; 
    size_t length; 
};

static int xioctl(int fd, int request, void *arg) 
{
    int r;
    do 
    {
	r = ioctl(fd, request, arg);
    }
    while (r == -1 && errno == EINTR);
    
    return r;
}

/* ---- YUV → RGB565 ---- */
static inline uint16_t yuv_to_rgb565_pixel(int y, int u, int v) {
    int r = y + 1.402f * v;
    int g = y - 0.344136f * u - 0.714136f * v;
    int b = y + 1.772f * u;
    if (r < 0) 
	r = 0; 
    if (r > 255) 
	r = 255;
    if (g < 0) 
	g = 0; 
    if (g > 255) 
	g = 255;
    if (b < 0) 
	b = 0; 
    if (b > 255) 
	b = 255;
    return (uint16_t)(((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3));
}

static void yuyv_to_rgb565(uint8_t *yuyv, uint16_t *rgb, int w, int h) {
    int pix = w * h;
    int in = 0, out = 0;
    while (out < pix) {
        int y0 = yuyv[in++];
        int u  = yuyv[in++] - 128;
        int y1 = yuyv[in++];
        int v  = yuyv[in++] - 128;
        rgb[out++] = yuv_to_rgb565_pixel(y0, u, v);
        if (out < pix) rgb[out++] = yuv_to_rgb565_pixel(y1, u, v);
    }
}

/* Nearest-neighbor resize */
static void resize_nn(uint16_t *src, int sw, int sh, uint16_t *dst, int dw, int dh) 
{
    for (int y = 0; y < dh; ++y) 
    {
        int sy = y * sh / dh;
        for (int x = 0; x < dw; ++x) 
	{
            int sx = x * sw / dw;
            dst[y * dw + x] = src[sy * sw + sx];
        }
    }
}

/* Pack RGB565 into bytes */
static void pack_bytes(uint16_t *src, uint8_t *dst, int pixels) 
{
    for (int i = 0; i < pixels; ++i) 
    {
        uint16_t v = src[i];
        dst[2*i + 0] = v & 0xFF;
        dst[2*i + 1] = (v >> 8) & 0xFF;
    }
}

/* Send framed packet */
static int send_packet(int sock, const uint8_t *buf, size_t len) 
{
    uint32_t netlen = htonl((uint32_t)len);
    if (send(sock, &netlen, 4, 0) != 4) 
	return -1;
    
    size_t sent = 0;
    while (sent < len) 
    {
        ssize_t w = send(sock, buf + sent, len - sent, 0);
        if (w <= 0) 
	    return -1;
        sent += w;
    }
    return 0;
}

int main(int argc, char **argv) 
{
    if (argc != 6) 
    {
        fprintf(stderr, "Usage: %s <pi_ip> <port> <v4l2_device> <width> <height>\n", argv[0]);
        return 1;
    }

    const char *pi_ip = argv[1];
    int port = atoi(argv[2]);
    const char *dev = argv[3];
    int cam_w = atoi(argv[4]);
    int cam_h = atoi(argv[5]);

    /* ---- Open Video Device ---- */
    int fd = open(dev, O_RDWR | O_NONBLOCK, 0);
    if (fd < 0) 
    { 
	perror("open video"); 
	return 1; 
    }

    struct v4l2_format fmt;
    memset(&fmt,0,sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = cam_w;
    fmt.fmt.pix.height = cam_h;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    fmt.fmt.pix.field = V4L2_FIELD_NONE;
    if (xioctl(fd, VIDIOC_S_FMT, &fmt) < 0) 
    { 
	perror("VIDIOC_S_FMT"); return 1; 
    }

    struct v4l2_requestbuffers req;
    memset(&req,0,sizeof(req));
    req.count = 4;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
    if (xioctl(fd, VIDIOC_REQBUFS, &req) < 0) 
    { 
	perror("VIDIOC_REQBUFS"); return 1; 
    }

    struct buffer *bufs = calloc(req.count, sizeof(*bufs));
    for (int i=0;i<req.count;i++) 
    {
        struct v4l2_buffer buf;
        memset(&buf,0,sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;
        if(xioctl(fd, VIDIOC_QUERYBUF, &buf) < 0) 
	{ 
	    perror("VIDIOC_QUERYBUF"); 
	    return 1; 
	}
        bufs[i].length = buf.length;
        bufs[i].start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);
    }
    for (int i=0;i<req.count;i++) {
        struct v4l2_buffer buf;
        memset(&buf,0,sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;
        xioctl(fd, VIDIOC_QBUF, &buf);
    }
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    xioctl(fd, VIDIOC_STREAMON, &type);

    /* ---- ALSA Audio ---- */
    snd_pcm_t *capture_handle;
    snd_pcm_hw_params_t *hw_params;
    unsigned int rate = 16000;
    snd_pcm_open(&capture_handle, "default", SND_PCM_STREAM_CAPTURE, 0);
    snd_pcm_hw_params_malloc(&hw_params);
    snd_pcm_hw_params_any(capture_handle, hw_params);
    snd_pcm_hw_params_set_access(capture_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(capture_handle, hw_params, SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_rate_near(capture_handle, hw_params, &rate, 0);
    snd_pcm_hw_params_set_channels(capture_handle, hw_params, 1);
    snd_pcm_hw_params(capture_handle, hw_params);
    snd_pcm_prepare(capture_handle);

    size_t frames = 1024;
    size_t bytes_per_sample = 2;
    size_t buf_bytes = frames * bytes_per_sample;
    uint8_t *audio_buf = malloc(buf_bytes);

    /* ---- Connect to Receiver ---- */
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in srv;
    memset(&srv,0,sizeof(srv));
    srv.sin_family = AF_INET;
    srv.sin_port = htons(port);
    inet_pton(AF_INET, pi_ip, &srv.sin_addr);
    if (connect(sock, (struct sockaddr*)&srv, sizeof(srv)) < 0) { perror("connect"); return 1; }
    printf("Connected to %s:%d\n", pi_ip, port);

    /* ---- Allocate Buffers ---- */
    uint16_t *rgb_cam = malloc(cam_w * cam_h * 2);
    uint16_t *rgb_lcd = malloc(LCD_W * LCD_H * 2);
    uint8_t *framebuf = malloc(LCD_W * LCD_H * 2);

    /* ---- Main Loop ---- */
    while (1) {
        // Video
        struct v4l2_buffer dbuf;
        memset(&dbuf,0,sizeof(dbuf));
        dbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        dbuf.memory = V4L2_MEMORY_MMAP;
        if (xioctl(fd, VIDIOC_DQBUF, &dbuf) == 0) {
            yuyv_to_rgb565((uint8_t*)bufs[dbuf.index].start, rgb_cam, cam_w, cam_h);
            resize_nn(rgb_cam, cam_w, cam_h, rgb_lcd, LCD_W, LCD_H);
            pack_bytes(rgb_lcd, framebuf, LCD_W*LCD_H);
            send_packet(sock, framebuf, LCD_W*LCD_H*2);
            xioctl(fd, VIDIOC_QBUF, &dbuf);
        }

        // Audio
        int rc = snd_pcm_readi(capture_handle, audio_buf, frames);
        if (rc > 0) {
            send_packet(sock, audio_buf, rc * bytes_per_sample);
        }
    }

    return 0;
}

