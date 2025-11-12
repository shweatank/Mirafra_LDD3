#include <stdio.h>              // Standard I/O
#include <stdlib.h>             // malloc, exit
#include <stdint.h>             // uint8_t, uint32_t
#include <string.h>             // memset, memcpy
#include <unistd.h>             // read, write, close
#include <arpa/inet.h>          // htons, htonl, ntohl, sockaddr_in
#include <fcntl.h>              // open, O_WRONLY
#include <sys/socket.h>         // socket(), bind(), listen(), accept()
#include <sys/types.h>          // system data types for sockets
#include <alsa/asoundlib.h>     // ALSA sound API

// LCD resolution (matches sender’s resize)
#define LCD_WIDTH   176
#define LCD_HEIGHT  220
#define FRAME_SIZE  (LCD_WIDTH * LCD_HEIGHT * 2)  // RGB565 (2 bytes per pixel)

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <port> <lcd_device>\n", argv[0]);
        return 1;
    }

    // -----------------------------
    // Parse arguments
    // -----------------------------
    int port = atoi(argv[1]);      // TCP port to listen on
    const char *lcd_dev = argv[2]; // LCD device node (e.g., /dev/fb0 or custom driver)

    // -----------------------------
    // Open LCD device
    // -----------------------------
    int lcd_fd = open(lcd_dev, O_WRONLY);
    if (lcd_fd < 0) {
        perror("open lcd device");
        return 1;
    }

    // -----------------------------
    // Setup TCP socket (Receiver side)
    // -----------------------------
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) { perror("socket"); return 1; }

    // sockaddr_in → structure for IPv4 addresses
    struct sockaddr_in srv;
    /*
        struct sockaddr_in {
            sa_family_t    sin_family;   // Address family (AF_INET for IPv4)
            in_port_t      sin_port;     // Port number (network byte order)
            struct in_addr sin_addr;     // IP address (struct in_addr has s_addr)
            unsigned char  sin_zero[8];  // Padding (unused, set to 0)
        };
    */
    memset(&srv, 0, sizeof(srv));
    srv.sin_family = AF_INET;         // IPv4
    srv.sin_addr.s_addr = INADDR_ANY; // Listen on all interfaces
    srv.sin_port = htons(port);       // Convert port to network byte order

    if (bind(listen_fd, (struct sockaddr*)&srv, sizeof(srv)) < 0) {
        perror("bind");
        return 1;
    }

    listen(listen_fd, 1); // Listen with a backlog of 1 connection
    printf("Waiting for sender on port %d...\n", port);

    // Accept one connection
    int conn = accept(listen_fd, NULL, NULL);
    if (conn < 0) { perror("accept"); return 1; }
    printf("Sender connected\n");

    // -----------------------------
    // ALSA setup for audio playback
    // -----------------------------
    snd_pcm_t *pcm_handle;             // PCM device handle
    snd_pcm_hw_params_t *params;       // Hardware params structure
    unsigned int rate = 16000;         // Sampling rate (Hz)
    int dir;
    snd_pcm_uframes_t frames = 1024;   // Buffer size in frames

    /*
       snd_pcm_open → Opens a PCM device
       "plughw:2,0" → ALSA device name:
           - "hw:2,0" is card 2, device 0
           - "plughw:" allows automatic conversions
       SND_PCM_STREAM_PLAYBACK → output mode
    */
    if (snd_pcm_open(&pcm_handle, "plughw:2,0", SND_PCM_STREAM_PLAYBACK, 0) < 0) {
        fprintf(stderr, "Error opening ALSA device\n");
        return 1;
    }

    snd_pcm_hw_params_alloca(&params);                // Allocate params object on stack
    snd_pcm_hw_params_any(pcm_handle, params);        // Fill it with default values

    // Configure audio stream format
    snd_pcm_hw_params_set_access(pcm_handle, params, SND_PCM_ACCESS_RW_INTERLEAVED); // Interleaved samples
    snd_pcm_hw_params_set_format(pcm_handle, params, SND_PCM_FORMAT_S16_LE);         // 16-bit signed little-endian
    snd_pcm_hw_params_set_channels(pcm_handle, params, 1);                           // Mono channel
    snd_pcm_hw_params_set_rate_near(pcm_handle, params, &rate, &dir);                // Sampling rate ~16kHz
    snd_pcm_hw_params_set_period_size_near(pcm_handle, params, &frames, &dir);       // Buffer size

    if (snd_pcm_hw_params(pcm_handle, params) < 0) {
        fprintf(stderr, "Error setting ALSA HW params\n");
        return 1;
    }

    // -----------------------------
    // Allocate buffer for video frames
    // -----------------------------
    uint8_t *framebuf = malloc(FRAME_SIZE);
    if (!framebuf) {
        perror("malloc");
        close(conn);
        return 1;
    }

    // -----------------------------
    // Main receive loop
    // -----------------------------
    while (1) {
        // ---- Step 1: Read packet length header (4 bytes) ----
        uint32_t netlen;
        int r = recv(conn, &netlen, 4, MSG_WAITALL);
        if (r <= 0) break; // connection closed
        int payload = ntohl(netlen); // Convert from network byte order

        // ---- Step 2: Video frame ----
        if (payload == FRAME_SIZE) {
            // Receive exactly FRAME_SIZE bytes of video
            r = recv(conn, framebuf, FRAME_SIZE, MSG_WAITALL);
            if (r <= 0) break;
            printf("Received video frame: %d bytes\n", r);

            // Write to LCD device (driver must accept RGB565 raw data)
            int written = write(lcd_fd, framebuf, FRAME_SIZE);
            if (written < 0) {
                perror("write to lcd");
                break;
            }
        }
        // ---- Step 3: Audio chunk ----
        else {
            // Dynamically allocate audio buffer for this chunk
            uint8_t *audiobuf = malloc(payload);
            if (!audiobuf) { perror("malloc audio"); break; }
            r = recv(conn, audiobuf, payload, MSG_WAITALL);
            if (r <= 0) { free(audiobuf); break; }

            printf("Received audio: %d bytes\n", r);

            // Play audio on ALSA device
            // payload / 2 → number of 16-bit samples (mono)
            int frames_played = snd_pcm_writei(pcm_handle, audiobuf, payload / 2);
            if (frames_played < 0) {
                snd_pcm_prepare(pcm_handle); // Reset ALSA on underrun
            }

            free(audiobuf);
        }
    }

    // -----------------------------
    // Cleanup
    // -----------------------------
    free(framebuf);
    snd_pcm_drain(pcm_handle);  // Wait for audio buffer to finish playing
    snd_pcm_close(pcm_handle);  // Close ALSA
    close(conn);                // Close TCP connection
    close(lcd_fd);              // Close LCD device
    return 0;
}

