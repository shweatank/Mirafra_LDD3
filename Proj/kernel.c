 #include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/mutex.h>
#include <linux/keyboard.h>
#include <linux/kthread.h>
#include <linux/wait.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/input-event-codes.h>  // For KEY_* constants

#define DEVICE_NAME "bit_a"
#define MAX_PASS_LEN 31
#define BITWISE_IOC_MAGIC 'b'
#define BITWISE_SET_PASSWORD _IOW(BITWISE_IOC_MAGIC, 1, char[MAX_PASS_LEN + 1])
#define BITWISE_SET_DATA     _IOW(BITWISE_IOC_MAGIC, 2, uint8_t)

static char password[MAX_PASS_LEN + 1];
static int password_len = 0;
static int password_index = 0;
static bool password_verified = false;
static bool waiting_for_password = false;

static uint8_t bit_data = 0;
static DEFINE_MUTEX(lock);

static dev_t dev_number = MKDEV(102, 0);  // Static major
static struct cdev bitwise_cdev;
static DECLARE_WAIT_QUEUE_HEAD(wait_queue);
static int data_ready = 0;

// Tasklet
static void bitwise_tasklet_func(struct tasklet_struct *unused);
DECLARE_TASKLET(bitwise_tasklet, bitwise_tasklet_func);

static void bitwise_tasklet_func(struct tasklet_struct *unused) {
    printk(KERN_INFO "bitwise_logger: tasklet running\n");
}

static int bitwise_process_key(char key) {
    mutex_lock(&lock);

    if (waiting_for_password) {
        if (key == password[password_index]) {
            password_index++;
            printk(KERN_INFO "bitwise_logger: Password progress: %.*s\n", password_index, password);
            if (password_index == password_len) {
                password_verified = true;
                waiting_for_password = false;
                printk(KERN_INFO "bitwise_logger: Password verified!\n");
                data_ready = 1;
                wake_up_interruptible(&wait_queue);
            }
        } else {
            printk(KERN_INFO "bitwise_logger: Password mismatch, resetting\n");
            password_index = 0;
        }
        mutex_unlock(&lock);
        return 0;
    }

    if (!password_verified) {
        printk(KERN_INFO "bitwise_logger: Key '%c' ignored - password not verified\n", key);
        mutex_unlock(&lock);
        return 0;
    }

    // Password verified, handle bit toggling
    // Only toggle bits for keys 'a' to 'h' lowercase (as per original logic)
    if (key >= 'a' && key <= 'h') {
        int bit_pos = key - 'a';
        bit_data ^= (1 << bit_pos);
        printk(KERN_INFO "bitwise_logger: Toggled bit %d by key '%c', new bit_data = 0x%02x\n", bit_pos, key, bit_data);
        tasklet_schedule(&bitwise_tasklet);
        data_ready = 1;
        wake_up_interruptible(&wait_queue);
    } else {
        printk(KERN_INFO "bitwise_logger: Key '%c' does not toggle bits\n", key);
    }

    mutex_unlock(&lock);
    return 0;
}

static bool shift_pressed = false;

static int bitwise_notify(struct notifier_block *nblock, unsigned long code, void *_param) {
    struct keyboard_notifier_param *param = _param;
    unsigned int keycode;
    char key = 0;

    if (code != KBD_KEYSYM)
        return NOTIFY_OK;

    keycode = param->value;

    if (param->down) {
        // Track shift keys
        if (keycode == KEY_LEFTSHIFT || keycode == KEY_RIGHTSHIFT) {
            shift_pressed = true;
            return NOTIFY_OK;
        }

        // Letters a-z
        if (keycode >= KEY_A && keycode <= KEY_Z) {
            key = shift_pressed ? ('A' + (keycode - KEY_A)) : ('a' + (keycode - KEY_A));
        }
        // Digits 1-9 and shifted symbols
        else if (keycode >= KEY_1 && keycode <= KEY_9) {
            if (shift_pressed) {
                // Shifted digit symbols: ! @ # $ % ^ & * (
                static const char shifted_symbols[] = {'!', '@', '#', '$', '%', '^', '&', '*', '('};
                key = shifted_symbols[keycode - KEY_1];
            } else {
                key = '1' + (keycode - KEY_1);
            }
        }
        else if (keycode == KEY_0) {
            key = shift_pressed ? ')' : '0';
        }
        else if (keycode == KEY_SPACE) {
            key = ' ';
        }
        // You can add more keys here if you want

        if (key) {
            bitwise_process_key(key);
        }
    } else {
        // Key released
        if (keycode == KEY_LEFTSHIFT || keycode == KEY_RIGHTSHIFT) {
            shift_pressed = false;
        }
    }

    return NOTIFY_OK;
}

