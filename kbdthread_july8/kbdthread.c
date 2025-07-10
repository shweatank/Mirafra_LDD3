#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/interrupt.h>
#include<linux/init.h>
#include<linux/delay.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Techdhaba");
MODULE_DESCRIPTION("Keyboard IRQ with threaded handler");
MODULE_VERSION("1.0");

#define KBD_IRQ 1
static int dev_id = 1;

// ---Top-half : quick acknowledge ---
static irqreturn_t kb_top_handler(int irq,void *dev_id)
{
	pr_info("kbd_threaded: [TOP HALF] IRQ %d received\n",irq);
	return IRQ_WAKE_THREAD; //Schedule bottom half
}

//--- Bottom-Half: threaded handler ---
static irqreturn_t kb_threaded_handler(int irq,void *dev_id)
{
	pr_info("kbd_threaded: [BOTTOM HALF] Handling key press...\n");

	msleep(500); //Simulated long processing
	pr_info("kbd_threaded: [BOTTOM HALF] Done processing\n");

	return IRQ_HANDLED;
}

static int __init kbd_irq_init(void)
{
	int ret;
	pr_info("kbd_threaded: Loading module\n");

	ret = request_threaded_irq(KBD_IRQ,
				   kb_top_handler, //Top Half
				   kb_threaded_handler, // Bottom Half
				   IRQF_SHARED,
				   "kbd_threaded_irq",
				   (void *)&dev_id); // dev_id
	if(ret)
	{
		pr_err("kbd_threaded: Failed to register IRQ %d\n",KBD_IRQ);
		return ret;
	}

	pr_info("kbd_threaded: IRQ %d registered successfully\n",KBD_IRQ);
	return 0;
}
static void __exit kbd_irq_exit(void)
{
	free_irq(KBD_IRQ,(void *)&dev_id);
	pr_info("kbd_threaded: IRQ %d freed , module unloaded\n",KBD_IRQ);
}
module_init(kbd_irq_init);
module_exit(kbd_irq_exit);

