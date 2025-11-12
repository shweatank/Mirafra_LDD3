// get_default_fps.c
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>

int main() {
    const char *device = "/dev/video10";
    int fd = open(device, O_RDWR);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    struct v4l2_streamparm parm;
    memset(&parm, 0, sizeof(parm));
    parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if (ioctl(fd, VIDIOC_G_PARM, &parm) == 0) {
        if (parm.parm.capture.timeperframe.denominator &&
            parm.parm.capture.timeperframe.numerator) {

            double fps = (double)parm.parm.capture.timeperframe.denominator /
                         (double)parm.parm.capture.timeperframe.numerator;

            printf("Default FPS: %.2f\n", fps);
        } else {
            printf("Driver did not report valid FPS info.\n");
        }
    } else {
        perror("VIDIOC_G_PARM");
    }

    close(fd);
    return 0;
}

