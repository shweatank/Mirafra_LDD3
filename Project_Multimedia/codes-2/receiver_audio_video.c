#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <alsa/asoundlib.h>

#define LCD_WIDTH   176
#define LCD_HEIGHT  220
#define FRAME_SIZE  (LCD_WIDTH * LCD_HEIGHT * 2)  // RGB565

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <port> <lcd_device>\n", argv[0]);
        return 1;
    }

    int port = atoi(argv[1]);
    const char *lcd_dev = argv[2];

    // Open LCD driver device
    int lcd_fd = open(lcd_dev, O_WRONLY);
    if (lcd_fd < 0) {
        perror("open lcd device");
        return 1;
    }

    // Setup TCP socket
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) { perror("socket"); return 1; }

    struct sockaddr_in srv;
    memset(&srv, 0, sizeof(srv));
    srv.sin_family = AF_INET;
    srv.sin_addr.s_addr = INADDR_ANY;
    srv.sin_port = htons(port);

    if (bind(listen_fd, (struct sockaddr*)&srv, sizeof(srv)) < 0) {
        perror("bind");
        return 1;
    }
    listen(listen_fd, 1);
    printf("Waiting for sender on port %d...\n", port);

    int conn = accept(listen_fd, NULL, NULL);
    if (conn < 0) { perror("accept"); return 1; }
    printf("Sender connected\n");

    // Init ALSA for audio playback
    snd_pcm_t *pcm_handle;
    snd_pcm_hw_params_t *params;
    unsigned int rate = 16000;
    int dir;
    snd_pcm_uframes_t frames = 1024;

    if (snd_pcm_open(&pcm_handle, "plughw:2,0", SND_PCM_STREAM_PLAYBACK, 0) < 0) {
        fprintf(stderr, "Error opening ALSA device\n");
        return 1;
    }
    snd_pcm_hw_params_alloca(&params);
    snd_pcm_hw_params_any(pcm_handle, params);
    snd_pcm_hw_params_set_access(pcm_handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(pcm_handle, params, SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_channels(pcm_handle, params, 1); // mono
    snd_pcm_hw_params_set_rate_near(pcm_handle, params, &rate, &dir);
    snd_pcm_hw_params_set_period_size_near(pcm_handle, params, &frames, &dir);
    if (snd_pcm_hw_params(pcm_handle, params) < 0) {
        fprintf(stderr, "Error setting ALSA HW params\n");
        return 1;
    }

    uint8_t *framebuf = malloc(FRAME_SIZE);
    if (!framebuf) {
        perror("malloc");
        close(conn);
        return 1;
    }

    while (1) {
        // Read 4-byte length header
        uint32_t netlen;
        int r = recv(conn, &netlen, 4, MSG_WAITALL);
        if (r <= 0) break;
        int payload = ntohl(netlen);

        // Video frame
        if (payload == FRAME_SIZE) {
            r = recv(conn, framebuf, FRAME_SIZE, MSG_WAITALL);
            if (r <= 0) break;
            printf("Received video frame: %d bytes\n", r);

            int written = write(lcd_fd, framebuf, FRAME_SIZE);
            if (written < 0) {
                perror("write to lcd");
                break;
            }
        }
        // Audio chunk
        else {
            uint8_t *audiobuf = malloc(payload);
            if (!audiobuf) { perror("malloc audio"); break; }
            r = recv(conn, audiobuf, payload, MSG_WAITALL);
            if (r <= 0) { free(audiobuf); break; }

            printf("Received audio: %d bytes\n", r);

            // Play audio
            int frames_played = snd_pcm_writei(pcm_handle, audiobuf, payload / 2); // 16-bit mono
            if (frames_played < 0) {
                snd_pcm_prepare(pcm_handle);
            }

            free(audiobuf);
        }
    }

    free(framebuf);
    snd_pcm_drain(pcm_handle);
    snd_pcm_close(pcm_handle);
    close(conn);
    close(lcd_fd);
    return 0;
}


