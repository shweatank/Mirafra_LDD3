#include <linux/init.h>                // For module init and exit macros
#include <linux/usb.h>                 // For USB device structures and functions
#include <linux/notifier.h>           // For USB event notification mechanism
#include <linux/proc_fs.h>            // For creating entries in /proc filesystem
#include <linux/uaccess.h>            // For copy_to_user, copy_from_user, etc. (not used here, but often included for procfs)
#include <linux/seq_file.h>           // For sequential file interface used in /proc file handling

#define PROC_NAME "usb_detect"        // Name of the proc entry file
#define BUFFER_SIZE 1024              // Maximum size of the internal log buffer

MODULE_LICENSE("GPL");               // Specifies the license of the module
MODULE_AUTHOR("Sanaboina Pavan");    // Module author name
MODULE_DESCRIPTION("USB Device Detector with Procfs Interface"); // Description of the module

static char usb_log[BUFFER_SIZE];     // Buffer to store log messages of USB events
static int log_len = 0;               // Keeps track of the length of content in the buffer
static struct proc_dir_entry *proc_entry; // Pointer to proc entry created for logging USB events

// This function is called when the /proc file is read
static int proc_show(struct seq_file *m, void *v)
{
    seq_printf(m, "%s", usb_log);    // Writes the content of usb_log buffer to the proc file
    return 0;                         // Return success
}

// Called when the proc file is opened
static int proc_open(struct inode *inode, struct file *file)
{
    return single_open(file, proc_show, NULL); // Opens the proc file and links it to proc_show
}

// File operations structure for the /proc file
static const struct proc_ops proc_fops = {
    .proc_open    = proc_open,        // Function to be called when the file is opened
    .proc_read    = seq_show,    // Standard sequential file read handler
    .proc_seek	  = seq_lseek,   // For reposition of file offset
    .proc_release = single_release, // Called when file is closed
};

// Callback function that gets invoked when a USB event (add/remove) occurs
static int usb_notifier_callback(struct notifier_block *self,
                                 unsigned long action, void *dev)
{
    struct usb_device *usb_dev = dev; // Cast void pointer to usb_device structure
    char temp[128];                   // Temporary buffer to hold formatted event message

    if (!usb_dev)                     // Safety check for NULL device pointer
        return NOTIFY_OK;

    switch (action) {
    case USB_DEVICE_ADD:
        snprintf(temp, sizeof(temp),
                 "Connected: VID=0x%04x, PID=0x%04x\n",
                 usb_dev->descriptor.idVendor,
                 usb_dev->descriptor.idProduct); // Format device added message with Vendor ID and Product ID
        break;
    case USB_DEVICE_REMOVE:
        snprintf(temp, sizeof(temp),
                 "Disconnected: VID=0x%04x, PID=0x%04x\n",
                 usb_dev->descriptor.idVendor,
                 usb_dev->descriptor.idProduct); // Format device removed message
        break;
    default:
        return NOTIFY_OK;            // For events we don't care about, return OK
    }

    pr_info("[USBDetector] %s", temp); // Print the message to kernel log (dmesg)

    // Check if buffer has space, then append, else overwrite with the latest message
    if (log_len + strlen(temp) < BUFFER_SIZE) {
        strcat(usb_log, temp);        // Append message to log buffer
        log_len += strlen(temp);      // Update log length
    } else {
        snprintf(usb_log, BUFFER_SIZE, "%s", temp); // Overwrite log if it overflows
        log_len = strlen(temp);
    }

    return NOTIFY_OK;                // Indicate successful handling
}

// Notifier block that binds our callback to USB events
static struct notifier_block usb_nb = {
    .notifier_call = usb_notifier_callback, // Callback to be executed on USB events
};

// Module initialization function
static int __init usb_detector_init(void)
{
    memset(usb_log, 0, sizeof(usb_log)); // Clear the log buffer initially

    // Create proc entry file with read-only permissions (0444)
    proc_entry = proc_create(PROC_NAME, 0444, NULL, &proc_fops);
    if (!proc_entry) {
        pr_err("Failed to create /proc/%s\n", PROC_NAME); // Log error if proc entry fails
        return -ENOMEM;             // Return memory allocation error
    }

    usb_register_notify(&usb_nb);   // Register our notifier block for USB events
    pr_info("[USBDetector] Module Loaded\n"); // Log module load success
    return 0;                        // Return success
}

// Module cleanup function
static void __exit usb_detector_exit(void)
{
    usb_unregister_notify(&usb_nb); // Unregister USB event notifier
    proc_remove(proc_entry);        // Remove the proc entry
    pr_info("[USBDetector] Module Unloaded\n"); // Log module unload
}

module_init(usb_detector_init);     // Define the initialization function
module_exit(usb_detector_exit);     // Define the cleanup function

