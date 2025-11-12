#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <pthread.h>
#include <fcntl.h>
#include <termios.h>
#include <alsa/asoundlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>

#define UART_DEV "/dev/serial0"
#define BAUD B921600

#define LCD_DEV "/dev/ili9225"
#define WIDTH 176
#define HEIGHT 220
#define FRAME_SIZE (WIDTH*HEIGHT*2)

#define AUDIO_RATE 44100
#define AUDIO_CHANNELS 2
#define AUDIO_BUFFER 1024

// ---------------- UART Setup ----------------
int open_uart(const char *dev, speed_t baud) {
    int fd = open(dev, O_RDWR | O_NOCTTY);
    if (fd < 0) { perror("open uart"); return -1; }

    struct termios tty;
    tcgetattr(fd, &tty);
    cfmakeraw(&tty);
    cfsetospeed(&tty, baud);
    cfsetispeed(&tty, baud);
    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_cflag &= ~CRTSCTS;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;
    tcsetattr(fd, TCSANOW, &tty);
    return fd;
}

// ---------------- Video Thread ----------------
void *video_thread(void *arg) {
    int uart_fd = *(int*)arg;

    int lcd_fd = open(LCD_DEV, O_WRONLY);
    if (lcd_fd < 0) { perror("open lcd"); return NULL; }

    // ffmpeg reads from stdin (UART) and outputs RGB565 raw video
    char cmd[512];
    snprintf(cmd, sizeof(cmd),
        "ffmpeg -hide_banner -loglevel error -i - -vf scale=%d:%d -f rawvideo -pix_fmt rgb565 -r 30 -",
        WIDTH, HEIGHT);

    FILE *pipe = popen(cmd, "w");
    if (!pipe) { perror("popen video"); close(lcd_fd); return NULL; }

    unsigned char buffer[4096];
    ssize_t r;
    while ((r = read(uart_fd, buffer, sizeof(buffer))) > 0) {
        fwrite(buffer, 1, r, pipe);
        fflush(pipe);
    }

    pclose(pipe);
    close(lcd_fd);
    return NULL;
}

// ---------------- Audio Thread ----------------
void *audio_thread(void *arg) {
    int uart_fd = *(int*)arg;

    snd_pcm_t *handle;
    snd_pcm_hw_params_t *params;

    if (snd_pcm_open(&handle, "default", SND_PCM_STREAM_PLAYBACK, 0) < 0) { perror("snd_pcm_open"); return NULL; }

    snd_pcm_hw_params_malloc(&params);
    snd_pcm_hw_params_any(handle, params);
    snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_channels(handle, params, AUDIO_CHANNELS);
    snd_pcm_hw_params_set_rate(handle, params, AUDIO_RATE, 0);
    snd_pcm_hw_params(handle, params);
    snd_pcm_hw_params_free(params);
    snd_pcm_prepare(handle);

    // ffmpeg reads from stdin (UART) and outputs PCM S16LE audio
    char cmd[512];
    snprintf(cmd, sizeof(cmd),
        "ffmpeg -hide_banner -loglevel error -i - -f s16le -acodec pcm_s16le -ac %d -ar %d -",
        AUDIO_CHANNELS, AUDIO_RATE);

    FILE *pipe = popen(cmd, "w");
    if (!pipe) { perror("popen audio"); snd_pcm_close(handle); return NULL; }

    unsigned char buffer[4096];
    ssize_t r;
    int16_t audio_buf[AUDIO_BUFFER*AUDIO_CHANNELS];

    while ((r = read(uart_fd, buffer, sizeof(buffer))) > 0) {
        fwrite(buffer, 1, r, pipe);
        fflush(pipe);

        // Also write to ALSA if decoded here
        // fread() or decoding would be required if you process PCM directly
    }

    pclose(pipe);
    snd_pcm_drain(handle);
    snd_pcm_close(handle);
    return NULL;
}

// ---------------- Main ----------------
int main() {
    int uart_fd = open_uart(UART_DEV, BAUD);
    if (uart_fd < 0) return 1;

    pthread_t vid_th, aud_th;
    pthread_create(&vid_th, NULL, video_thread, &uart_fd);
    pthread_create(&aud_th, NULL, audio_thread, &uart_fd);

    pthread_join(vid_th, NULL);
    pthread_join(aud_th, NULL);

    close(uart_fd);
    return 0;
}

