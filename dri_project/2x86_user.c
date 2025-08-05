#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <alsa/asoundlib.h>
#include <termios.h>
#include <errno.h>

#define SERVER_PORT 8080
#define AUDIO_FILE "recorded.wav"
#define UART_DEVICE "/dev/ttyUSB0"
#define BUFFER_SIZE 4096

int receive_audio_file(int server_port, const char *filename) {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addrlen = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        perror("fopen");
        return -1;
    }

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        fclose(fp);
        return -1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(server_port);

    bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_fd, 1);

    printf("Waiting for audio data...\n");
    client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &addrlen);
    printf("Connected to client\n");

    int n;
    while ((n = read(client_fd, buffer, BUFFER_SIZE)) > 0) {
        fwrite(buffer, 1, n, fp);
    }

    fclose(fp);
    close(client_fd);
    close(server_fd);
    printf("Audio file received.\n");
    return 0;
}

void play_audio(const char *filename) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        perror("fopen");
        return;
    }

    fseek(fp, 44, SEEK_SET);  // skip WAV header

    snd_pcm_t *pcm;
    if (snd_pcm_open(&pcm, "default", SND_PCM_STREAM_PLAYBACK, 0) < 0) {
        fprintf(stderr, "Error opening PCM device\n");
        fclose(fp);
        return;
    }

    if (snd_pcm_set_params(pcm,
        SND_PCM_FORMAT_S16_LE,
        SND_PCM_ACCESS_RW_INTERLEAVED,
        1, 44100, 1, 500000) < 0) {
        fprintf(stderr, "Error setting PCM params\n");
        snd_pcm_close(pcm);
        fclose(fp);
        return;
    }

    short buffer[BUFFER_SIZE];
    int frames;

    while (!feof(fp)) {
        size_t len = fread(buffer, 1, sizeof(buffer), fp);
        frames = snd_pcm_writei(pcm, buffer, len / 2);  // 2 bytes * 2 channels
        if (frames < 0)
            frames = snd_pcm_recover(pcm, frames, 0);
    }

    snd_pcm_drain(pcm);
    snd_pcm_close(pcm);
    fclose(fp);
    printf("Playback complete.\n");
}

int send_uart_ack(const char *dev, const char *ack_msg) {
    int uart_fd = open(dev, O_RDWR | O_NOCTTY | O_SYNC);
    if (uart_fd < 0) {
        perror("open UART");
        return -1;
    }

    struct termios tty;
    if (tcgetattr(uart_fd, &tty) != 0) {
        perror("tcgetattr");
        close(uart_fd);
        return -1;
    }

    cfsetospeed(&tty, B115200);
    cfsetispeed(&tty, B115200);
    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;
    tty.c_iflag &= ~IGNBRK;
    tty.c_lflag = 0;
    tty.c_oflag = 0;
    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 1;
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);
    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_cflag &= ~(PARENB | PARODD);
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;

    if (tcsetattr(uart_fd, TCSANOW, &tty) != 0) {
        perror("tcsetattr");
        close(uart_fd);
        return -1;
    }

    write(uart_fd, ack_msg, strlen(ack_msg));
    tcdrain(uart_fd);
    close(uart_fd);
    return 0;
}

int main() {
    if (receive_audio_file(SERVER_PORT, AUDIO_FILE) == 0) {
        play_audio(AUDIO_FILE);

        if (send_uart_ack(UART_DEVICE, "complete\n") == 0) {
            printf("ACK sent to Raspberry Pi via UART.\n");
        } else {
            fprintf(stderr, "Failed to send UART ACK\n");
        }
    }

    return 0;
}

