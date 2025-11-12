#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <gpiod.h>
#include "image_header/all.h"

// GPIO pins (BCM numbering)
#define DC_PIN    24
#define RESET_PIN 25

// SPI
static int spi_fd;
static struct gpiod_chip *chip;
static struct gpiod_line *dc_line, *reset_line;


// GPIO helper
void gpio_write(struct gpiod_line *line, int value) {
    gpiod_line_set_value(line, value);
}

// SPI write
void spi_write(uint8_t *data, int len, int is_data) {
    gpio_write(dc_line, is_data ? 1 : 0);
    write(spi_fd, data, len);
}

// Send command
void write_cmd(uint16_t cmd) {
    uint8_t buf[2] = { cmd >> 8, cmd & 0xFF };
    spi_write(buf, 2, 0);
}

// Send data
void write_data(uint16_t data) {
    uint8_t buf[2] = { data >> 8, data & 0xFF };
    spi_write(buf, 2, 1);
}

// Hardware reset
void reset_display() {
    gpio_write(reset_line, 1); usleep(10000);
    gpio_write(reset_line, 0); usleep(10000);
    gpio_write(reset_line, 1); usleep(10000);
}
// ILI9225 init sequence
void ili9225_init() {
    reset_display();

    // Power-on sequence
    write_cmd(0x10); write_data(0x0000);
    write_cmd(0x11); write_data(0x0000);
    write_cmd(0x12); write_data(0x0000);
    write_cmd(0x13); write_data(0x0000);
    write_cmd(0x14); write_data(0x0000);
    usleep(40000);

    write_cmd(0x11); write_data(0x0018);
    write_cmd(0x12); write_data(0x6121);
    write_cmd(0x13); write_data(0x006F);
    write_cmd(0x14); write_data(0x495F);
    write_cmd(0x10); write_data(0x0800);
    usleep(10000);

    write_cmd(0x11); write_data(0x103B);
    usleep(50000);

    // Driver output control
    write_cmd(0x01); write_data(0x011C);
    write_cmd(0x02); write_data(0x0100);
    write_cmd(0x03); write_data(0x1030);
    write_cmd(0x07); write_data(0x0000);

    // Frame rate
    write_cmd(0x08); write_data(0x0808);

    // Entry mode
    write_cmd(0x0F); write_data(0x0801);
    write_cmd(0x0B); write_data(0x1100);

    // Set GRAM area
    write_cmd(0x36); write_data(0x00AF); // X end
    write_cmd(0x37); write_data(0x0000); // X start
    write_cmd(0x38); write_data(0x00DB); // Y end
    write_cmd(0x39); write_data(0x0000); // Y start
    write_cmd(0x20); write_data(0x0000);
    write_cmd(0x21); write_data(0x0000);

    // Gamma correction
    write_cmd(0x30); write_data(0x0000);
    write_cmd(0x31); write_data(0x00DB);
    write_cmd(0x32); write_data(0x0000);
    write_cmd(0x33); write_data(0x0000);
    write_cmd(0x34); write_data(0x00DB);
    write_cmd(0x35); write_data(0x0000);
    write_cmd(0x36); write_data(0x00AF);
    write_cmd(0x37); write_data(0x0000);
    write_cmd(0x38); write_data(0x00DB);
    write_cmd(0x39); write_data(0x0000);
    // Display ON
    write_cmd(0x07); write_data(0x1017);
}

// Set active drawing window
void set_window(int x0, int y0, int x1, int y1) {
    write_cmd(0x36); write_data(x1);
    write_cmd(0x37); write_data(x0);
    write_cmd(0x38); write_data(y1);
    write_cmd(0x39); write_data(y0);
    write_cmd(0x20); write_data(x0);
    write_cmd(0x21); write_data(y0);
    write_cmd(0x22);
}

