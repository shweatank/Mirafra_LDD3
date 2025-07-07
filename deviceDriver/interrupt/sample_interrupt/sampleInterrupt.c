#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <asm/io.h>

#define KEYBOARD_IRQ 1
#define KBD_DATA_PORT 0x60

static struct work_struct my_work;
static unsigned char last_scancode;
void process_key_action(unsigned char code)
{
    int a = 4, b = 2;

    switch (code) {
        case 0x1E: // A key
            pr_info("A pressed: %d + %d = %d\n", a, b, a + b);
            break;

        case 0x30: // B key
            pr_info("B pressed: %d * %d = %d\n", a, b, a * b);
            break;

        default:
            break;
    }
}

static void work_handler(struct work_struct *work)
{
    process_key_action(last_scancode);
}

irqreturn_t keyboard_irq_handler(int irq, void *dev_id)
{

    last_scancode = inb(KBD_DATA_PORT);
    schedule_work(&my_work);  
    return IRQ_HANDLED;
}

static int __init calc_irq_init(void)
{
    int result;
    pr_info("Loading keyboard calc IRQ module...\n");
 INIT_WORK(&my_work, work_handler); 
    result = request_irq(KEYBOARD_IRQ, keyboard_irq_handler, IRQF_SHARED,
                         "keyboard_calc_irq", (void *)(keyboard_irq_handler));

    if (result) {
        pr_err("Failed to register IRQ handler.\n");
        return result;
    }

    pr_info("IRQ Calculator handler registered.\n");
    return 0;
}

static void __exit calc_irq_exit(void)
{
    cancel_work_sync(&my_work);	
    free_irq(KEYBOARD_IRQ, (void *)(keyboard_irq_handler));
    pr_info("IRQ Calculator handler removed.\n");
}

module_init(calc_irq_init);
module_exit(calc_irq_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("sunny usadadiya");
MODULE_DESCRIPTION("Keyboard IRQ Calculator: A=Add, B=Multiply");
