// Kernel headers for networking, device, GPIO, threads, file access, and memory operations
#include <linux/inet.h>              // for in_aton()
#include <linux/module.h>            // for module macros
#include <linux/init.h>              // for __init and __exit macros
#include <linux/fs.h>                // for file_operations and register_chrdev
#include <linux/uaccess.h>           // for copy_from_user()
#include <linux/slab.h>              // for kmalloc/kfree
#include <linux/mutex.h>             // for mutex_lock()
#include <linux/kthread.h>           // for kthread
#include <linux/delay.h>             // for msleep()
#include <linux/gpio.h>              // for GPIO operations (not used directly here)
#include <linux/wait.h>              // for wait queue
#include <linux/time.h>              // for timestamps (not used directly here)
#include <linux/net.h>               // for networking structs
#include <linux/in.h>                // for sockaddr_in
#include <linux/socket.h>            // for sockets
#include <linux/i2c.h>               // included for completeness (not used here)
#include <net/net_namespace.h>       // for init_net
#include <linux/string.h>            // for string operations
#include <linux/kernel.h>            // for printk
#include <linux/file.h>              // for kernel file ops
#include <linux/err.h>               // for IS_ERR
#include <linux/kmod.h>              // for call_usermodehelper()

// Device constants
#define DEVICE_NAME "rpi_secure"
#define CLASS_NAME  "rpi_secure_class"

// IOCTL commands
#define IOCTL_SEND_PASSWORD     _IOW('x', 1, char *)
#define IOCTL_VERIFY_PASSWORD   _IOW('x', 2, char *)
#define IOCTL_SEND_THRESHOLD    _IOW('x', 3, int)

// Temperature sensor path
#define TEMP_SENSOR_PATH "/sys/bus/w1/devices/28-0316721be1ff/w1_slave"
#define TEMP_CHECK_INTERVAL_MS 2000  // Interval in milliseconds

// Alerts
#define ALERT_THRESHOLD_AUDIO "alert_audio2.wav\n"
#define ALERT_PASSWORD_FAIL_AUDIO "alert_audio1.wav\n"

// Device and class registration
static int major;
static struct class*  rpi_class  = NULL;
static struct device* rpi_device = NULL;

// Password and threshold state
static char stored_password[32] = "";
static int stored_threshold = 0;
static bool password_verified = false;
static int failed_attempts = 0;

// Socket for alerting x86
static struct socket *alert_sock = NULL;

// Thread to monitor temperature
static struct task_struct *monitor_thread = NULL;

// Wait queue for deferred thread start
static DECLARE_WAIT_QUEUE_HEAD(start_monitor_wait);
static bool start_monitoring = false;

// Mutex for IOCTL access synchronization
static DEFINE_MUTEX(rpi_mutex);

// Sends alert message to x86 via TCP socket
static void send_alert(const char *msg) {
    struct sockaddr_in addr;
    struct msghdr msg_hdr = {};
    struct kvec iov;
    int ret;

    // Create socket if not already created
    if (!alert_sock) {
        ret = sock_create_kern(&init_net, AF_INET, SOCK_STREAM, IPPROTO_TCP, &alert_sock);
        if (ret < 0) return;

        addr.sin_family = AF_INET;
        addr.sin_port = htons(8080);
        addr.sin_addr.s_addr = in_aton("192.168.0.142"); // x86 IP

        ret = alert_sock->ops->connect(alert_sock, (struct sockaddr *)&addr, sizeof(addr), 0);
        if (ret < 0) {
            sock_release(alert_sock);
            alert_sock = NULL;
            return;
        }
    }

    // Send message
    iov.iov_base = (char *)msg;
    iov.iov_len = strlen(msg);
    kernel_sendmsg(alert_sock, &msg_hdr, &iov, 1, strlen(msg));
}

// Reads temperature value from sensor
static int read_temperature(void) {
    char buf[256] = {0};
    int temp = 0;
    struct file *f;
    loff_t pos = 0;

    f = filp_open(TEMP_SENSOR_PATH, O_RDONLY, 0);
    if (IS_ERR(f)) {
        pr_err("[rpi_secure] Failed to open sensor file\n");
        return -1;
    }

    kernel_read(f, buf, sizeof(buf) - 1, &pos);
    filp_close(f, NULL);

    // Extract temperature in millidegree and convert to °C
    if (strstr(buf, "t=")) {
        sscanf(strstr(buf, "t=") + 2, "%d", &temp);
        printk(KERN_INFO"temp:%d\n", (temp / 1000));
        return temp / 1000;
    }

    return -1;
}