// Display bitmap from array
void display_bitmap(const uint16_t *bitmap) {
    int total = 176 * 220;

    set_window(0, 0, 175, 219);

    gpio_write(dc_line, 1); // Data mode

    for (int i = 0; i < total; i++) {
        uint8_t d[2] = { bitmap[i] >> 8, bitmap[i] & 0xFF };
        write(spi_fd, d, 2);
    }
}


int main() {
    // Init GPIO (libgpiod)
    chip = gpiod_chip_open_by_number(0); // /dev/gpiochip0
    dc_line = gpiod_chip_get_line(chip, DC_PIN);
    reset_line = gpiod_chip_get_line(chip, RESET_PIN);
    gpiod_line_request_output(dc_line, "ili9225", 0);
    gpiod_line_request_output(reset_line, "ili9225", 0);

    // Init SPI
    spi_fd = open("/dev/spidev0.0", O_WRONLY);
    if (spi_fd < 0) {
        perror("SPI open failed");
        return 1;
    }

    uint8_t mode = 0;
    uint32_t speed = 16000000;
    ioctl(spi_fd, SPI_IOC_WR_MODE, &mode);
    ioctl(spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);

    // Init LCD
    ili9225_init();
    //lcd_init();

    // Show bitmap
    char cmd[20];
    /*for(int i=1;i<99;i++)
    {

	   // snprintf(cmd,sizeof(cmd),"photo_rgb_%d",i);
        // display_bitmap((const uint16_t*)photo_rgb);
	display_bitmap((const uint16_t*)cmd);
	
    }*/
    display_bitmap((const uint16_t*)photo_rgb_1);
display_bitmap((const uint16_t*)photo_rgb_2);
display_bitmap((const uint16_t*)photo_rgb_3);
display_bitmap((const uint16_t*)photo_rgb_4);
display_bitmap((const uint16_t*)photo_rgb_5);
display_bitmap((const uint16_t*)photo_rgb_6);
display_bitmap((const uint16_t*)photo_rgb_7);
display_bitmap((const uint16_t*)photo_rgb_8);
display_bitmap((const uint16_t*)photo_rgb_9);
display_bitmap((const uint16_t*)photo_rgb_10);
display_bitmap((const uint16_t*)photo_rgb_11);
display_bitmap((const uint16_t*)photo_rgb_12);
display_bitmap((const uint16_t*)photo_rgb_13);
display_bitmap((const uint16_t*)photo_rgb_14);
display_bitmap((const uint16_t*)photo_rgb_15);
display_bitmap((const uint16_t*)photo_rgb_16);
display_bitmap((const uint16_t*)photo_rgb_17);
display_bitmap((const uint16_t*)photo_rgb_18);
display_bitmap((const uint16_t*)photo_rgb_19);
display_bitmap((const uint16_t*)photo_rgb_20);
display_bitmap((const uint16_t*)photo_rgb_21);
display_bitmap((const uint16_t*)photo_rgb_22);
display_bitmap((const uint16_t*)photo_rgb_23);
display_bitmap((const uint16_t*)photo_rgb_24);
display_bitmap((const uint16_t*)photo_rgb_25);
display_bitmap((const uint16_t*)photo_rgb_26);
display_bitmap((const uint16_t*)photo_rgb_27);
display_bitmap((const uint16_t*)photo_rgb_28);
display_bitmap((const uint16_t*)photo_rgb_29);
display_bitmap((const uint16_t*)photo_rgb_30);
display_bitmap((const uint16_t*)photo_rgb_31);
display_bitmap((const uint16_t*)photo_rgb_32);
display_bitmap((const uint16_t*)photo_rgb_33);
display_bitmap((const uint16_t*)photo_rgb_34);
display_bitmap((const uint16_t*)photo_rgb_35);
display_bitmap((const uint16_t*)photo_rgb_36);
display_bitmap((const uint16_t*)photo_rgb_37);
display_bitmap((const uint16_t*)photo_rgb_38);
display_bitmap((const uint16_t*)photo_rgb_39);
display_bitmap((const uint16_t*)photo_rgb_40);
display_bitmap((const uint16_t*)photo_rgb_41);
display_bitmap((const uint16_t*)photo_rgb_42);
display_bitmap((const uint16_t*)photo_rgb_43);
display_bitmap((const uint16_t*)photo_rgb_44);
display_bitmap((const uint16_t*)photo_rgb_45);
display_bitmap((const uint16_t*)photo_rgb_46);
display_bitmap((const uint16_t*)photo_rgb_47);
display_bitmap((const uint16_t*)photo_rgb_48);
display_bitmap((const uint16_t*)photo_rgb_49);
display_bitmap((const uint16_t*)photo_rgb_50);
display_bitmap((const uint16_t*)photo_rgb_51);
display_bitmap((const uint16_t*)photo_rgb_52);
display_bitmap((const uint16_t*)photo_rgb_53);
display_bitmap((const uint16_t*)photo_rgb_54);
display_bitmap((const uint16_t*)photo_rgb_55);
display_bitmap((const uint16_t*)photo_rgb_56);
display_bitmap((const uint16_t*)photo_rgb_57);
display_bitmap((const uint16_t*)photo_rgb_58);
display_bitmap((const uint16_t*)photo_rgb_59);
display_bitmap((const uint16_t*)photo_rgb_60);
display_bitmap((const uint16_t*)photo_rgb_61);
display_bitmap((const uint16_t*)photo_rgb_62);
display_bitmap((const uint16_t*)photo_rgb_63);
display_bitmap((const uint16_t*)photo_rgb_64);
display_bitmap((const uint16_t*)photo_rgb_65);
display_bitmap((const uint16_t*)photo_rgb_66);
display_bitmap((const uint16_t*)photo_rgb_67);
display_bitmap((const uint16_t*)photo_rgb_68);
display_bitmap((const uint16_t*)photo_rgb_69);
display_bitmap((const uint16_t*)photo_rgb_70);
display_bitmap((const uint16_t*)photo_rgb_71);
display_bitmap((const uint16_t*)photo_rgb_72);
display_bitmap((const uint16_t*)photo_rgb_73);
display_bitmap((const uint16_t*)photo_rgb_74);
display_bitmap((const uint16_t*)photo_rgb_75);
display_bitmap((const uint16_t*)photo_rgb_76);
display_bitmap((const uint16_t*)photo_rgb_77);
display_bitmap((const uint16_t*)photo_rgb_78);
display_bitmap((const uint16_t*)photo_rgb_79);
display_bitmap((const uint16_t*)photo_rgb_80);
display_bitmap((const uint16_t*)photo_rgb_81);
display_bitmap((const uint16_t*)photo_rgb_82);
display_bitmap((const uint16_t*)photo_rgb_83);
display_bitmap((const uint16_t*)photo_rgb_84);
display_bitmap((const uint16_t*)photo_rgb_85);
display_bitmap((const uint16_t*)photo_rgb_86);
display_bitmap((const uint16_t*)photo_rgb_87);
display_bitmap((const uint16_t*)photo_rgb_88);
display_bitmap((const uint16_t*)photo_rgb_89);
display_bitmap((const uint16_t*)photo_rgb_90);
display_bitmap((const uint16_t*)photo_rgb_91);
display_bitmap((const uint16_t*)photo_rgb_92);
display_bitmap((const uint16_t*)photo_rgb_93);
display_bitmap((const uint16_t*)photo_rgb_94);
display_bitmap((const uint16_t*)photo_rgb_95);
display_bitmap((const uint16_t*)photo_rgb_96);
display_bitmap((const uint16_t*)photo_rgb_97);
display_bitmap((const uint16_t*)photo_rgb_98);
display_bitmap((const uint16_t*)photo_rgb_99);
display_bitmap((const uint16_t*)photo_rgb_100);
    // Cleanup
    close(spi_fd);
    gpiod_chip_close(chip);
    return 0;
}

