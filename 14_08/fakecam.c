// fakecam.c// fakecam.c
#include <linux/videodev2.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <stdlib.h>

#define WIDTH 640
#define HEIGHT 480

int main() {
    int fd = open("/dev/video10", O_WRONLY);
    if (fd < 0) {
        perror("Cannot open /dev/video10");
        return 1;
    }

    // Set format
    struct v4l2_format fmt;
    memset(&fmt, 0, sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
    fmt.fmt.pix.width = WIDTH;
    fmt.fmt.pix.height = HEIGHT;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    fmt.fmt.pix.field = V4L2_FIELD_NONE;

    if (ioctl(fd, VIDIOC_S_FMT, &fmt) < 0) {
        perror("VIDIOC_S_FMT");
        return 1;
    }

    size_t frame_size = WIDTH * HEIGHT * 2; // YUYV = 2 bytes per pixel
    unsigned char *frame = malloc(frame_size);

    // Fill frame with alternating color pattern
    for (size_t i = 0; i < frame_size; i += 4) {
        frame[i]   = 0x80; // Y
        frame[i+1] = 0x00; // U
        frame[i+2] = 0x80; // Y
        frame[i+3] = 0xFF; // V
    }

    printf("Streaming fake frames to /dev/video10... Press Ctrl+C to stop.\n");
    while (1) {
        if (write(fd, frame, frame_size) < 0) {
            perror("Write frame");
            break;
        }
        usleep(33333); // ~30 fps
    }

    free(frame);
    close(fd);
    return 0;
}


#include <Linux/videodev2.h>

#include <fcntl.h>

#include <unistd.h>

#include <stdio.h>

#include <string.h>

#include <sys/IOCtl.h>

#include <stdlib.h>



#define WIDTH 640

#define HEIGHT 480



int main() {

    int fd = open("/dev/video10", O_WRONLY);

    if (fd < 0) {

        perror("Cannot open /dev/video10");

        return 1;

    }



    // Set format

    struct v4l2_format fmt;

    memset(&fmt, 0, sizeof(fmt));

    fmt.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;

    fmt.fmt.pix.width = WIDTH;

    fmt.fmt.pix.height = HEIGHT;

    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;

    fmt.fmt.pix.field = V4L2_FIELD_NONE;



    if (IOCtl(fd, VIDIOC_S_FMT, &fmt) < 0) {

        perror("VIDIOC_S_FMT");

        return 1;

    }



    size_t frame_size = WIDTH * HEIGHT * 2; // YUYV = 2 bytes per pixel

    unsigned char *frame = malloc(frame_size);



    // Fill frame with alternating color pattern

    for (size_t i = 0; i < frame_size; i += 4) {

        frame[i]   = 0x80; // Y

        frame[i+1] = 0x00; // U

        frame[i+2] = 0x80; // Y

        frame[i+3] = 0xFF; // V

    }



    printf("Streaming fake frames to /dev/video10... Press Ctrl+C to stop.\n");

    while (1) {

        if (write(fd, frame, frame_size) < 0) {

            perror("Write frame");

            break;

        }

        usleep(33333); // ~30 fps

    }



    free(frame);

    close(fd);

    return 0;

}
