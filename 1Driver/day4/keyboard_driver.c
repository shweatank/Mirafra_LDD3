#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <asm/io.h>
#include <linux/interrupt.h>

#define KEYBOARD_IRQ 1
#define KBD_DATA_PORT 0x60

static int __init keyboard_irq_init(void);
static void __exit keyboard_irq_exit(void);

// Simplified scancode table for US QWERTY layout (only make codes)
static const char *scancode_map[128] = {
    [0x01] = "ESC",   [0x02] = "1",     [0x03] = "2",     [0x04] = "3",
    [0x05] = "4",     [0x06] = "5",     [0x07] = "6",     [0x08] = "7",
    [0x09] = "8",     [0x0A] = "9",     [0x0B] = "0",     [0x0C] = "-",
    [0x0D] = "=",     [0x0E] = "Back",  [0x0F] = "Tab",   [0x10] = "Q",
    [0x11] = "W",     [0x12] = "E",     [0x13] = "R",     [0x14] = "Ctrl",
    [0x15] = "Y",     [0x16] = "U",     [0x17] = "I",     [0x18] = "O",
    [0x19] = "P",     [0x1C] = "Enter", [0x1E] = "A",     [0x1F] = "S",
    [0x20] = "D",     [0x21] = "F",     [0x22] = "G",     [0x23] = "H",
    [0x24] = "J",     [0x25] = "K",     [0x26] = "L",     [0x2C] = "Z",
    [0x2D] = "X",     [0x2E] = "C",     [0x2F] = "V",     [0x30] = "B",
    [0x31] = "N",     [0x32] = "M",     [0x39] = "Space"
};

irqreturn_t keyboard_irq_handler(int irq, void *dev_id)
{
    unsigned char scancode = inb(KBD_DATA_PORT);

    if (scancode < 0x80) {
        // Key press (make code)
        const char *key = scancode_map[scancode];
        if (key)
            printk(KERN_INFO "keyboard IRQ: Scancode = 0x%02x, Key = %s\n", scancode, key);
        else
            printk(KERN_INFO "keyboard IRQ: Scancode = 0x%02x, Key = Unknown\n", scancode);
    } else {
        // Key release (break code)
        printk(KERN_INFO "keyboard IRQ: Scancode = 0x%02x (key release)\n", scancode);
    }

    return IRQ_HANDLED;
}

static int __init keyboard_irq_init(void)
{
	int result;
	printk(KERN_INFO "LOading custom Keyboard IRQ handler...\n");
	result = request_irq(KEYBOARD_IRQ,keyboard_irq_handler,IRQF_SHARED,"keyboard_irq_handler",(void *)(keyboard_irq_handler));
	if(result)
	{
		printk(KERN_ERR "keybord_irq: cannot register IRQ %d\n",KEYBOARD_IRQ);
		return result;
	}
	printk(KERN_INFO "Keyboard_irq: IRQ Handler ADDED!");
	return 0;
}

static void __exit keyboard_irq_exit(void)
{
	free_irq(KEYBOARD_IRQ, (void*) (keyboard_irq_handler));
	printk(KERN_INFO "Keyboard_irq: IRQ Handler removed");
}

module_init(keyboard_irq_init);
module_exit(keyboard_irq_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Hemanth");
MODULE_DESCRIPTION("Keyboard IRQ Handler for X86 LINUX");
