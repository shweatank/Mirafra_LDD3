#include<linux/init.h>
#include<linux/module.h>
#include<linux/uaccess.h>
#define DEVICE_NAME "simple_char_dev"
#define BUF_LEN 128
static int major;
static char device_buffer[BUF_LEN];
static int dev_open(struct inode *inode,struct file*file)
{
	printk(KERN_INFO "simple_char_dev: Device opened\n");
	return 0;
}
static int dev_release(struct inode *inode,struct file *file)
{
	printk(KERN_INFO "simple_char_dev: device closed\n");
		return 0;
}
static ssize_t dev_read(struct file *file ,char __user *user_buf ,size_t count,loff_t *ppos)
{
	int bytes_read=strlen(device_buffer);
	if(copy_to_user(user_buf,device_buffer,bytes_read))
	{
		return -EFAULT;
	}
	printk("KERN_INFO :simple_char_dev: read %d bytes\n",bytes_read);
	return  bytes_read;
}
static ssize_t dev_write(struct file *file,const char __user *user_buf,size_t len,loff_t *ppos)
{
	/*if(count >BUF_LEN-1)
		count=BUF_LEN-1;
	if(copy_from_user(device_buffer,user_buf,count))
	{
		return -EFAULT;
	}
	device_buffer[count]='\0';
	printk(KERN_INFO "simple_char_dev: written %zu bytes\n",count);
	return count;*/
	int i, j;
    char temp;
    

    
    if (len >= BUF_LEN )
        len = BUF_LEN - 1;

    if (copy_from_user(device_buffer, user_buf, len))
        return -EFAULT;

    device_buffer[len] = '\0'; // Null-terminate

    i = 0;
    j = len - 1;
    while (i < j) {
        temp = device_buffer[i];
       device_buffer[i] =device_buffer[j];
       device_buffer[j] = temp;
        i++;
        j--;
    }

   // printk(KERN_INFO "Reversed string from user: %s\n",device_buffer);

      /*int op1, op2, result = 0;
    char operator;

    if (len >= BUF_LEN)
        len = BUF_LEN - 1;

    if (copy_from_user(device_buffer, user_buf, len))
        return -EFAULT;

   device_buffer[len] = '\0'; 
    if (sscanf(device_buffer, "%d %d %c", &op1, &op2, &operator) != 3) {
        printk(KERN_ERR "Invalid input format\n");
        return -EINVAL;
    }

    // Perform operation
    switch (operator) {
        case '+': result = op1 + op2; break;
        case '-': result = op1 - op2; break;
        case '*': result = op1 * op2; break;
        case '/': result = (op2 != 0) ? op1 / op2 : 0; break;
        default:
            printk(KERN_ERR "Unsupported operator: %c\n", operator);
            return -EINVAL;
    }

    printk(KERN_INFO "result:%d\n ", result);*/
    return len;
}
static struct file_operations fops={
	.open=dev_open,
	.release=dev_release,
	.read=dev_read,
	.write=dev_write,
};
static int __init simple_char_init(void)
{
	major =register_chrdev(0,DEVICE_NAME,&fops);
	if(major<0)
	{
		printk(KERN_ALERT "Failed to regidter character device\n");
		return  major;
	}
		printk(KERN_INFO "simple_char_dev: registered with major number %d\n",major);
		return 0;
}
static void __exit simple_char_exit(void)
{
	unregister_chrdev(major,DEVICE_NAME);
	printk(KERN_INFO"simple_char_dev :unregistered device\n");
}
module_init(simple_char_init);
module_exit(simple_char_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Adinarayana");
MODULE_DESCRIPTION("Minimual character Device Driver");





