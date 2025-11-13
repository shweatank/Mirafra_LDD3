#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/usb.h>

static const struct usb_device_id usb_detect_table[] = {
    { USB_DEVICE_INFO(USB_CLASS_PER_INTERFACE, 0, 0) },  // Match all interfaces
    {}
};
MODULE_DEVICE_TABLE(usb, usb_detect_table);

// Called on device insertion
static int usb_detect_probe(struct usb_interface *interface, const struct usb_device_id *id)
{
    struct usb_device *udev = interface_to_usbdev(interface);
    __u16 vid = le16_to_cpu(udev->descriptor.idVendor);
    __u16 pid = le16_to_cpu(udev->descriptor.idProduct);

    pr_info("[usb_detect] Device plugged in -> Vendor ID: 0x%04x, Product ID: 0x%04x\n", vid, pid);
    return 0;
}

// Called on device removal
static void usb_detect_disconnect(struct usb_interface *interface)
{
    pr_info("[usb_detect] Device removed\n");
}

static struct usb_driver usb_detect_driver = {
    .name = "usb_detect",
    .id_table = usb_detect_table,
    .probe = usb_detect_probe,
    .disconnect = usb_detect_disconnect,
};

static int __init usb_detect_init(void)
{
    pr_info("[usb_detect] Module loaded\n");
    return usb_register(&usb_detect_driver);
}

static void __exit usb_detect_exit(void)
{
    pr_info("[usb_detect] Module unloaded\n");
    usb_deregister(&usb_detect_driver);
}

module_init(usb_detect_init);
module_exit(usb_detect_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sunny");
MODULE_DESCRIPTION("USB Device Hotplug Detection with Vendor/Product ID Print");

