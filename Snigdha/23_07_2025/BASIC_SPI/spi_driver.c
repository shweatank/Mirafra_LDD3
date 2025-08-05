#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/spi/spi.h>
#include <linux/platform_device.h>
#include <linux/gpio.h>
#include <linux/delay.h>

#define SPI0_BASE_PHYS 0xFE204000  // BCM2711 SPI0 base address
#define SPI0_REG_SIZE  0x1000

#define SPI_CS      0x00
#define SPI_FIFO    0x04
#define SPI_CLK     0x08
#define SPI_DLEN    0x0C
#define SPI_LTOH    0x10
#define SPI_DC      0x14

#define SPI_CS_LEN  (1 << 25)
#define SPI_CS_TA   (1 << 7)
#define SPI_CS_DONE (1 << 16)
#define SPI_CS_TXD  (1 << 18)
#define SPI_CS_RXD  (1 << 17)
#define SPI_CS_CLEAR_TX  (1 << 4)
#define SPI_CS_CLEAR_RX  (1 << 5)

#define SPI_CS_MODE_MASK 0x33
#define SPI_CS_CPOL (1 << 3)
#define SPI_CS_CPHA (1 << 2)

// GPIO for manual CS control (if hardware CS not used)
#define SPI_CS_GPIO 8

struct spi_sd {
    void __iomem *base;
    struct spi_master *master;
    int cs_gpio;
};

static void spi_sd_set_cs(struct spi_sd *sd, bool active)
{
    gpio_set_value(sd->cs_gpio, active ? 0 : 1); // CS active low
    udelay(1);  // small delay after CS change
}

static void spi_sd_start_transfer(struct spi_sd *sd)
{
    u32 cs = readl(sd->base + SPI_CS);
    cs |= SPI_CS_TA | SPI_CS_CLEAR_TX | SPI_CS_CLEAR_RX;
    writel(cs, sd->base + SPI_CS);
}

static void spi_sd_end_transfer(struct spi_sd *sd)
{
    u32 cs = readl(sd->base + SPI_CS);
    cs &= ~SPI_CS_TA;
    writel(cs, sd->base + SPI_CS);
}

static int spi_sd_transfer_bytes(struct spi_sd *sd, const u8 *txbuf, u8 *rxbuf, size_t len)
{
    size_t i;
    u32 cs;

    for (i = 0; i < len; i++) {
        // Wait for TX ready
        do {
            cs = readl(sd->base + SPI_CS);
        } while (!(cs & SPI_CS_TXD));

        writel(txbuf ? txbuf[i] : 0xFF, sd->base + SPI_FIFO);

        // Wait for RX ready
        do {
            cs = readl(sd->base + SPI_CS);
        } while (!(cs & SPI_CS_RXD));

        u8 val = readl(sd->base + SPI_FIFO) & 0xFF;

        if (rxbuf)
            rxbuf[i] = val;
    }

    // Wait for done
    do {
        cs = readl(sd->base + SPI_CS);
    } while (!(cs & SPI_CS_DONE));

    return 0;
}

static int spi_sd_send_command(struct spi_sd *sd, const u8 *cmd, u8 *response)
{
    int i;
    u8 resp = 0xFF;

    spi_sd_set_cs(sd, true);   // CS low - start

    spi_sd_start_transfer(sd);

    // Send command (6 bytes)
    spi_sd_transfer_bytes(sd, cmd, NULL, 6);

    // Wait for response (response is 1-8 bytes, first with MSB=0)
    for (i = 0; i < 8; i++) {
        spi_sd_transfer_bytes(sd, NULL, &resp, 1);
        if ((resp & 0x80) == 0)
            break;
    }

    spi_sd_end_transfer(sd);

    spi_sd_set_cs(sd, false);  // CS high - end

    if (response)
        *response = resp;

    return 0;
}

static int spi_sd_setup(struct spi_device *spi)
{
    struct spi_sd *sd = spi_master_get_devdata(spi->master);

    sd->cs_gpio = SPI_CS_GPIO;
    if (gpio_request(sd->cs_gpio, "spi_sd_cs")) {
        pr_err("Failed to request GPIO for CS\n");
        return -EBUSY;
    }
    gpio_direction_output(sd->cs_gpio, 1); // CS inactive high

    // Set SPI clock divider for ~400kHz: 250MHz/400kHz = 625
    writel(625, sd->base + SPI_CLK);

    // SPI mode 0 (CPOL=0, CPHA=0)
    u32 cs = readl(sd->base + SPI_CS);
    cs &= ~SPI_CS_MODE_MASK;
    writel(cs, sd->base + SPI_CS);

    pr_info("SPI SD card driver setup complete\n");

    return 0;
}

static int spi_sd_transfer_one(struct spi_master *master, struct spi_device *spi,
                               struct spi_transfer *transfer)
{
    struct spi_sd *sd = spi_master_get_devdata(master);

    if (!transfer->len)
        return 0;

    // Start SPI transfer
    spi_sd_start_transfer(sd);

    // Transfer bytes
    spi_sd_transfer_bytes(sd, transfer->tx_buf, transfer->rx_buf, transfer->len);

    // End SPI transfer
    spi_sd_end_transfer(sd);

    spi_finalize_current_transfer(master);

    return 0;
}

static int spi_sd_probe(struct platform_device *pdev)
{
    struct spi_master *master;
    struct spi_sd *sd;

    master = spi_alloc_master(&pdev->dev, sizeof(*sd));
    if (!master)
        return -ENOMEM;

    sd = spi_master_get_devdata(master);

    sd->base = ioremap(SPI0_BASE_PHYS, SPI0_REG_SIZE);
    if (!sd->base) {
        dev_err(&pdev->dev, "Failed to ioremap SPI registers\n");
        spi_master_put(master);
        return -ENOMEM;
    }

    sd->master = master;

    master->bus_num = 0;
    master->num_chipselect = 1;
    master->mode_bits = SPI_CPOL | SPI_CPHA;
    master->bits_per_word_mask = SPI_BPW_RANGE_MASK(8, 8);
    master->setup = spi_sd_setup;
    master->transfer_one = spi_sd_transfer_one;
    master->dev.of_node = pdev->dev.of_node;

    platform_set_drvdata(pdev, sd);

    return spi_register_master(master);
}

static int spi_sd_remove(struct platform_device *pdev)
{
    struct spi_sd *sd = platform_get_drvdata(pdev);

    gpio_free(sd->cs_gpio);
    iounmap(sd->base);
    spi_unregister_master(sd->master);

    return 0;
}

static struct platform_driver spi_sd_driver = {
    .driver = {
        .name = "spi_sd",
        .owner = THIS_MODULE,
    },
    .probe = spi_sd_probe,
    .remove = spi_sd_remove,
};

module_platform_driver(spi_sd_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ChatGPT");
MODULE_DESCRIPTION("Minimal SPI Master Driver for Micro SD Card Adapter");
