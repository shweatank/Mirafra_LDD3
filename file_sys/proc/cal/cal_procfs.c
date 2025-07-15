// SPDX-License-Identifier: GPL-2.0
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/string.h>

#define PROC_NAME "procfs_calc"
#define BUFFER_SIZE 128

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Simple /proc calculator driver");
MODULE_VERSION("1.0");

static char procfs_buffer[BUFFER_SIZE];
static int result = 0;

// === Read handler ===
static ssize_t proc_read(struct file *file, char __user *user_buf, size_t count, loff_t *pos)
{
    int len;

    if (*pos > 0)
        return 0;

    len = snprintf(procfs_buffer, BUFFER_SIZE, "%d\n", result);

    if (copy_to_user(user_buf, procfs_buffer, len))
        return -EFAULT;

    *pos = len;
    return len;
}

// === Write handler ===
static ssize_t proc_write(struct file *file, const char __user *user_buf, size_t count, loff_t *pos)
{
    int a, b;
    char op;

    if (count >= BUFFER_SIZE)
        return -EINVAL;

    memset(procfs_buffer, 0, BUFFER_SIZE);

    if (copy_from_user(procfs_buffer, user_buf, count))
        return -EFAULT;

    // Null-terminate safely
    procfs_buffer[count] = '\0';

    // Parse expression: e.g., "5 + 3"
    if (sscanf(procfs_buffer, "%d %c %d", &a, &op, &b) != 3)
        return -EINVAL;

    switch (op) {
    case '+':
        result = a + b;
        break;
    case '-':
        result = a - b;
        break;
    case '*':
        result = a * b;
        break;
    case '/':
        if (b == 0)
            return -EINVAL;
        result = a / b;
        break;
    default:
        return -EINVAL;
    }

    return count;
}

// === Proc ops ===
static const struct proc_ops proc_file_ops = {
    .proc_read  = proc_read,
    .proc_write = proc_write,
};

// === Init ===
static int __init procfs_calc_init(void)
{
    struct proc_dir_entry *entry;

    entry = proc_create(PROC_NAME, 0666, NULL, &proc_file_ops);
    if (!entry) {
        pr_err("Failed to create /proc/%s\n", PROC_NAME);
        return -ENOMEM;
    }

    pr_info("Calculator procfs created at /proc/%s\n", PROC_NAME);
    return 0;
}

// === Exit ===
static void __exit procfs_calc_exit(void)
{
    remove_proc_entry(PROC_NAME, NULL);
    pr_info("Calculator procfs removed\n");
}

module_init(procfs_calc_init);
module_exit(procfs_calc_exit);

