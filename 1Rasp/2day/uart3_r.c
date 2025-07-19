#include <linux/module.h>
#include <linux/init.h>
#include <linux/tty.h>
#include <linux/serial_core.h>
#include <linux/serial_reg.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <asm/io.h>

#define UART_BASE 0x3F8 // COM1 for UART0 via PL011 on older firmware, change for Pi

static struct hrtimer timer;
static char uart_rx_buf[128];
static int rx_pos = 0;

enum hrtimer_restart timer_callback(struct hrtimer *timer) {
    int len = rx_pos;
    if (len > 0) {
        for (int i = 0; i < len; i++) {
            pr_info("RPI UART Received: %c\n", uart_rx_buf[i]);
        }

        // Send back string size
        char size_msg[10];
        snprintf(size_msg, sizeof(size_msg), "%d\n", len);
        for (int i = 0; size_msg[i]; ++i) {
            while (!(inb(UART_BASE + UART_LSR) & 0x20));
            outb(size_msg[i], UART_BASE);
        }

        rx_pos = 0;
    }
    hrtimer_forward_now(timer, ms_to_ktime(1000));
    return HRTIMER_RESTART;
}

static int __init uart_timer_init(void) {
    request_region(UART_BASE, 8, "uart_rpi");

    // UART config
    outb(0x80, UART_BASE + UART_LCR);
    outb(0x01, UART_BASE + UART_DLL);
    outb(0x00, UART_BASE + UART_DLM);
    outb(0x03, UART_BASE + UART_LCR);
    outb(0xC7, UART_BASE + UART_FCR);
    outb(0x0B, UART_BASE + UART_MCR);

    hrtimer_init(&timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
    timer.function = &timer_callback;
    hrtimer_start(&timer, ms_to_ktime(1000), HRTIMER_MODE_REL);

    pr_info("RPI UART Timer Module Loaded\n");
    return 0;
}

static void __exit uart_timer_exit(void) {
    hrtimer_cancel(&timer);
    release_region(UART_BASE, 8);
    pr_info("RPI UART Timer Module Unloaded\n");
}

module_init(uart_timer_init);
module_exit(uart_timer_exit);
MODULE_LICENSE("GPL");

