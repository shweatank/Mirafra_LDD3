#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/interrupt.h>
#include<linux/init.h>
#include<linux/delay.h>
#include<asm/io.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Snigdha");
MODULE_DESCRIPTION("Keyboard IRQ with threaded handler");


#define KBD_IRQ 1
#define KBD_DEVICE_PORT 0x60
static int dev_id=1;

enum operation
{
	NONE,
	ADD,
	SUB,
	MUL,
	DIV
};

struct calc_data
{
	int a;
	int b;
	int res;
	enum operation op;
};

static struct calc_data data={
	.a=10,
	.b=5,
	.op=NONE,
};


static irqreturn_t kb_top_handler(int irq, void *dev_id)
{
	unsigned char scancode = inb(KBD_DEVICE_PORT);

	switch(scancode)
	{
		case 0X4E:
			data.op=ADD;
			break;
		case 0X4A:
			data.op=SUB;
			break;
		case 0X37:
			data.op=MUL;
			break;
		case 0X35:
			data.op=DIV;
			break;
		default:
			data.op=NONE;
			return IRQ_HANDLED;
	}
	pr_info("kbd threaded: [TOP HALF] scancode 0X%X received\n",scancode);
	return IRQ_WAKE_THREAD;
}


static irqreturn_t kb_thread_handler(int irq, void *dev_id)
{
	int res=0;

	switch(data.op)
	{
		case ADD:
			res=data.a+data.b;
			pr_info("kbd_threaded: [BOTTOM HALF] %d + %d = %d\n",data.a, data.b,res);
			break;
		case SUB:
			res=data.a-data.b;
			pr_info("kbd_threaded: [BOTTOM HALF] %d - %d = %d\n",data.a,data.b,res);
			break;
		case MUL:
			res=data.a*data.b;
			pr_info("kbd_threaded: [BOTTOM HALF] %d * %d = %d\n",data.a,data.b,res);
			break;
		case DIV:
			res=data.a/data.b;
			pr_info("kbd_threaded: [BOTTOM HALF] %d / %d = %d\n",data.a,data.b,res);
			break;
		default:
			pr_info("kbd_threaded: No valid operation\n");
			return IRQ_HANDLED;
	}
	data.res=res;
	data.op=NONE;

	return IRQ_HANDLED;
}


static int __init kbd_irq_init(void)
{
	int ret;
	pr_info("kbd threaded: Loading module\n");

	ret=request_threaded_irq(KBD_IRQ, kb_top_handler, kb_thread_handler, IRQF_SHARED, "kbd_threaded_irq", (void *)&dev_id);

	if(ret)
	{
		pr_err("kbd_threaded: Failed to register IRQ %d\n", KBD_IRQ);
		return ret;
	}

	pr_info("kbd_threaded: IRQ %d registered successfully\n",KBD_IRQ);

	return 0;
}

static void __exit kbd_irq_exit(void)
{
	free_irq(KBD_IRQ, (void *)&dev_id);
	pr_info("kbd_threaded: IRQ %d freed, module unloaded\n",KBD_IRQ);
}

module_init(kbd_irq_init);
module_exit(kbd_irq_exit);
