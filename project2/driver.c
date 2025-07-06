//A Linux character device driver for keyboard-triggered string transformations using read/write and IOCTL.
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/interrupt.h>
#include <linux/spinlock.h>
#include <linux/wait.h>
#include <asm/io.h>
#include <linux/string.h>

#define DEVICE_NAME "myproject"
#define MAGIC       'a'

struct st {
    char str2[50];
    int  choice;
    int  len;
};

#define IOCTL_OP _IOWR(MAGIC, 0, struct st)

// keyboard constants
#define KEY_A          0x1E
#define KEY_B          0x30
#define KBD_IRQ        1
#define KBD_PORT_DATA  0x60

// scancode ring buffer
#define SC_MAX 64
static u8   sc_buf[SC_MAX];
static u32  sc_head, sc_tail;
static spinlock_t sc_lock;
static DECLARE_WAIT_QUEUE_HEAD(sc_wq);
static int major;

//string buffer for write()/read() path
#define BUF_LEN 50
static char   dev_buf[BUF_LEN];
static size_t dev_len;
static DEFINE_MUTEX(buf_lock);

  
static irqreturn_t kbd_irq(int irq, void *dev_id)
{
    u8 sc = inb(KBD_PORT_DATA);

    spin_lock(&sc_lock);
    if (((sc_head + 1) & (SC_MAX - 1)) != sc_tail) {
        sc_buf[sc_head] = sc;
        sc_head = (sc_head + 1) & (SC_MAX - 1);
        wake_up_interruptible(&sc_wq);
    }
    spin_unlock(&sc_lock);

    return IRQ_HANDLED;
}

static void str_reverse(char *s)
{
    int i = 0, j = strlen(s) - 1;
    while (i < j) {
        char t = s[i];
        s[i]   = s[j];
        s[j]   = t;
        i++; j--;
    }
}

static void str_toggle_case(char *s)
{
    for (; *s; ++s)
        if ((*s >= 'A' && *s <= 'Z') || (*s >= 'a' && *s <= 'z'))
            *s ^= 32;
}

                                                      
//count == 1 return a scancode (blocks until available)   
//otherwise   return data from dev_buf (string path)      
static ssize_t dev_read(struct file *filp, char __user *ubuf,
                        size_t count, loff_t *ppos)
{
  
    if (count == 1) {
        u8 sc;

        if (wait_event_interruptible(sc_wq, sc_head != sc_tail))
            return -ERESTARTSYS;

        spin_lock_irq(&sc_lock);
        sc = sc_buf[sc_tail];
        sc_tail  = (sc_tail + 1) & (SC_MAX - 1);
        spin_unlock_irq(&sc_lock);

        return copy_to_user(ubuf, &sc, 1) ? -EFAULT : 1;
    }


    if (*ppos >= dev_len)
        return 0;

    if (mutex_lock_interruptible(&buf_lock))
        return -ERESTARTSYS;

    if (count > dev_len - *ppos)
        count = dev_len - *ppos;

    if (copy_to_user(ubuf, dev_buf + *ppos, count)) {
        mutex_unlock(&buf_lock);
        return -EFAULT;
    }

    *ppos += count;
    mutex_unlock(&buf_lock);
    return count;
}


// reverse the string, store            
static ssize_t dev_write(struct file *filp, const char __user *ubuf,
                         size_t count, loff_t *ppos)
{
    if (count >= BUF_LEN)
        count = BUF_LEN - 1;

    if (mutex_lock_interruptible(&buf_lock))
        return -ERESTARTSYS;

    if (copy_from_user(dev_buf, ubuf, count)) {
        mutex_unlock(&buf_lock);
        return -EFAULT;
    }

    dev_buf[count] = '\0';
    dev_len = count;
    str_reverse(dev_buf);

    mutex_unlock(&buf_lock);
    *ppos = 0;
    return count;
}

//IOCTL – length or toggle‑case                                
static long dev_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    struct st x;

    if (cmd != IOCTL_OP)
        return -EINVAL;

    if (copy_from_user(&x, (void __user *)arg, sizeof (x)))
        return -EFAULT;

    if (x.choice == 1)
        x.len = strlen(x.str2);
    else if (x.choice == 2)
        str_toggle_case(x.str2);
    else
        return -EINVAL;

    return copy_to_user((void __user *)arg, &x, sizeof x) ? -EFAULT : 0;
}


static const struct file_operations fops = {
    .owner          = THIS_MODULE,
    .read           = dev_read,
    .write          = dev_write,
    .unlocked_ioctl = dev_ioctl,
    .llseek         = no_llseek,
    
};




static int __init my_init(void)
{
    int ret;

    spin_lock_init(&sc_lock);
    init_waitqueue_head(&sc_wq);

    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
        pr_err("%s: failed to register char device\n", DEVICE_NAME);
        return major;
    }

    ret = request_irq(KBD_IRQ, kbd_irq, IRQF_SHARED,
                      DEVICE_NAME, (void *)kbd_irq);
    if (ret) {
        unregister_chrdev(major, DEVICE_NAME);
        pr_err("%s: can't request IRQ %d (%d)\n", DEVICE_NAME, KBD_IRQ, ret);
        return ret;
    }

    pr_info("%s: registered major %d\n",
            DEVICE_NAME, major);
    return 0;
}

static void __exit my_exit(void)
{
    free_irq(KBD_IRQ, (void *)kbd_irq);
    unregister_chrdev(major, DEVICE_NAME);
    pr_info("%s: unregistered\n", DEVICE_NAME);
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Revathi");
MODULE_DESCRIPTION("Keyboard scancode + string ops char driver");

