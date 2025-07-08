#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/smp.h>
#include <linux/io.h>
MODULE_LICENSE("GPL");
MODULE_AUTHOR("swarna");
MODULE_DESCRIPTION("calculator using keyboard interrupt using tasklet");
#define KEYBOARD_IRQ 1
static void my_tasklet_func(struct tasklet_struct *tasklet);
static struct tasklet_struct my_tasklet;
static int a = 20, b = 10;
static char op;
static char scancode_to_op(unsigned char sc)
{
    switch (sc) 
    {
        case 0x1e: return 'a';
        case 0x30: return 'b';
        case 0x2e: return 'c'; 
        case 0x20: return 'd'; 
        default: return '\0';
    }
}
static void my_tasklet_func(struct tasklet_struct *tasklet)
{
    int result = 0;
    switch (op) 
    {
        case 'a':
            	result=a+b;
            	pr_info("tasklet: %d + %d = %d\n", a, b, result);
            	break;
        case 'b':
            	result=a-b;
            	pr_info("tasklet: %d - %d = %d\n", a, b, result);
           	 break;
        case 'c':
            	result = a * b;
            	pr_info("tasklet: %d * %d = %d\n", a, b, result);
            	break;
        case 'd':
                result = a / b;
                pr_info("tasklet: %d / %d = %d\n", a, b, result);
            	break;
        default:
            	pr_info("tasklet: invalid key %c\n", op);
    }
}
static irqreturn_t keyboard_irq_handler(int irq, void *dev_id)
{
    unsigned char scancode = inb(0x60); 
    char key=scancode_to_op(scancode);
    if (key) 
    {
        op=key;
        pr_info("keyboard: key '%c' pressed, scheduling tasklet\n", op);
        tasklet_schedule(&my_tasklet);
    }
    return IRQ_HANDLED;
}
static int __init tasklet_init_module(void)
{
    pr_info("tasklet calculator module loaded\n");
    tasklet_setup(&my_tasklet, my_tasklet_func);
    if (request_irq(KEYBOARD_IRQ, keyboard_irq_handler, IRQF_SHARED,
                    "keyboard_tasklet_irq", (void *)(keyboard_irq_handler))) {
        pr_err("Failed to request IRQ %d\n", KEYBOARD_IRQ);
        return -EIO;
    }
    pr_info("Keyboard IRQ handler registered\n");
    return 0;
}
static void __exit tasklet_cleanup_module(void)
{
    pr_info("tasklet module unloading\n");
    free_irq(KEYBOARD_IRQ, (void *)(keyboard_irq_handler));
    tasklet_kill(&my_tasklet);
}

module_init(tasklet_init_module);
module_exit(tasklet_cleanup_module);

