#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/usb.h>
#include <linux/mod_devicetable.h>

// Add this manually for newer kernels like 6.8
#ifndef USB_ANY_ID
#define USB_ANY_ID 0xffff
#endif

MODULE_LICENSE("GPL");
MODULE_AUTHOR("TechDhaba");
MODULE_DESCRIPTION("Simple USB Driver - Print VID & PID");

static int simple_usb_probe(struct usb_interface *interface, const struct usb_device_id *id)
{
    struct usb_device *dev = interface_to_usbdev(interface);
    printk(KERN_INFO "Simple USB Driver: Vendor ID = %04x, Product ID = %04x\n",
           dev->descriptor.idVendor, dev->descriptor.idProduct);
    return 0;
}

static void simple_usb_disconnect(struct usb_interface *interface)
{
    printk(KERN_INFO "Simple USB Driver: Device disconnected\n");
}

static const struct usb_device_id simple_table[] = {
    { USB_DEVICE(USB_ANY_ID, USB_ANY_ID) },
    {}
};
MODULE_DEVICE_TABLE(usb, simple_table);

static struct usb_driver simple_usb_driver = {
    .name = "simple_usb_driver",
    .id_table = simple_table,
    .probe = simple_usb_probe,
    .disconnect = simple_usb_disconnect,
};

static int __init simple_usb_init(void)
{
    return usb_register(&simple_usb_driver);
}

static void __exit simple_usb_exit(void)
{
    usb_deregister(&simple_usb_driver);
}

module_init(simple_usb_init);
module_exit(simple_usb_exit);
