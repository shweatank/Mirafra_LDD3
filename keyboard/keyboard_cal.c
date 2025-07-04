#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/interrupt.h>
#include<asm/io.h>

#define KEYBOARD_IRQ 1 //irq no for keyboard
#define KBD_DATA_PORT 0x60 //port to read keyboard scancode
static int operand1 = 0;
static int operand2 = 0;
static char op = 0;
static int result = 0;
static int input_state = 0; // 0: operand1, 1: operator, 2: operand2

char scancode_to_ascii[128] = {
    [0x1E] = 'a', [0x1F] = 's', [0x20] = 'd', [0x2E] = 'c', [0x2D] = 'x',
    [0x02] = '1', [0x03] = '2', [0x04] = '3', [0x05] = '4',
    [0x06] = '5', [0x07] = '6', [0x08] = '7', [0x09] = '8',
    [0x0A] = '9', [0x0B] = '0', [0x1C] = '\n' // Enter key
};

//interrupt handler

irqreturn_t keyboard_irq_handler(int irq,void *dev_id)
{
	unsigned char sc = inb(0x60);
	if (!(sc & 0x80)) { // key press only
     char key = scancode_to_ascii[sc];
    if (key >= '0' && key <= '9') {
        if (input_state == 0)
            operand1 = operand1 * 10 + (key - '0');
        else if (input_state == 2)
            operand2 = operand2 * 10 + (key - '0');
    } else if (key == 'a' || key == 's' || key == 'd' || key == 'm') {
        op = key;
        input_state = 2;
    } else if (key == '\n') {
        switch (op) {
            case 'a': result = operand1 + operand2; break;
            case 's': result = operand1 - operand2; break;
            case 'd': result = operand2 ? operand1 / operand2 : 0; break;
            case 'm': result = operand1 * operand2; break;
        }
	printk(KERN_INFO "operand1 = %d, operand2 = %d, op = %c\n", operand1, operand2, op);
        printk(KERN_INFO "Result: %d\n", result);
        operand1 = operand2 = result = 0;
        input_state = 0;
    }
}

	return IRQ_HANDLED;
}

static int __init keyboard_irq_init(void)
{
	int result;
	 printk(KERN_INFO "loading custom keyboard IRQ handler...\n");

	 //requesting iRQ 1
	 
	 result=request_irq(KEYBOARD_IRQ,keyboard_irq_handler,IRQF_SHARED,"keyboard_irq_handler",(void *)(keyboard_irq_handler));

	 if(result)
	 {
		 printk(KERN_ERR "keyboard_irq: cannot register IRQ %d\n",KEYBOARD_IRQ);
		 return result;
	 }

	 printk(KERN_INFO "keyboard_irq: IRQ handler registered sucessfully.\n");
	 return 0;
}

static void __exit keyboard_irq_exit(void)
{
	free_irq(KEYBOARD_IRQ,(void *)(keyboard_irq_handler));
	printk(KERN_INFO "keyboard_irq: IRQ handler removed. \n");
}

module_init(keyboard_irq_init);
module_exit(keyboard_irq_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("pavan");
MODULE_DESCRIPTION("keyboard custom device driver");
