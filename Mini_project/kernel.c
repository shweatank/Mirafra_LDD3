#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/workqueue.h>
#include <linux/time.h>
#include <linux/fs.h>
#include <linux/buffer_head.h>
#include <linux/io.h>

#define MODULE_NAME "keyboard_monitor"
#define PROC_NAME "mykbd"
#define BUFFER_SIZE 1024
#define KEYBOARD_IRQ 1

// Circular buffer structure
struct kbd_event {
    unsigned long timestamp;
    unsigned long cpu_idle;
    unsigned long io_wait;
    unsigned char key;
};

struct circular_buffer {
    struct kbd_event events[BUFFER_SIZE];
    int head;
    int tail;
    int count;
    spinlock_t lock;
};

// Global variables
static struct circular_buffer kbd_buffer;
static struct proc_dir_entry *proc_entry;
static struct task_struct *monitor_thread;
static struct workqueue_struct *kbd_workqueue;
static struct work_struct kbd_work;
static bool shift_pressed = false;

// Tasklet
static struct tasklet_struct kbd_tasklet;

// Function prototypes
static irqreturn_t keyboard_interrupt_handler(int irq, void *dev_id);
static void kbd_tasklet_handler(unsigned long data);
static void kbd_work_handler(struct work_struct *work);
static int monitor_thread_func(void *data);


static const char scancode_ascii[128] = {
    [0x02] = '1', [0x03] = '2', [0x04] = '3', [0x05] = '4',
    [0x06] = '5', [0x07] = '6', [0x08] = '7', [0x09] = '8',
    [0x0A] = '9', [0x0B] = '0', [0x0C] = '-', [0x0D] = '=',
    [0x0E] = '\b', [0x0F] = '\t',
    [0x10] = 'q', [0x11] = 'w', [0x12] = 'e', [0x13] = 'r',
    [0x14] = 't', [0x15] = 'y', [0x16] = 'u', [0x17] = 'i',
    [0x18] = 'o', [0x19] = 'p', [0x1E] = 'a', [0x1F] = 's',
    [0x20] = 'd', [0x21] = 'f', [0x22] = 'g', [0x23] = 'h',
    [0x24] = 'j', [0x25] = 'k', [0x26] = 'l', [0x2C] = 'z',
    [0x2D] = 'x', [0x2E] = 'c', [0x2F] = 'v', [0x30] = 'b',
    [0x31] = 'n', [0x32] = 'm', [0x33] = ',', [0x34] = '.',
    [0x35] = '/', [0x39] = ' ', [0x1C] = '\n'
};

static const char scancode_shift_ascii[128] = {
    [0x02] = '!', [0x03] = '@', [0x04] = '#', [0x05] = '$',
    [0x06] = '%', [0x07] = '^', [0x08] = '&', [0x09] = '*',
    [0x0A] = '(', [0x0B] = ')', [0x0C] = '_', [0x0D] = '+',
    [0x10] = 'Q', [0x11] = 'W', [0x12] = 'E', [0x13] = 'R',
    [0x14] = 'T', [0x15] = 'Y', [0x16] = 'U', [0x17] = 'I',
    [0x18] = 'O', [0x19] = 'P', [0x1E] = 'A', [0x1F] = 'S',
    [0x20] = 'D', [0x21] = 'F', [0x22] = 'G', [0x23] = 'H',
    [0x24] = 'J', [0x25] = 'K', [0x26] = 'L', [0x2C] = 'Z',
    [0x2D] = 'X', [0x2E] = 'C', [0x2F] = 'V', [0x30] = 'B',
    [0x31] = 'N', [0x32] = 'M', [0x33] = '<', [0x34] = '>',
    [0x35] = '?', [0x39] = ' ', [0x1C] = '\n'
};

