// SPDX-License-Identifier: GPL-2.0
#include <linux/module.h>       // Needed for all modules
#include <linux/kernel.h>       // Needed for KERN_INFO
#include <linux/proc_fs.h>      // For procfs APIs
#include <linux/uaccess.h>      // For copy_from_user and copy_to_user

#define PROC_NAME "procfs_example"
#define BUFFER_SIZE 128

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Simple /proc driver with read and write support");
MODULE_VERSION("1.0");


// === Write callback ===
// This function is called when someone does: echo 42 > /proc/procfs_example
ssize_t proc_write(struct file *file, const char __user *user_buf, size_t count, loff_t *pos)
{
	char ch;
    // Clear buffer
    //memset(procfs_buffer, 0, BUFFER_SIZE);

    // Copy user data into kernel space
    if (copy_from_user(ch, user_buf, sizeof(char)))
        return -EFAULT;
    if(ch=='1')
	    gpio_set_value(529,1);
    else
	    gpio_set_value(529,0);

    return sizeof(char);
}

// === File operations structure for procfs ===
static const struct proc_ops proc_file_ops = {
    .proc_read  = proc_read,   // Assign read handler
    .proc_write = proc_write,  // Assign write handler
};

// === Module initialization ===
static int __init procfs_driver_init(void)
{
    // Create proc file with read+write permissions
    struct proc_dir_entry *entry = proc_create(PROC_NAME, 0666, NULL, &proc_file_ops);

    if (!entry) {
        pr_err("procfs_driver: Failed to create /proc/%s\n", PROC_NAME);
        return -ENOMEM;
    }

    pr_info("procfs_driver: Loaded. /proc/%s created.\n", PROC_NAME);
    return 0;
}

// === Module exit ===
static void __exit procfs_driver_exit(void)
{
    // Remove proc entry
    remove_proc_entry(PROC_NAME, NULL);
    pr_info("procfs_driver: Unloaded. /proc/%s removed.\n", PROC_NAME);
}

module_init(procfs_driver_init);
module_exit(procfs_driver_exit);
