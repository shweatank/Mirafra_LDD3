#include <opencv2/opencv.hpp>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>

int main() {
    const char* dev_name = "/dev/video0";
    int fd = open(dev_name, O_RDWR);
    if (fd < 0) { perror("open"); return 1; }

    // Set format
    v4l2_format fmt = {};
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = 640;
    fmt.fmt.pix.height = 480;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_GREY; // safe format
    fmt.fmt.pix.field = V4L2_FIELD_NONE;

    if (ioctl(fd, VIDIOC_S_FMT, &fmt) < 0) {
        perror("VIDIOC_S_FMT");
        close(fd);
        return 1;
    }

    cv::VideoCapture cap(dev_name, cv::CAP_V4L2);
    if (!cap.isOpened()) {
        std::cerr << "Cannot open camera\n";
        return 1;
    }

    // Optional: force format in OpenCV
    cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
    cap.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('G','R','E','Y'));

    std::cout << "Press ESC to quit\n";

    cv::Mat frame;
    while (true) {
        cap >> frame;
        if (frame.empty()) break;

        cv::imshow("OV7251 Live", frame);

        if (cv::waitKey(1) == 27) break; // ESC to exit
    }

    cap.release();
    cv::destroyAllWindows();
    close(fd);
    return 0;
}