// Kernel thread function to monitor temperature
static int monitor_fn(void *data) {
    wait_event_interruptible(start_monitor_wait, start_monitoring);

    while (!kthread_should_stop()) {
        int temp = read_temperature();
        if (temp >= stored_threshold) {
            send_alert(ALERT_THRESHOLD_AUDIO);
        }

        // Prepare OLED display arguments
        char temp_str[16], thresh_str[16];
        snprintf(temp_str, sizeof(temp_str), "%d", temp);
        snprintf(thresh_str, sizeof(thresh_str), "%d", stored_threshold);
        char *argv[] = { "/usr/bin/oled_display", temp_str, thresh_str, NULL };
        static char *envp[] = { "HOME=/", NULL };

        printk(KERN_INFO "temp:%s thr:%s\n", temp_str, thresh_str);
        int ret = call_usermodehelper(argv[0], argv, envp, UMH_NO_WAIT);
        if (ret < 0) {
            pr_err("[rpi_secure] Failed to invoke OLED display: %d\n", ret);
        }

        msleep(TEMP_CHECK_INTERVAL_MS);
    }

    return 0;
}

// IOCTL handler to receive password/threshold and verify password
static long rpi_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    char user_input[32];
    int new_threshold;

    if (mutex_lock_interruptible(&rpi_mutex))
        return -ERESTARTSYS;

    switch (cmd) {
    case IOCTL_SEND_PASSWORD:
        if (copy_from_user(user_input, (char __user *)arg, sizeof(user_input))) {
            mutex_unlock(&rpi_mutex);
            return -EFAULT;
        }
        strscpy(stored_password, user_input, sizeof(stored_password));
        stored_password[strcspn(stored_password, "\n")] = 0;
        password_verified = false;
        failed_attempts = 0;
        pr_info("[rpi_secure] Password stored: '%s'\n", stored_password);
        break;

    case IOCTL_VERIFY_PASSWORD:
        if (copy_from_user(user_input, (char __user *)arg, sizeof(user_input))) {
            mutex_unlock(&rpi_mutex);
            return -EFAULT;
        }
        user_input[strcspn(user_input, "\n")] = 0;
        pr_info("[rpi_secure] User input: '%s' | Stored: '%s'\n", user_input, stored_password);

        if (strcmp(user_input, stored_password) == 0) {
            password_verified = true;
            failed_attempts = 0;
            pr_info("[rpi_secure] Password verified.\n");
            wake_up_interruptible(&start_monitor_wait);
            start_monitoring = true;
            mutex_unlock(&rpi_mutex);
            return 0;
        } else {
            failed_attempts++;
            pr_warn("[rpi_secure] Failed password attempt %d.\n", failed_attempts);
            if (failed_attempts >= 3) {
                // Show lock status on OLED
                char *str = "STATUS=LOCKED";
                char *argv[] = { "/usr/bin/oled_display", str, NULL };
                static char *envp[] = { "HOME=/", NULL };
                int ret = call_usermodehelper(argv[0], argv, envp, UMH_NO_WAIT);

                send_alert(ALERT_PASSWORD_FAIL_AUDIO);
                failed_attempts = 0;
            }
            mutex_unlock(&rpi_mutex);
            return -EPERM;
        }

    case IOCTL_SEND_THRESHOLD:
        if (!password_verified) {
            mutex_unlock(&rpi_mutex);
            return -EPERM;
        }
        if (copy_from_user(&new_threshold, (int __user *)arg, sizeof(new_threshold))) {
            mutex_unlock(&rpi_mutex);
            return -EFAULT;
        }
        stored_threshold = new_threshold;
        pr_info("[rpi_secure] Threshold set to %d\n", stored_threshold);
        break;

    default:
        mutex_unlock(&rpi_mutex);
        return -EINVAL;
    }

    mutex_unlock(&rpi_mutex);
    return 0;
}

// File operations structure
static struct file_operations fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = rpi_ioctl,
};

// Initialization function
static int __init rpi_init(void) {
    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) return major;

    rpi_class = class_create(CLASS_NAME);
    if (IS_ERR(rpi_class)) return PTR_ERR(rpi_class);

    rpi_device = device_create(rpi_class, NULL, MKDEV(major, 0), NULL, DEVICE_NAME);
    if (IS_ERR(rpi_device)) return PTR_ERR(rpi_device);

    monitor_thread = kthread_run(monitor_fn, NULL, "temp_monitor_thread");

    pr_info("[rpi_secure] Driver loaded: /dev/%s\n", DEVICE_NAME);
    return 0;
}

// Exit function
static void __exit rpi_exit(void) {
    if (monitor_thread) kthread_stop(monitor_thread);
    if (alert_sock) sock_release(alert_sock);
    device_destroy(rpi_class, MKDEV(major, 0));
    class_destroy(rpi_class);
    unregister_chrdev(major, DEVICE_NAME);
    pr_info("[rpi_secure] Driver unloaded\n");
}

module_init(rpi_init);
module_exit(rpi_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("TEAM3");
MODULE_DESCRIPTION("Raspberry pi driver");