// Circular buffer operations
static void buffer_add_event(struct kbd_event *event)
{
    unsigned long flags;
    
    spin_lock_irqsave(&kbd_buffer.lock, flags);
    
    kbd_buffer.events[kbd_buffer.head] = *event;
    kbd_buffer.head = (kbd_buffer.head + 1) % BUFFER_SIZE;
    
    if (kbd_buffer.count < BUFFER_SIZE) {
        kbd_buffer.count++;
    } else {
        // Buffer full, move tail
        kbd_buffer.tail = (kbd_buffer.tail + 1) % BUFFER_SIZE;
    }
    
    spin_unlock_irqrestore(&kbd_buffer.lock, flags);
}

static int buffer_get_event(struct kbd_event *event)
{
    unsigned long flags;
    int ret = 0;
    
    spin_lock_irqsave(&kbd_buffer.lock, flags);
    
    if (kbd_buffer.count > 0) {
        *event = kbd_buffer.events[kbd_buffer.tail];
        kbd_buffer.tail = (kbd_buffer.tail + 1) % BUFFER_SIZE;
        kbd_buffer.count--;
        ret = 1;
    }
    
    spin_unlock_irqrestore(&kbd_buffer.lock, flags);
    return ret;
}

// Parse /proc/stat for CPU statistics
static void parse_cpu_stats(unsigned long *idle, unsigned long *iowait)
{
    struct file *file;
    char buffer[256];
    loff_t pos = 0;
    int ret;
    
    *idle = 0;
    *iowait = 0;
    
    file = filp_open("/proc/stat", O_RDONLY, 0);
    if (IS_ERR(file)) {
        printk(KERN_ERR "Failed to open /proc/stat\n");
        return;
    }
    
    ret = kernel_read(file, buffer, sizeof(buffer) - 1, &pos);
    if (ret > 0) {
        buffer[ret] = '\0';
        // Parse first line: cpu  user nice system idle iowait irq softirq steal guest guest_nice
           if (sscanf(buffer, "cpu %*u %*u %*u %lu %lu", idle, iowait) != 2) 
	   {printk(KERN_ERR "Failed to parse /proc/stat\n");
        }
    }
    
    filp_close(file, NULL);
}

// Interrupt handler - minimal processing
static irqreturn_t keyboard_interrupt_handler(int irq, void *dev_id)
{
    // Schedule tasklet for immediate processing
    tasklet_schedule(&kbd_tasklet);
    return IRQ_HANDLED;
}

// Tasklet handler - quick processing
static void kbd_tasklet_handler(unsigned long data)
{
    // Get scancode from keyboard controller
    unsigned char scancode = inb(0x60);
    
    // Store scancode for later processing
    // In a real implementation, you'd want to be more careful about this
    static unsigned char last_scancode;
    last_scancode = scancode;
    
    // Schedule work for heavier processing
    queue_work(kbd_workqueue, &kbd_work);
}

// Work handler - heavier processing
static void kbd_work_handler(struct work_struct *work)
{
    struct kbd_event event;
    struct timespec64 ts;
    unsigned char raw = inb(0x60);

    // Track Shift key press/release
    if (raw == 0x2A || raw == 0x36) {  // Shift press
        shift_pressed = true;
        return;
    } else if (raw == 0xAA || raw == 0xB6) {  // Shift release
        shift_pressed = false;
        return;
    }

    // Ignore release keys
    if (raw & 0x80)
        return;

    // Convert using proper table
    char ascii = shift_pressed ? scancode_shift_ascii[raw] : scancode_ascii[raw];
    if (!ascii)
        return;

    ktime_get_real_ts64(&ts);
    event.timestamp = ts.tv_sec;
    parse_cpu_stats(&event.cpu_idle, &event.io_wait);
    event.key = ascii;

    buffer_add_event(&event);

    printk(KERN_INFO "Key='%c', idle=%lu, iowait=%lu, shift=%d\n",
           event.key, event.cpu_idle, event.io_wait, shift_pressed);
}
    

// Monitor thread function
static int monitor_thread_func(void *data)
{
    while (!kthread_should_stop()) {
        // This thread can be used for periodic maintenance
        // For now, it just sleeps
        msleep(1000);
    }
    return 0;
}

