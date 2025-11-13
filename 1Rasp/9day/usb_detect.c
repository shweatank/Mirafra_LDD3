#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/usb.h>

// Define supported device (this matches ALL devices for demo)
#define USB_ANY_ID USB_DEVICE_ID_MATCH_VENDOR | USB_DEVICE_ID_MATCH_PRODUCT

static int usb_probe(struct usb_interface *interface, const struct usb_device_id *id) {
    struct usb_device *udev = interface_to_usbdev(interface);

    printk(KERN_INFO "[USB] Device Plugged In\n");
    printk(KERN_INFO "[USB] Vendor ID: %04x, Product ID: %04x\n",
           id->idVendor, id->idProduct);

    printk(KERN_INFO "[USB] Manufacturer: %s\n", udev->manufacturer ? udev->manufacturer : "N/A");
    printk(KERN_INFO "[USB] Product: %s\n", udev->product ? udev->product : "N/A");
    printk(KERN_INFO "[USB] Serial Number: %s\n", udev->serial ? udev->serial : "N/A");

    return 0;
}

static void usb_disconnect(struct usb_interface *interface) {
    printk(KERN_INFO "[USB] Device Removed\n");
}

// Match table: match all USB devices (not recommended in production)
static struct usb_device_id usb_table[] = {
    { USB_INTERFACE_INFO(USB_CLASS_VENDOR_SPEC, 0, 0) },  // Match vendor-specific class
    {}  // Terminating entry
};

MODULE_DEVICE_TABLE(usb, usb_table);

static struct usb_driver usb_driver = {
    .name = "usb_detector_driver",
    .id_table = usb_table,
    .probe = usb_probe,
    .disconnect = usb_disconnect,
};

static int __init usb_init(void) {
    printk(KERN_INFO "USB Detector Module Loaded\n");
    return usb_register(&usb_driver);
}

static void __exit usb_exit(void) {
    printk(KERN_INFO "USB Detector Module Unloaded\n");
    usb_deregister(&usb_driver);
}

module_init(usb_init);
module_exit(usb_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Hemanth");
MODULE_DESCRIPTION("USB Detection Driver Example");

