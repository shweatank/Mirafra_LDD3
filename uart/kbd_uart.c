// Required kernel headers
#include <linux/module.h>       // Core module functionality
#include <linux/kernel.h>       // Kernel logging and types
#include <linux/init.h>         // __init and __exit macros
#include <linux/interrupt.h>    // IRQ handling (request_irq, free_irq, etc.)
#include <linux/fs.h>           // File operations for filp_open, kernel_write
#include <linux/slab.h>         // For memory functions like memset
#include <linux/io.h>           // For inb() to read from I/O ports
#include <linux/file.h>         // For file handling inside kernel
#include <linux/fcntl.h>        // File open flags

#define KEYBOARD_IRQ 1          // IRQ number for keyboard (legacy PS/2)
#define KBD_DATA_PORT 0x60      // I/O port to read keyboard data
#define BUF_SIZE 256            // Maximum input buffer size

static char kbuf[BUF_SIZE];     // Buffer to store keyboard input until Enter is pressed
static int buf_index = 0;       // Current index in the buffer

static struct file *uart_fp = NULL;  // File pointer to UART device
static unsigned char last_scancode = 0; // Latest scan code received from keyboard

// Partial scan code to ASCII mapping (US layout, lowercase only, no shift/caps support)
static const char scancode_to_ascii[] = {
    0, 27, '1','2','3','4','5','6','7','8','9','0','-','=','\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n', 0,
    'a','s','d','f','g','h','j','k','l',';','\'','`', 0,
    '\\','z','x','c','v','b','n','m',',','.','/', 0, '*', 0, ' '
};

// 🔹 Top-half IRQ handler: executes quickly, just saves scan code
static irqreturn_t kbd_irq_handler(int irq, void *dev_id) {
    last_scancode = inb(KBD_DATA_PORT);   // Read raw scancode from I/O port
    return IRQ_WAKE_THREAD;               // Wake bottom-half threaded handler
}

// 🔸 Threaded bottom-half handler: runs in process context
static irqreturn_t kbd_thread_fn(int irq, void *dev_id) {
    // Ignore key release (MSB = 1 means key release)
    if (last_scancode & 0x80)
        return IRQ_HANDLED;

    // Convert valid scancode to ASCII character
    if (last_scancode < sizeof(scancode_to_ascii)) {
        char ch = scancode_to_ascii[last_scancode];

        if (ch == '\n') { // Enter key pressed
            loff_t pos = 0; // File position for writing

            // If UART is open and buffer has data, write to UART
            if (uart_fp && buf_index > 0) {
		    printk("buffer : %s \n",kbuf);
                kernel_write(uart_fp, kbuf, buf_index, &pos);     // Write buffer
                //kernel_write(uart_fp, "\n", 1, &pos);              // Write newline
            }

            // Reset buffer after sending
            memset(kbuf, 0, BUF_SIZE);
            buf_index = 0;
        } else if (buf_index < BUF_SIZE - 1) {
            // Append typed character to buffer
            kbuf[buf_index++] = ch;
        }
    }

    return IRQ_HANDLED;
}

// 🔧 Module initialization
static int __init kbd_uart_init(void) {
    int ret;

    pr_info("Loading threaded IRQ keyboard-to-UART driver...\n");

    // Open UART device file for writing
    uart_fp = filp_open("/dev/ttyUSB0", O_WRONLY | O_NOCTTY, 0);
    if (IS_ERR(uart_fp)) {
        pr_err("Failed to open /dev/ttyUSB0\n");
        return PTR_ERR(uart_fp);
    }

    // Request a threaded IRQ for the keyboard
    ret = request_threaded_irq(KEYBOARD_IRQ,            // IRQ number
                               kbd_irq_handler,          // Top-half handler
                               kbd_thread_fn,            // Bottom-half handler
                               IRQF_SHARED,              // Shared IRQ flag
                               "kbd_uart_irq",           // Name shown in /proc/interrupts
                               (void *)(kbd_irq_handler) // Device ID (used during free_irq)
                              );
    if (ret) {
        pr_err("Failed to request IRQ %d\n", KEYBOARD_IRQ);
        filp_close(uart_fp, NULL);  // Clean up UART file on failure
        return ret;
    }

    pr_info("Threaded IRQ driver loaded. Type on keyboard and press Enter.\n");
    return 0;
}

// 🔻 Module cleanup
static void __exit kbd_uart_exit(void) {
    free_irq(KEYBOARD_IRQ, (void *)(kbd_irq_handler)); // Free IRQ

    // Close UART file if valid
    if (uart_fp && !IS_ERR(uart_fp))
        filp_close(uart_fp, NULL);

    pr_info("Keyboard UART driver unloaded.\n");
}

// Register init and exit functions
module_init(kbd_uart_init);
module_exit(kbd_uart_exit);

// Module metadata
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sanaboina Pavan Satya Swaroop");
MODULE_DESCRIPTION("Keyboard interrupt driver that sends typed data to UART on Enter key using threaded IRQ");

