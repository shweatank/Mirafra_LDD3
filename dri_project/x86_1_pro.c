#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <alsa/asoundlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define UART_DEV "/dev/ttyUSB1"
#define TEMP_WAV "recorded.wav"
#define SERVER_IP "192.168.0.153"   // Replace with Raspberry Pi IP
#define SERVER_PORT 9090

#define RECORD_SECONDS 6
#define SAMPLE_RATE 44100
#define CHANNELS 1
#define BITS_PER_SAMPLE 16

void setup_uart(int fd) {
    struct termios tty;
    if (tcgetattr(fd, &tty) != 0) {
        perror("tcgetattr failed");
        return;
    }

    cfsetospeed(&tty, B115200);
    cfsetispeed(&tty, B115200);

    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;       // 8-bit
    tty.c_cflag &= ~PARENB;   // No parity
    tty.c_cflag &= ~CSTOPB;   // 1 stop bit
    tty.c_cflag &= ~CRTSCTS;  // No hardware flow control

  //  tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);  // Raw input
  //  tty.c_iflag &= ~(IXON | IXOFF | IXANY);          // No flow control
  //  tty.c_oflag &= ~OPOST;                           // Raw output

 //   tty.c_cc[VMIN] = 0;
 //   tty.c_cc[VTIME] = 10;  // 1 second timeout

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        perror("tcsetattr failed");
    }
}

int record_audio(const char *filename) {
    snd_pcm_t *pcm_handle;
    snd_pcm_hw_params_t *params;
    unsigned int sample_rate = SAMPLE_RATE;
    int rc, dir;
    int buffer_frames = 32;
    int size;
    snd_pcm_uframes_t frames = 32;

    short channels = CHANNELS;
    short bits_per_sample = BITS_PER_SAMPLE;
    short audio_format = 1; // PCM
    int byte_rate = SAMPLE_RATE * CHANNELS * BITS_PER_SAMPLE / 8;
    short block_align = CHANNELS * BITS_PER_SAMPLE / 8;

    char *buffer;
    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        perror("File open failed");
        return -1;
    }

    // Open ALSA device
    rc = snd_pcm_open(&pcm_handle, "default", SND_PCM_STREAM_CAPTURE, 0);
    if (rc < 0) {
        fprintf(stderr, "Unable to open PCM device: %s\n", snd_strerror(rc));
        fclose(fp);
        return -1;
    }

    // Set HW parameters
    snd_pcm_hw_params_alloca(&params);
    snd_pcm_hw_params_any(pcm_handle, params);
    snd_pcm_hw_params_set_access(pcm_handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(pcm_handle, params, SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_channels(pcm_handle, params, CHANNELS);
    snd_pcm_hw_params_set_rate_near(pcm_handle, params, &sample_rate, &dir);
    snd_pcm_hw_params_set_period_size_near(pcm_handle, params, &frames, &dir);
    snd_pcm_hw_params(pcm_handle, params);

    snd_pcm_hw_params_get_period_size(params, &frames, &dir);
    size = frames * CHANNELS * BITS_PER_SAMPLE / 8;
    buffer = (char *) malloc(size);

    int total_frames = SAMPLE_RATE * RECORD_SECONDS;
    int total_data_bytes = total_frames * CHANNELS * BITS_PER_SAMPLE / 8;

    // WAV header (44 bytes)
    fwrite("RIFF", 1, 4, fp);
    int chunk_size = 36 + total_data_bytes;
    fwrite(&chunk_size, 4, 1, fp);
    fwrite("WAVE", 1, 4, fp);
    fwrite("fmt ", 1, 4, fp);
    int subchunk1_size = 16;
    fwrite(&subchunk1_size, 4, 1, fp);
    fwrite(&audio_format, 2, 1, fp);
    fwrite(&channels, 2, 1, fp);
    fwrite(&sample_rate, 4, 1, fp);
    fwrite(&byte_rate, 4, 1, fp);
    fwrite(&block_align, 2, 1, fp);
    fwrite(&bits_per_sample, 2, 1, fp);
    fwrite("data", 1, 4, fp);
    fwrite(&total_data_bytes, 4, 1, fp);

    printf("Recording audio...\n");
    int frames_recorded = 0;
    while (frames_recorded < total_frames) {
        rc = snd_pcm_readi(pcm_handle, buffer, frames);
        if (rc == -EPIPE) {
            snd_pcm_prepare(pcm_handle);
            continue;
        } else if (rc < 0) {
            fprintf(stderr, "Read error: %s\n", snd_strerror(rc));
        } else {
            fwrite(buffer, CHANNELS * BITS_PER_SAMPLE / 8, rc, fp);
            frames_recorded += rc;
        }
    }
    fclose(fp);
    snd_pcm_state_t state = snd_pcm_state(pcm_handle);
    printf("PCM state: %s\n", snd_pcm_state_name(state));
    // Immediately stop PCM without waiting
    snd_pcm_drop(pcm_handle);
    snd_pcm_close(pcm_handle);
    free(buffer);
    printf("doneee..\n");
    return 0;
}

int send_file_over_tcp(const char *filename) {
    int sockfd;
    struct sockaddr_in server_addr;
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        perror("Failed to open WAV file");
        return -1;
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket error");
        fclose(fp);
        return -1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(sockfd);
        fclose(fp);
        return -1;
    }

    char buf[1024];
    size_t r;
    while ((r = fread(buf, 1, sizeof(buf), fp)) > 0) {
        send(sockfd, buf, r, 0);
    }

    fclose(fp);
    close(sockfd);
    printf("Sent audio to Raspberry Pi\n");
    return 0;
}

int main()
{

    int fd = open(UART_DEV, O_RDWR | O_NOCTTY);
    if (fd < 0) {
        perror("Failed to open UART");
        return 1;
    }
    //setup_uart(fd);

    char start_buf[16] = {0};
    printf("Waiting for 'start' from Raspberry Pi...\n");
    while(read(fd, start_buf, 5)<=0);
    printf("Received UART command: %s\n", start_buf);

    if (strncmp(start_buf, "start", 5) == 0) {
        if (record_audio(TEMP_WAV) == 0) {
            send_file_over_tcp(TEMP_WAV);
        }
    }

    close(fd);
   
//	printf("started..\n");
//	record_audio(TEMP_WAV);
    return 0;
}
