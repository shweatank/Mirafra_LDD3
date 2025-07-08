#include <linux/module.h>   
#include <linux/init.h>     
#include <linux/kernel.h>   


static int __init hello_init(void) 
{
    printk(KERN_INFO "Hello, Kernel! Module loaded.\n");

    int a=5,b=0;
    int res=a/b;
    printk(KERN_INFO "Result:%d\n",res);
    return 0;  
}


static void __exit hello_exit(void) 
{
    printk(KERN_INFO "Goodbye, Kernel! Module unloaded.\n");
}


module_init(hello_init);
module_exit(hello_exit);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Snigdha");
MODULE_DESCRIPTION("Exception Program ");
MODULE_VERSION("1.0");