// Proc file operations
static int proc_show(struct seq_file *m, void *v)
{
    struct kbd_event event;
    int count = 0;

    seq_printf(m, "Keyboard Activity Log\n");
    seq_printf(m, "Timestamp\tKey\tCPU Idle\tI/O Wait\n");
    seq_printf(m, "=========\t===\t========\t========\n");

    while (buffer_get_event(&event) && count < 50) {
        seq_printf(m, "%lu\t\t%c\t\t%lu\t\t%lu\n",
                   event.timestamp, event.key,
                   event.cpu_idle, event.io_wait);
        count++;
    }

    return 0;
}


static int proc_open(struct inode *inode, struct file *file)
{
    return single_open(file, proc_show, NULL);
}

static const struct proc_ops proc_fops = {
    .proc_open = proc_open,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
};

// Module initialization
static int __init keyboard_monitor_init(void)
{
    int ret;
    
    printk(KERN_INFO "Loading keyboard monitor module\n");
    
    // Initialize circular buffer
    memset(&kbd_buffer, 0, sizeof(kbd_buffer));
    spin_lock_init(&kbd_buffer.lock);
    
    // Create proc entry
    proc_entry = proc_create(PROC_NAME, 0444, NULL, &proc_fops);
    if (!proc_entry) {
        printk(KERN_ERR "Failed to create /proc/%s\n", PROC_NAME);
        return -ENOMEM;
    }
    
    // Initialize tasklet
    tasklet_init(&kbd_tasklet, kbd_tasklet_handler, 0);
    
    // Create workqueue
    kbd_workqueue = create_singlethread_workqueue("kbd_workqueue");
    if (!kbd_workqueue) {
        printk(KERN_ERR "Failed to create workqueue\n");
        ret = -ENOMEM;
        goto cleanup_proc;
    }
    
    // Initialize work
    INIT_WORK(&kbd_work, kbd_work_handler);
    
    // Create monitor thread
    monitor_thread = kthread_create(monitor_thread_func, NULL, "kbd_monitor");
    if (IS_ERR(monitor_thread)) {
        printk(KERN_ERR "Failed to create monitor thread\n");
        ret = PTR_ERR(monitor_thread);
        goto cleanup_workqueue;
    }
    wake_up_process(monitor_thread);
    
    // Register interrupt handler
    ret = request_irq(KEYBOARD_IRQ, keyboard_interrupt_handler, 
                      IRQF_SHARED, MODULE_NAME, &kbd_buffer);
    if (ret) {
        printk(KERN_ERR "Failed to register interrupt handler\n");
        goto cleanup_thread;
    }
    
    printk(KERN_INFO "Keyboard monitor module loaded successfully\n");
    return 0;
    
cleanup_thread:
    kthread_stop(monitor_thread);
cleanup_workqueue:
    destroy_workqueue(kbd_workqueue);
cleanup_proc:
    proc_remove(proc_entry);
    return ret;
}

// Module cleanup
static void __exit keyboard_monitor_exit(void)
{
    printk(KERN_INFO "Unloading keyboard monitor module\n");
    
    // Free interrupt
    free_irq(KEYBOARD_IRQ, &kbd_buffer);
    
    // Stop and cleanup thread
    if (monitor_thread) {
        kthread_stop(monitor_thread);
    }
    
    // Cleanup tasklet
    tasklet_kill(&kbd_tasklet);
    
    // Cleanup workqueue
    if (kbd_workqueue) {
        flush_workqueue(kbd_workqueue);
        destroy_workqueue(kbd_workqueue);
    }
    
    // Remove proc entry
    proc_remove(proc_entry);
    
    printk(KERN_INFO "Keyboard monitor module unloaded\n");
}

module_init(keyboard_monitor_init);
module_exit(keyboard_monitor_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Bharath");
MODULE_DESCRIPTION("Smart Keyboard Activity Logger");
MODULE_VERSION("1.0");
