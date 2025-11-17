// Standard headers
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>           // For open()
#include <string.h>          // For memset, strlen, memcpy
#include <unistd.h>          // For close(), usleep(), read(), write()

// ALSA library for audio output
#include <alsa/asoundlib.h>
#include <math.h>            // For sine wave generation

// I2C and ioctl for OLED
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>

// PWM-related paths and definitions
#define PERIOD_NS 1000                          // PWM period in nanoseconds
#define PWM_CHIP "/sys/class/pwm/pwmchip0"
#define PWM_EXPORT PWM_CHIP "/export"
#define PWM_UNEXPORT PWM_CHIP "/unexport"
#define PWM0_DIR PWM_CHIP "/pwm0"
#define PWM0_PERIOD PWM0_DIR "/period"
#define PWM0_DUTY PWM0_DIR "/duty_cycle"
#define PWM0_ENABLE PWM0_DIR "/enable"

// I2C device and OLED parameters
#define I2C_DEV "/dev/i2c-1"
#define OLED_ADDR 0x3C
#define OLED_WIDTH 128
#define OLED_HEIGHT 64

// Frequency and duty cycle maps for digits 0–9
int freq_map[10] = {15000, 200, 1844, 3488, 5133, 6777, 8422, 10066, 11711, 13355};
int duty_map[10] = {100, 10, 20, 30, 40, 50, 60, 70, 80, 90};

// Global I2C file descriptor
int i2c_fd;

// Helper function to write to a sysfs file (PWM control)
int write_sysfs(const char *path, const char *value) {
    int fd = open(path, O_WRONLY);             // Open file for writing
    if (fd < 0) {
        perror(path);                          // Print error if open fails
        return -1;
    }
    write(fd, value, strlen(value));           // Write value to sysfs file
    close(fd);                                 // Close file
    return 0;
}

// Setup PWM: export the PWM channel, set period, and enable
void setup_pwm() {
    write_sysfs(PWM_EXPORT, "0");              // Export PWM0
    usleep(100000);                            // Wait for sysfs node to appear
    write_sysfs(PWM0_PERIOD, "1000");          // Set period (in ns)
    write_sysfs(PWM0_ENABLE, "1");             // Enable PWM output
}

// Set the duty cycle of the PWM signal
void set_pwm_duty(int percent) {
    char buf[32];
    snprintf(buf, sizeof(buf), "%d", (PERIOD_NS * percent) / 100); // Calculate duty cycle
    write_sysfs(PWM0_DUTY, buf);              // Write duty cycle to sysfs
}

