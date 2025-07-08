#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/interrupt.h>
#include<asm/io.h>
#include<linux/string.h>
#include<linux/init.h>
static void my_tasklet_func(unsigned long data);
static struct tasklet_struct my_tasklet;

#define KEYBOARD_IRQ 1
#define KBD_PORT 0x60

char scancode_to_ascii[128]={
	[2]='1',[3]='2',[4]='3',[5]='4',[6]='5',
	[7]='6',[8]='7',[9]='8',[10]='9',[11]='0',
	[12]='-',[13]='=',[28]='\n',
	[69]='+',[53]='/',[55]='*'
};

static char scancode;
static char expr[100];
static int expr_index=0;

static void my_tasklet_func(unsigned long data)
{
	int num1=0,num2=0,result=0;
	char op=0;
	int i=0;
	expr[expr_index]='\0';
	printk("Calculator expression %s\n",expr);
	while(expr[i]>='0'&&expr[i]<='9')
	{
		num1=num1*10+(expr[i]-'0');
		i++;
	}
	op=expr[i++];
	while(expr[i]>='0'&&expr[i]<='9')
	{
		num2=num2*10+(expr[i]-'0');
		i++;
	}
	switch(op)
	{
		case '+': result=num1+num2;
			  break;
		case '-': result=num1-num2;
			  break;
		case '*': result=num1*num2;
			  break;
		case '/': result=num1/num2;
			  break;
		default: printk("Invalid operator\n");
			 return;
	}
	printk("Result is %d\n",result);
	expr_index=0;
	memset(expr,0,sizeof(expr));
}

irqreturn_t keyboard_irq_handler(int irq,void *devid)
{	
	scancode=inb(KBD_PORT);
	if(!(scancode & 0x80))
	{
		char c=scancode_to_ascii[scancode];
		if(c=='\n')
		{
			tasklet_schedule(&my_tasklet);
		}
		else if(c && expr_index<sizeof(expr)-1)
		{
			expr[expr_index++]=c;
		}
	}
	return IRQ_HANDLED;
}



static int __init my_tasklet_init(void)
{
	int ret=request_irq(KEYBOARD_IRQ,keyboard_irq_handler,IRQF_SHARED,"keyboard_irq_handler",(void *)keyboard_irq_handler);
	if(ret<0)
	{
		printk("Failed to request irq\n");
		return ret;
	}
	printk("IRQ registered successfully\n");
	pr_info("Tasklet module loaded\n");
	return 0;
}

static void __exit my_tasklet_exit(void)
{
	free_irq(KEYBOARD_IRQ,(void *)keyboard_irq_handler);
	printk("IRQ removed successfully\n");
	tasklet_kill(&my_tasklet);
	printk("Tasklet module unloading...\n");
}
module_init(my_tasklet_init);
module_exit(my_tasklet_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("sumanth");
MODULE_DESCRIPTION("a combination of keyboard interrupt and tasklet");