static struct notifier_block bitwise_nb = {
    .notifier_call = bitwise_notify,
};

static long bitwise_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    int ret = 0;

    if (_IOC_TYPE(cmd) != BITWISE_IOC_MAGIC)
        return -ENOTTY;

    mutex_lock(&lock);
    switch (cmd) {
        case BITWISE_SET_PASSWORD:
            if (copy_from_user(password, (char __user *)arg, MAX_PASS_LEN)) {
                ret = -EFAULT;
                break;
            }
            password[MAX_PASS_LEN] = '\0';
            password_len = strnlen(password, MAX_PASS_LEN);
            password_index = 0;
            password_verified = false;
            waiting_for_password = true;
            printk(KERN_INFO "bitwise_logger: Password set. Please type it using keyboard.\n");
            break;

        case BITWISE_SET_DATA:
            if (copy_from_user(&bit_data, (uint8_t __user *)arg, sizeof(bit_data))) {
                ret = -EFAULT;
                break;
            }
            printk(KERN_INFO "bitwise_logger: Bit data set to 0x%02x via ioctl\n", bit_data);
            break;

        default:
            ret = -ENOTTY;
    }
    mutex_unlock(&lock);
    return ret;
}

static int bitwise_open(struct inode *inode, struct file *file) {
    printk(KERN_INFO "bitwise_logger: Device opened\n");
    return 0;
}

static int bitwise_release(struct inode *inode, struct file *file) {
    printk(KERN_INFO "bitwise_logger: Device closed\n");
    return 0;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = bitwise_ioctl,
    .open = bitwise_open,
    .release = bitwise_release,
};

static int kthread_fn(void *data) {
    while (!kthread_should_stop()) {
        wait_event_interruptible(wait_queue, data_ready != 0 || kthread_should_stop());
        if (kthread_should_stop())
            break;

        mutex_lock(&lock);
        printk(KERN_INFO "bitwise_logger: kthread running - bit_data = 0x%02x\n", bit_data);
        data_ready = 0;
        mutex_unlock(&lock);
        msleep(1000);
    }
    return 0;
}

static struct task_struct *bitwise_thread;

static int __init bitwise_init(void) {
    int ret;

    ret = register_chrdev_region(dev_number, 1, DEVICE_NAME);
    if (ret)
        return ret;

    cdev_init(&bitwise_cdev, &fops);
    bitwise_cdev.owner = THIS_MODULE;

    ret = cdev_add(&bitwise_cdev, dev_number, 1);
    if (ret) {
        unregister_chrdev_region(dev_number, 1);
        return ret;
    }

    register_keyboard_notifier(&bitwise_nb);
    init_waitqueue_head(&wait_queue);
    mutex_init(&lock);

    bitwise_thread = kthread_run(kthread_fn, NULL, "bitwise_kthread");
    if (IS_ERR(bitwise_thread)) {
        unregister_keyboard_notifier(&bitwise_nb);
        cdev_del(&bitwise_cdev);
        unregister_chrdev_region(dev_number, 1);
        return PTR_ERR(bitwise_thread);
    }

    printk(KERN_INFO "bitwise_logger: Module loaded with major %d\n", MAJOR(dev_number));
    return 0;
}

static void __exit bitwise_exit(void) {
    kthread_stop(bitwise_thread);
    tasklet_kill(&bitwise_tasklet);
    unregister_keyboard_notifier(&bitwise_nb);
    cdev_del(&bitwise_cdev);
    unregister_chrdev_region(dev_number, 1);
    printk(KERN_INFO "bitwise_logger: Module unloaded\n");
}

module_init(bitwise_init);
module_exit(bitwise_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("OpenAI");
MODULE_DESCRIPTION("Bit toggling driver with kernel-side password verification");
