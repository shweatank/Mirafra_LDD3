#include<linux/init.h>
#include<linux/module.h>
#include<linux/fs.h>
#include<linux/uaccess.h>

static int major;

static int __init excption_init(void)
{
     printk(KERN_INFO "Init begins... \n");
     int a = 34;
     int b = 0;
     int ans = 0;

     ans = a/b;
     
     printk(KERN_INFO "Kernal crash code %d\n", ans);
     return 0;
}

static void __exit exception_exit(void)
{
    printk(KERN_INFO "Exiting...\n");
}

module_init(simple_char_init);
module_exit(simple_char_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("SunnyUsadadiya");
MODULE_DESCRIPTION("Exceptions for Device Driver");
