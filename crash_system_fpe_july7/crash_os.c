#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/init.h>
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("A Simple Hello World Linux Kerel Module for soft crash");

static int __init hello_init(void){
pr_info("Hello, Kernel!\n");
int a=10,b=0;
printk(KERN_INFO "a and b initialized\n");
int c=a/b;
printk(KERN_INFO "c = %d\n",c);
return 0;
}

static void __exit hello_exit(void)
{
printk(KERN_INFO "Goodbye, /kernel!\n");
}
module_init(hello_init);
module_exit(hello_exit);

