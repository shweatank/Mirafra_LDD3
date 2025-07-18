
#include<linux/module.h>
#include<linux/fs.h>
#include<linux/uaccess.h>
#include<linux/ioctl.h>
#include<linux/gpio.h>
#include<linux/device.h>
#include<linux/kernel.h>

#define DEVICE_NAME "ioctl_demo"
#define MAJOR_NUM 100
#define DRIVER_NAME "led_gpio_driver"
#define CLASS_NAME "led"
#define GPIO_LED (17+512)

#define IOCTL_SET_NUM _IOW(MAJOR_NUM,0,int)
#define IOCTL_GET_NUM _IOR(MAJOR_NUM,1,int)

static int device_number=0;

static int my_open(struct inode *inode,struct file *file){
        pr_info("ioctl_demo: Device opened\n");
        return 0;
}

static int my_release(struct inode *inode,struct file *file)
{
        pr_info("ioctl_demo:device closed\n");
        return 0;
}

static long my_ioctl(struct file *file,unsigned int cmd,unsigned long arg){
        int user_val;
        switch(cmd){
                case IOCTL_SET_NUM:
                        if(copy_from_user(&user_val,(int __user *)arg,sizeof(user_val)))
                                return -EFAULT;
                        device_number=user_val;
                       // pr_info("ioctl_demo: Received number=%d\n",device_number);
		       if(device_number){
		       gpio_set_value(GPIO_LED,1);
		       printk(KERN_INFO "LED: ON\n");
		       }
		       else if(device_number==0){
			       gpio_set_value(GPIO_LED,0);
			       printk(KERN_INFO "LED : OFF\n");
		       }
                        break;
              //  case IOCTL_GET_NUM:
                //        if(copy_to_user((int __user *)arg,&device_number,sizeof(device_number)))
                  //              return -EFAULT;
                    //    pr_info("ioctl_demo: Sent number=%d\n",device_number);
                      //  break;
                default:
                        return -EFAULT;
        }
return 0;
}

static struct file_operations fops = {
        .owner = THIS_MODULE,
        .open = my_open,
        .release = my_release,
        .unlocked_ioctl = my_ioctl,
};

static int __init ioctl_init(void){
        int ret = register_chrdev(MAJOR_NUM,DEVICE_NAME, &fops);
        if(ret<0){
                pr_err("Failed to register char device\n");
                return 0;
        }
        pr_info("ioctl_demo: module loaded,major %d\n",MAJOR_NUM);
        //Request the GPIO
        if(!gpio_is_valid(GPIO_LED)){
		printk(KERN_ALERT "LED:Invalid GPIO %d\n",GPIO_LED);
          return -ENODEV;
	}
	gpio_request(GPIO_LED,"sysfs");
	gpio_direction_output(GPIO_LED,0);

        return 0;
}
static void __exit ioctl_exit(void){
	gpio_set_value(GPIO_LED,0);
	gpio_free(GPIO_LED);
        unregister_chrdev(MAJOR_NUM,DEVICE_NAME);
        pr_info("ioctl_demo: module unloaded\n");
}

module_init(ioctl_init);
module_exit(ioctl_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("BHARATH");
MODULE_DESCRIPTION("Simple IOCTL Driver for LED blinking");
                                                                               
                                                                         


