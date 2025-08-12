// v4l2_capture_send.c
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include <sys/mman.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>

#define WIDTH 128
#define HEIGHT 64

struct buffer {
    void   *start;
    size_t length;
};

#pragma pack(push, 1)
typedef struct {
    unsigned short bfType;      // BM
    unsigned int bfSize;
    unsigned short bfReserved1;
    unsigned short bfReserved2;
    unsigned int bfOffBits;
} BITMAPFILEHEADER;

typedef struct {
    unsigned int biSize;  
    int  biWidth;
    int  biHeight;
    unsigned short biPlanes;
    unsigned short biBitCount;
    unsigned int biCompression;
    unsigned int biSizeImage;
    int  biXPelsPerMeter;
    int  biYPelsPerMeter;
    unsigned int biClrUsed;
    unsigned int biClrImportant;
} BITMAPINFOHEADER;
#pragma pack(pop)

int write_bmp(const char *filename, unsigned char *rgb_data, int width, int height) {
    BITMAPFILEHEADER file_header;
    BITMAPINFOHEADER info_header;
    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        perror("fopen");
        return -1;
    }

    int row_padded = (width*3 + 3) & (~3); // rows padded to 4 bytes
    int data_size = row_padded * height;

    file_header.bfType = 0x4D42;  // 'BM'
    file_header.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + data_size;
    file_header.bfReserved1 = 0;
    file_header.bfReserved2 = 0;
    file_header.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    info_header.biSize = sizeof(BITMAPINFOHEADER);
    info_header.biWidth = width;
    info_header.biHeight = height;
    info_header.biPlanes = 1;
    info_header.biBitCount = 24;  // 3 bytes per pixel
    info_header.biCompression = 0; // BI_RGB no compression
    info_header.biSizeImage = data_size;
    info_header.biXPelsPerMeter = 0;
    info_header.biYPelsPerMeter = 0;
    info_header.biClrUsed = 0;
    info_header.biClrImportant = 0;

    fwrite(&file_header, sizeof(file_header), 1, fp);
    fwrite(&info_header, sizeof(info_header), 1, fp);

    // BMP stores pixels bottom-up
    unsigned char *row = malloc(row_padded);
    if (!row) {
        fclose(fp);
        return -1;
    }

    for (int y = height - 1; y >= 0; y--) {
        // Prepare row with padding
        for (int x = 0; x < width; x++) {
            // BMP pixel order: B G R
            row[x*3 + 0] = rgb_data[(y*width + x)*3 + 2];  // B
            row[x*3 + 1] = rgb_data[(y*width + x)*3 + 1];  // G
            row[x*3 + 2] = rgb_data[(y*width + x)*3 + 0];  // R
        }
        // padding bytes = 0
        for (int p = width*3; p < row_padded; p++)
            row[p] = 0;

        fwrite(row, 1, row_padded, fp);
    }

    free(row);
    fclose(fp);
    return 0;
}

int main() {
    int fd = open("/dev/video0", O_RDWR);
    if (fd < 0) {
        perror("Cannot open device");
        return 1;
    }

    // Query device capabilities
    struct v4l2_capability cap;
    if (ioctl(fd, VIDIOC_QUERYCAP, &cap) < 0) {
        perror("VIDIOC_QUERYCAP");
        return 1;
    }
    if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
        fprintf(stderr, "Not a video capture device\n");
        return 1;
    }

    // Set format
    struct v4l2_format fmt = {0};
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = WIDTH;
    fmt.fmt.pix.height = HEIGHT;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB24;  // Use RGB24 to simplify BMP write
    fmt.fmt.pix.field = V4L2_FIELD_NONE;

    if (ioctl(fd, VIDIOC_S_FMT, &fmt) < 0) {
        perror("VIDIOC_S_FMT");
        return 1;
    }

    // Request buffer
    struct v4l2_requestbuffers req = {0};
    req.count = 1;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    if (ioctl(fd, VIDIOC_REQBUFS, &req) < 0) {
        perror("VIDIOC_REQBUFS");
        return 1;
    }
    if (req.count < 1) {
        fprintf(stderr, "Insufficient buffer memory\n");
        return 1;
    }

    // Map buffer
    struct buffer buf;
    struct v4l2_buffer bufferinfo = {0};
    bufferinfo.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    bufferinfo.memory = V4L2_MEMORY_MMAP;
    bufferinfo.index = 0;

    buf.length = req.count;
    buf.start = mmap(NULL, fmt.fmt.pix.sizeimage, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (buf.start == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    // Queue the buffer
    if (ioctl(fd, VIDIOC_QBUF, &bufferinfo) < 0) {
        perror("VIDIOC_QBUF");
        return 1;
    }

    // Start streaming
    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(fd, VIDIOC_STREAMON, &type) < 0) {
        perror("VIDIOC_STREAMON");
        return 1;
    }

    // Dequeue the buffer (capture frame)
    if (ioctl(fd, VIDIOC_DQBUF, &bufferinfo) < 0) {
        perror("VIDIOC_DQBUF");
        return 1;
    }

    // Save captured frame to BMP file
    unsigned char *frame_data = (unsigned char*)buf.start;
    // frame_data contains RGB24 data with size fmt.fmt.pix.sizeimage
    if (write_bmp("frame.bmp", frame_data, WIDTH, HEIGHT) < 0) {
        fprintf(stderr, "Failed to write BMP\n");
        return 1;
    }

    // Stop streaming
    ioctl(fd, VIDIOC_STREAMOFF, &type);
    munmap(buf.start, fmt.fmt.pix.sizeimage);
    close(fd);

    // --- Now send the BMP file over socket to Raspberry Pi ---

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return 1;
    }

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(5000);
    inet_pton(AF_INET, "192.168.0.143", &serv_addr.sin_addr); // replace with your Pi IP

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("connect");
        return 1;
    }

    FILE *fp = fopen("frame.bmp", "rb");
    if (!fp) {
        perror("fopen frame.bmp");
        return 1;
    }

    // Get file size
    fseek(fp, 0, SEEK_END);
    long filesize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    // Send filesize first
    uint32_t fsize_net = htonl(filesize);
    if (send(sockfd, &fsize_net, sizeof(fsize_net), 0) < 0) {
        perror("send filesize");
        fclose(fp);
        close(sockfd);
        return 1;
    }

    // Send file content
    char buffer[1024];
    size_t read_bytes;
    while ((read_bytes = fread(buffer, 1, sizeof(buffer), fp)) > 0) {
        if (send(sockfd, buffer, read_bytes, 0) < 0) {
            perror("send file data");
            fclose(fp);
            close(sockfd);
            return 1;
        }
    }

    fclose(fp);
    close(sockfd);

    printf("Frame sent successfully\n");
    return 0;
}

