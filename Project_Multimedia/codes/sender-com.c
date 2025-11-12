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

/* Buffer structure to hold mmap()'ed regions */
struct buffer 
{ 
    void *start;    // pointer to the start of memory-mapped buffer
    size_t length;  // size of the buffer
};

/* Wrapper for ioctl that retries when interrupted by signals */
static int xioctl(int fd, int request, void *arg) 
{
    int r;
    do 
    {
        r = ioctl(fd, request, arg);
    }
    while (r == -1 && errno == EINTR);   // retry if interrupted
    return r;
}

/* ---- YUV → RGB565 conversion ---- */
/* Convert a single YUV pixel to 16-bit RGB565 */
static inline uint16_t yuv_to_rgb565_pixel(int y, int u, int v) {
    // Convert YUV (with U,V centered at 0) to RGB
    int r = y + 1.402f * v;
    int g = y - 0.344136f * u - 0.714136f * v;
    int b = y + 1.772f * u;

    // Clamp values into [0,255]
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

    // Pack into RGB565: RRRRRGGGGGGBBBBB
    return (uint16_t)(((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3));
}

/* Convert entire YUYV frame to RGB565 frame */
static void yuyv_to_rgb565(uint8_t *yuyv, uint16_t *rgb, int w, int h) {
    int pix = w * h;   // total number of pixels
    int in = 0, out = 0;
    while (out < pix) {
        int y0 = yuyv[in++];       //  Y0
        int u  = yuyv[in++] - 128; //   U
        int y1 = yuyv[in++];       //  Y1
        int v  = yuyv[in++] - 128; //   V
        // Each U,V applies to 2 neighboring Y samples
        rgb[out++] = yuv_to_rgb565_pixel(y0, u, v);
        if (out < pix) rgb[out++] = yuv_to_rgb565_pixel(y1, u, v);
    }
}

/* Nearest-neighbor resize from (sw,sh) → (dw,dh) */
static void resize_nn(uint16_t *src, int sw, int sh, uint16_t *dst, int dw, int dh) 
{
    for (int y = 0; y < dh; ++y) 
    {
        int sy = y * sh / dh;   // map dest row → source row
        for (int x = 0; x < dw; ++x) 
        {
            int sx = x * sw / dw;   // map dest col → source col
            dst[y * dw + x] = src[sy * sw + sx];
        }
    }
}

/* Pack 16-bit RGB565 into byte array (little-endian) */
static void pack_bytes(uint16_t *src, uint8_t *dst, int pixels) 
{
    for (int i = 0; i < pixels; ++i) 
    {
        uint16_t v = src[i];
        dst[2*i + 0] = v & 0xFF;         // low byte
        dst[2*i + 1] = (v >> 8) & 0xFF;  // high byte
    }
}

/* Send a packet over TCP:
   - Prefix with 4-byte length (network order)
   - Then send full payload
*/
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

    /* ------------------------------
       struct v4l2_format (used for setting capture format)
       ------------------------------
       struct v4l2_format {
           enum v4l2_buf_type type;  // buffer type, e.g. V4L2_BUF_TYPE_VIDEO_CAPTURE
           union {
               struct v4l2_pix_format pix; // format details for VIDEO_CAPTURE
               ...
           } fmt;
       };
	
       struct v4l2_pix_format {
           __u32          width;
           __u32          height;
           __u32          pixelformat;  // e.g. V4L2_PIX_FMT_YUYV
           __u32          field;        // interlacing
           __u32          bytesperline; // driver fills
           __u32          sizeimage;    // driver fills
           __u32          colorspace;
           __u32          priv;
       };
    */
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

    /* ------------------------------
       struct v4l2_requestbuffers (used for buffer allocation)
       ------------------------------
       struct v4l2_requestbuffers {
           __u32               count;   // number of buffers requested
           enum v4l2_buf_type  type;    // buffer type (VIDEO_CAPTURE)
           enum v4l2_memory    memory;  // e.g. V4L2_MEMORY_MMAP
           __u32               reserved[2];
       };
    */
    struct v4l2_requestbuffers req;
    memset(&req,0,sizeof(req));
    req.count = 4;                         // request 4 buffers
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;         // buffers will be mmap'ed
    if (xioctl(fd, VIDIOC_REQBUFS, &req) < 0) 
    { 
        perror("VIDIOC_REQBUFS"); return 1; 
    }

    /* Allocate space to hold buffer info in userspace */
    struct buffer *bufs = calloc(req.count, sizeof(*bufs));

    /* ------------------------------
       struct v4l2_buffer (used to query or queue buffers)
       ------------------------------
       struct v4l2_buffer {
           __u32                  index;     // buffer index [0..count-1]
           enum v4l2_buf_type     type;      // VIDEO_CAPTURE
           __u32                  bytesused; // filled by driver
           __u32                  flags;
           enum v4l2_field        field;
           struct timeval         timestamp;
           struct v4l2_timecode   timecode;
           __u32                  sequence;
           enum v4l2_memory       memory;    // e.g. V4L2_MEMORY_MMAP
           union {
               __u32 offset;   // mmap offset
               unsigned long userptr;
               struct v4l2_plane *planes;
           } m;
           __u32                  length;    // size of buffer
           __u32                  input;
           __u32                  reserved;
       };
    */
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

    /* Queue all buffers so the driver can fill them */
    for (int i=0;i<req.count;i++) {
        struct v4l2_buffer buf;
        memset(&buf,0,sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;
        xioctl(fd, VIDIOC_QBUF, &buf);
    }

    /* Start video streaming */
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    xioctl(fd, VIDIOC_STREAMON, &type);

    /* ---- ALSA Audio Setup ---- */
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

    /* ---- Connect to Receiver (TCP socket) ---- */
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in srv;
    memset(&srv,0,sizeof(srv));
    srv.sin_family = AF_INET;
    srv.sin_port = htons(port);
    inet_pton(AF_INET, pi_ip, &srv.sin_addr);
    if (connect(sock, (struct sockaddr*)&srv, sizeof(srv)) < 0) { perror("connect"); return 1; }
    printf("Connected to %s:%d\n", pi_ip, port);

    /* ---- Allocate Buffers ---- */
    uint16_t *rgb_cam = malloc(cam_w * cam_h * 2);   // raw camera frame in RGB565
    uint16_t *rgb_lcd = malloc(LCD_W * LCD_H * 2);   // resized frame
    uint8_t *framebuf = malloc(LCD_W * LCD_H * 2);   // packed bytes for sending

    /* ---- Main Loop ---- */
    while (1) {
        // ---- Video capture ----
        struct v4l2_buffer dbuf;
        memset(&dbuf,0,sizeof(dbuf));
        dbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        dbuf.memory = V4L2_MEMORY_MMAP;

        // Dequeue a filled buffer from driver
        if (xioctl(fd, VIDIOC_DQBUF, &dbuf) == 0) {
            yuyv_to_rgb565((uint8_t*)bufs[dbuf.index].start, rgb_cam, cam_w, cam_h);
            resize_nn(rgb_cam, cam_w, cam_h, rgb_lcd, LCD_W, LCD_H);
            pack_bytes(rgb_lcd, framebuf, LCD_W*LCD_H);
            send_packet(sock, framebuf, LCD_W*LCD_H*2);
            xioctl(fd, VIDIOC_QBUF, &dbuf); // requeue buffer back to driver
        }

        // ---- Audio capture ----
        int rc = snd_pcm_readi(capture_handle, audio_buf, frames);
        if (rc > 0) {
            send_packet(sock, audio_buf, rc * bytes_per_sample);
        }
    }

    return 0;
}

