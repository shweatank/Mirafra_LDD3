// capture_video.c - Simple V4L2 video capture program that saves YUYV frames to a .yuv file

#include <stdio.h>          // Standard I/O (fopen, fwrite, perror)
#include <stdlib.h>         // malloc, free, exit
#include <string.h>         // memset
#include <fcntl.h>          // open flags
#include <unistd.h>         // close, read, write
#include <sys/ioctl.h>      // ioctl
#include <linux/videodev2.h>// V4L2 structures and constants
#include <sys/mman.h>       // mmap, munmap
#include <errno.h>          // errno

// ---------- Configuration ----------
#define WIDTH 1280              // Frame width
#define HEIGHT 720              // Frame height
#define FRAME_RATE 
10           // Frames per second
#define DURATION 10             // Recording duration in seconds
#define OUTPUT_FILE "video.yuv" // Output file

// Structure to hold buffer info
struct buffer {
    void *start;   // Pointer to mapped memory
    size_t length; // Size of the buffer
};

int main() {
    // Open video device (/dev/video0) for read/write
    int fd = open("/dev/video0", O_RDWR);
    if (fd < 0) { 
        perror("open"); 
        return 1; 
    }

    // Set video format (width, height, pixel format)
    struct v4l2_format fmt = {0};
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;   // Type of buffer
    fmt.fmt.pix.width = WIDTH;                // Frame width
    fmt.fmt.pix.height = HEIGHT;              // Frame height
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV; // Pixel format (YUYV422)
    fmt.fmt.pix.field = V4L2_FIELD_NONE;      // Progressive scan
    if (ioctl(fd, VIDIOC_S_FMT, &fmt) < 0) {  // Apply format
        perror("VIDIOC_S_FMT");
        close(fd);
        return 1;
    }

    // Set frame rate
    struct v4l2_streamparm parm = {0};
    parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    parm.parm.capture.timeperframe.numerator = 1;       // Frame interval numerator
    parm.parm.capture.timeperframe.denominator = FRAME_RATE; // Interval denominator
    if (ioctl(fd, VIDIOC_S_PARM, &parm) < 0) {
        perror("VIDIOC_S_PARM");  // May not be supported; continue anyway
    }

    // Request buffers for memory-mapped I/O
    struct v4l2_requestbuffers req = {0};
    req.count = 4;                           // Number of buffers
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;  // Buffer type
    req.memory = V4L2_MEMORY_MMAP;           // Memory-mapped buffers
    ioctl(fd, VIDIOC_REQBUFS, &req);         // Send request

    // Map buffers into user space
    struct buffer buffers[req.count];
    for (int i = 0; i < req.count; ++i) {
        struct v4l2_buffer buf = {0};
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;
        ioctl(fd, VIDIOC_QUERYBUF, &buf); // Query buffer info
        buffers[i].length = buf.length;   // Save buffer length
        buffers[i].start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset); // Map buffer
        ioctl(fd, VIDIOC_QBUF, &buf);     // Queue buffer for capture
    }

    // Start video streaming
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ioctl(fd, VIDIOC_STREAMON, &type);

    // Open output file to write raw YUYV frames
    FILE *file = fopen(OUTPUT_FILE, "wb");

    int total_frames = DURATION * FRAME_RATE;  // Total frames to capture
    for (int i = 0; i < total_frames; ++i) {
        struct v4l2_buffer buf = {0};
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        ioctl(fd, VIDIOC_DQBUF, &buf); // Dequeue filled buffer
        fwrite(buffers[buf.index].start, buf.bytesused, 1, file); // Write frame to file
        ioctl(fd, VIDIOC_QBUF, &buf); // Requeue buffer for next capture
    }

    // Cleanup
    fclose(file);                           // Close output file
    ioctl(fd, VIDIOC_STREAMOFF, &type);     // Stop streaming
    for (int i = 0; i < req.count; ++i)
        munmap(buffers[i].start, buffers[i].length); // Unmap buffers
    close(fd);                              // Close video device

    return 0;
}

