#include <stdio.h>
#include <fcntl.h>
#include<stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include <errno.h>

#define WIDTH 640
#define HEIGHT 480
#define FRAME_SIZE (WIDTH * HEIGHT * 2)  // YUYV format (2 bytes per pixel)

int main() {
    const char *device = "/dev/video2"; // v4l2loopback device
    int fd = open(device, O_WRONLY);
    if (fd < 0) {
        perror("Failed to open video device");
        return 1;
    }
    printf("Opened device %s\n", device);

    // Set the video format
    struct v4l2_format format;
    memset(&format, 0, sizeof(format));
    format.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
    format.fmt.pix.width = WIDTH;
    format.fmt.pix.height = HEIGHT;
    format.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    format.fmt.pix.field = V4L2_FIELD_NONE;

    if (ioctl(fd, VIDIOC_S_FMT, &format) == -1) {
        perror("VIDIOC_S_FMT");
        close(fd);
        return 1;
    }
    printf("Format set: %dx%d, YUYV\n", WIDTH, HEIGHT);

    // Open the YUV file
    FILE *fp = fopen("video.yuv", "rb");
    if (!fp) {
        perror("Cannot open YUV file");
        close(fd);
        return 1;
    }
    printf("Opened YUV file\n");

    char buffer[FRAME_SIZE];
    size_t frame_count = 0;
    if(fork()==0)
    {
	system("ffplay /dev/video2");
    }
	else
	{
    while (fread(buffer, 1, FRAME_SIZE, fp) == FRAME_SIZE) {
        ssize_t written = write(fd, buffer, FRAME_SIZE);
        if (written < 0) {
            perror("Write failed");
            break;
        } else if (written != FRAME_SIZE) {
            fprintf(stderr, "Partial write: %ld bytes\n", written);
        }
        frame_count++;
       // printf("Wrote frame %zu\n", frame_count);
        usleep(33333);  // ~30 FPS
    }

    printf("Finished writing %zu frames\n", frame_count);
    fclose(fp);
    close(fd);
	}
    return 0;
}

