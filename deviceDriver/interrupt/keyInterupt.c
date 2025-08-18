#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <asm/io.h>
#include <linux/delay.h>

#define KEYBOARD_IRQ 1
#define KBD_DATA_PORT 0x60

irqreturn_t keyboard_irq_handler(int irq,void *dev_id)
{
   unsigned char scancode;

   scancode = inb(KBD_DATA_PORT);
   int a = 4, b = 2;

    switch (scancode)
    {
        case 0x1E:
            pr_info("Pressed A: %d + %d = %d\n", a, b, a + b);
            break;

        case 0x30:
            pr_info("Pressed B: %d * %d = %d\n", a, b, a * b);
            break;

        default:
            break;
    }
   pr_info("Key board irq: scancode = 0x%02x\n",scancode);
  // udelay(1000);
   return IRQ_HANDLED;
}

static int __init keyboard_irq_init(void)
{
    int result;
    pr_info("Loading custom keyboard IRQ handler...\n");
    result = request_irq(KEYBOARD_IRQ,keyboard_irq_handler,IRQF_SHARED,
		    "keyboard_irq_handler",(void *)(keyboard_irq_handler));
    if(result){
         pr_err("Keyboard_irq: cannot register irq  %d\n",KEYBOARD_IRQ);
	 return result;
    }
    pr_info("Keyboard_irq: IRQ handler registered successfully.\n");
    return 0;
}

static void __exit keyboard_irq_exit(void)
{
    free_irq(KEYBOARD_IRQ,(void *)(keyboard_irq_handler));
    pr_info("Keyboard_irq:IRQ handler removed.\n");
}

module_init(keyboard_irq_init);
module_exit(keyboard_irq_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sunny Usadadiya");
MODULE_DESCRIPTION("Simple keyboard irq");