// Generate and play a tone using ALSA at given frequency and duration
int play_tone(int freq, int duration_ms) {
    snd_pcm_t *pcm_handle;
    snd_pcm_hw_params_t *params;
    int rate = 44100, channels = 1;

    // Open default audio device
    snd_pcm_open(&pcm_handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
    snd_pcm_hw_params_malloc(&params);
    snd_pcm_hw_params_any(pcm_handle, params);
    snd_pcm_hw_params_set_access(pcm_handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(pcm_handle, params, SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_channels(pcm_handle, params, channels);
    snd_pcm_hw_params_set_rate_near(pcm_handle, params, &rate, 0);
    snd_pcm_hw_params(pcm_handle, params);
    snd_pcm_hw_params_free(params);
    snd_pcm_prepare(pcm_handle);

    // Calculate number of audio samples to generate
    int samples = (duration_ms * rate) / 1000;
    short *buffer = malloc(samples * sizeof(short));

    // Generate sine wave samples
    for (int i = 0; i < samples; i++) {
        buffer[i] = 32767 * sin(2 * M_PI * freq * i / rate);
    }

    // Write samples to the PCM device
    snd_pcm_writei(pcm_handle, buffer, samples);
    snd_pcm_drain(pcm_handle);               // Wait for playback to finish
    snd_pcm_close(pcm_handle);               // Close PCM device
    free(buffer);                            // Free sample buffer
    return 0;
}

// Send a single command byte to the OLED
void oled_command(unsigned char cmd) {
    unsigned char buffer[2] = {0x00, cmd};   // 0x00 indicates command
    write(i2c_fd, buffer, 2);                // Write to OLED
}

// Send a data buffer to OLED
void oled_data(unsigned char* data, size_t len) {
    unsigned char* buffer = malloc(len + 1);
    buffer[0] = 0x40;                        // 0x40 indicates data
    memcpy(buffer + 1, data, len);
    write(i2c_fd, buffer, len + 1);          // Write to OLED
    free(buffer);
}

// OLED initialization sequence (based on SSD1306 datasheet)
void oled_init() {
    oled_command(0xAE);                      // Display off
    oled_command(0xA8); oled_command(0x3F);  // Set multiplex ratio
    oled_command(0xD3); oled_command(0x00);  // Set display offset
    oled_command(0x40);                      // Set start line
    oled_command(0xA1);                      // Segment remap
    oled_command(0xC8);                      // COM output scan direction
    oled_command(0xDA); oled_command(0x12);  // COM pins config
    oled_command(0x81); oled_command(0x7F);  // Contrast
    oled_command(0xA4);                      // Resume display RAM
    oled_command(0xA6);                      // Normal display
    oled_command(0xD5); oled_command(0x80);  // Clock divide
    oled_command(0x8D); oled_command(0x14);  // Enable charge pump
    oled_command(0xAF);                      // Display on
}

// Clear the entire OLED screen
void oled_clear() {
    for (int page = 0; page < 8; page++) {
        oled_command(0xB0 + page);           // Set page address
        oled_command(0x00);                  // Set lower column
        oled_command(0x10);                  // Set higher column
        unsigned char zeros[OLED_WIDTH] = {0}; // Empty data
        oled_data(zeros, OLED_WIDTH);        // Clear page
    }
}

// Set cursor to specific page and column
void oled_set_cursor(int page, int col) {
    oled_command(0xB0 + page);               // Set page
    oled_command(0x00 + (col & 0x0F));       // Set lower nibble of column
    oled_command(0x10 + (col >> 4));         // Set upper nibble of column
}

// Draw a square wave on OLED based on duty cycle
void draw_square_wave(int duty_percent) {
    oled_clear();
    int cycle_width = 16;                    // Pixels per cycle
    int high_len = (cycle_width * duty_percent) / 100;
    int low_len = cycle_width - high_len;
    int y_top = 1, y_bottom = 4;
    int col = 0;

    while (col + cycle_width <= OLED_WIDTH) {
        // Rising edge
        oled_set_cursor(y_top, col);
        unsigned char full = 0xFF;
        oled_data(&full, 1);

        // High period (horizontal line)
        for (int i = 1; i < high_len - 1; i++) {
            oled_set_cursor(y_top, col + i);
            oled_data(&full, 1);
        }

        // Falling edge
        oled_set_cursor(y_top, col + high_len - 1);
        for (int i = y_top; i <= y_bottom; i++) {
            oled_set_cursor(i, col + high_len - 1);
            oled_data(&full, 1);
        }

        // Low period (horizontal line)
        for (int i = col + high_len; i < col + cycle_width - 1; i++) {
            oled_set_cursor(y_bottom, i);
            oled_data(&full, 1);
        }

        // End of cycle (rising edge)
        oled_set_cursor(y_bottom, col + cycle_width - 1);
        for (int i = y_top; i <= y_bottom; i++) {
            oled_set_cursor(i, col + cycle_width - 1);
            oled_data(&full, 1);
        }

        col += cycle_width;
    }
}

// Main application logic
int main() {
    // Open UART RX device
    int fd = open("/dev/uart_rx", O_RDONLY | O_NONBLOCK);
    if (fd < 0) {
        perror("open /dev/uart_rx");
        return 1;
    }

    // Open I2C bus
    if ((i2c_fd = open(I2C_DEV, O_RDWR)) < 0) {
        perror("Failed to open I2C device");
        return 1;
    }

    // Set I2C slave address to OLED
    if (ioctl(i2c_fd, I2C_SLAVE, OLED_ADDR) < 0) {
        perror("Failed to set I2C address");
        return 1;
    }

    // Initialize OLED and PWM
    oled_init();
    oled_clear();
    setup_pwm();

    // Read characters from UART device
    char str[100] = {0};
    int len = read(fd, str, 5);              // Read max 5 bytes
    if (len <= 0) {
        perror("read from /dev/uart_rx");
        close(fd);
        return 1;
    }

    str[len] = '\0';                         // Null-terminate input string
    printf("Received: %s\n", str);           // Debug print

    // Loop through each character and act
    for (int i = 0; i < len; i++) {
        if (str[i] < '0' || str[i] > '9') continue;
        int index = str[i] - '0';
        int freq = freq_map[index];
        int duty = duty_map[index];

        printf("Char: %c, Freq: %d Hz, Duty: %d%%\n", str[i], freq, duty);
        set_pwm_duty(duty);                 // Set PWM output
        play_tone(freq, 500);               // Play audio tone
        draw_square_wave(duty);             // Visualize waveform
        sleep(2);                           // Pause between signals
    }

    // Cleanup
    close(fd);                               // Close UART
    write_sysfs(PWM0_ENABLE, "0");           // Disable PWM
    write_sysfs(PWM_UNEXPORT, "0");          // Unexport PWM
    oled_clear();                            // Clear OLED
    close(i2c_fd);                           // Close I2C
    return 0;
}

