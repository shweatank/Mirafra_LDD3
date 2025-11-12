#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include <errno.h>

int main(int argc, char *argv[])
{
    if (argc != 3) {
        printf("Usage: %s /dev/videoX vblank_value\n", argv[0]);
        return -1;
    }

    const char *dev_name = argv[1];
    int vblank_val = atoi(argv[2]);

    int fd = open(dev_name, O_RDWR);
    if (fd < 0) {
        perror("Cannot open device");
        return -1;
    }

    // Set vertical blanking
    struct v4l2_control ctrl;
    ctrl.id = V4L2_CID_VBLANK; // Vertical blanking control
    ctrl.value = vblank_val;

    if (ioctl(fd, VIDIOC_S_CTRL, &ctrl) < 0) {
        perror("VIDIOC_S_CTRL failed");
        close(fd);
        return -1;
    }

    printf("Vertical blanking set to %d\n", vblank_val);

    // Optionally, read it back to verify
    if (ioctl(fd, VIDIOC_G_CTRL, &ctrl) == 0) {
        printf("Verified vertical blanking = %d\n", ctrl.value);
    }

    close(fd);
    return 0;
}

