#include <stdio.h>              // Standard I/O functions
#include <stdlib.h>             // For exit(), malloc(), free()
#include <string.h>             // String manipulation functions
#include <unistd.h>             // For read(), write(), close(), sleep()
#include <arpa/inet.h>          // For socket operations and sockaddr_in
#include <fcntl.h>              // For open(), O_RDWR flags
#include <alsa/asoundlib.h>     // ALSA sound library for audio playback
#include <termios.h>            // For UART serial port settings
#include <errno.h>              // Error number macros

// Macro definitions for server port, file name, UART device, and buffer size
#define SERVER_PORT 9090
#define AUDIO_FILE "recorded.wav"
#define UART_DEVICE "/dev/ttyUSB0"
#define BUFFER_SIZE 4096

int receive_audio_file(int server_port, const char *filename) {
    int server_fd, client_fd;  // File descriptors for server and client socket
    struct sockaddr_in server_addr, client_addr;  // Address structures
    socklen_t addrlen = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    // Open file to store received audio
    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        perror("fopen");
        return -1;
    }

    // Create TCP socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        fclose(fp);
        return -1;
    }

    // Zero out the server address structure and assign values
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;  // Bind to all interfaces
    server_addr.sin_port = htons(server_port); // Convert port to network byte order

    // Bind socket to port and listen for connections
    bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_fd, 1);

    printf("Waiting for audio data...\n");
    // Accept incoming client connection
    client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &addrlen);
    printf("Connected to client\n");

    // Read data from client and write to file
    int n;
    while ((n = read(client_fd, buffer, BUFFER_SIZE)) > 0) {
        fwrite(buffer, 1, n, fp);
    }

    // Clean up
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

    fseek(fp, 44, SEEK_SET);  // Skip WAV file header (44 bytes)

    snd_pcm_t *pcm;  // PCM handle
    // Open default PCM playback device
    if (snd_pcm_open(&pcm, "default", SND_PCM_STREAM_PLAYBACK, 0) < 0) {
        fprintf(stderr, "Error opening PCM device\n");
        fclose(fp);
        return;
    }

    // Set PCM parameters: 16-bit little endian, mono, 44100Hz, 0.5s latency
    if (snd_pcm_set_params(pcm,
        SND_PCM_FORMAT_S16_LE,
        SND_PCM_ACCESS_RW_INTERLEAVED,
        1, 44100, 1, 500000) < 0) {
        fprintf(stderr, "Error setting PCM params\n");
        snd_pcm_close(pcm);
        fclose(fp);
        return;
    }

    short buffer[BUFFER_SIZE];  // Short = 16-bit PCM samples
    int frames;

    // Read audio samples from file and write to PCM device
    while (!feof(fp)) {
        size_t len = fread(buffer, 1, sizeof(buffer), fp);
        frames = snd_pcm_writei(pcm, buffer, len / 2);  // len/2 since each frame = 2 bytes
        if (frames < 0)
            frames = snd_pcm_recover(pcm, frames, 0);
    }

    // Finalize audio playback
    snd_pcm_drain(pcm);
    snd_pcm_close(pcm);
    fclose(fp);
    printf("Playback complete.\n");
}

int send_uart_ack(const char *dev, const char *ack_msg) {
    // Open UART device with read-write, no controlling terminal, synchronous
    int uart_fd = open(dev, O_RDWR | O_NOCTTY | O_SYNC);
    if (uart_fd < 0) {
        perror("open UART");
        return -1;
    }

    struct termios tty;  // Terminal control structure
    if (tcgetattr(uart_fd, &tty) != 0) {
        perror("tcgetattr");
        close(uart_fd);
        return -1;
    }

    // Set baud rate for input and output
    cfsetospeed(&tty, B115200);
    cfsetispeed(&tty, B115200);

    // Configure UART settings: 8N1, no flow control
    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;       // 8 data bits
    tty.c_cflag &= ~PARENB;   // No parity bit
    tty.c_cflag &= ~CSTOPB;   // 1 stop bit
    tty.c_cflag &= ~CRTSCTS;  // Disable hardware flow control

    // Raw mode: disable canonical input, echo, signal interpretation
    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);  // Disable software flow control
    tty.c_oflag &= ~OPOST;                   // Disable output processing

    // Optional: timeout settings
    // tty.c_cc[VMIN] = 0;
    // tty.c_cc[VTIME] = 10;  // 1-second read timeout

    // Apply UART configuration
    if (tcsetattr(uart_fd, TCSANOW, &tty) != 0) {
        perror("tcsetattr");
        close(uart_fd);
        return -1;
    }

    // Send acknowledgment message
    write(uart_fd, ack_msg, strlen(ack_msg));
    tcdrain(uart_fd);  // Wait until all output has been transmitted
    close(uart_fd);    // Close UART file descriptor
    return 0;
}

int main() {
    // Step 1: Receive .wav file over socket
    if (receive_audio_file(SERVER_PORT, AUDIO_FILE) == 0) {

        // Step 2: Play the received audio file using ALSA
        play_audio(AUDIO_FILE);

        // Step 3: Send UART acknowledgment message
        if (send_uart_ack(UART_DEVICE, "complete\n") == 0) {
            printf("ACK sent to Raspberry Pi via UART.\n");
        } else {
            fprintf(stderr, "Failed to send UART ACK\n");
        }
    }

    return 0;
}

