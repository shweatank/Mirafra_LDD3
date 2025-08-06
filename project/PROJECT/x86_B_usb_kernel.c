#include <linux/module.h>             // Core header for modules
#include <linux/kernel.h>             // printk()
#include <linux/usb.h>                // USB core
#include <linux/mod_devicetable.h>    // usb_device_id table

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Team1");
MODULE_DESCRIPTION("Simple USB Driver - Detect PL2303 USB-TTL Cable");
MODULE_VERSION("1.0");

// Called when a matching USB device is plugged in
static int simple_usb_probe(struct usb_interface *interface, const struct usb_device_id *id)
{
    struct usb_device *dev = interface_to_usbdev(interface);

    printk(KERN_INFO "[USB TTL] Connected Device: VID = 0x%04x, PID = 0x%04x\n",
           dev->descriptor.idVendor, dev->descriptor.idProduct);

    printk(KERN_INFO "[USB TTL] Interface Class = 0x%02x, SubClass = 0x%02x, Protocol = 0x%02x\n",
           interface->cur_altsetting->desc.bInterfaceClass,
           interface->cur_altsetting->desc.bInterfaceSubClass,
           interface->cur_altsetting->desc.bInterfaceProtocol);

    return 0; // success
}

// Called when the device is removed
static void simple_usb_disconnect(struct usb_interface *interface)
{
    printk(KERN_INFO "[USB TTL] Device disconnected\n");
}

// Match only Prolific PL2303 USB-TTL cable
static const struct usb_device_id simple_table[] = {
    { USB_DEVICE(0x067b, 0x2303) },  // Prolific PL2303
    {} // Terminating entry
};
MODULE_DEVICE_TABLE(usb, simple_table);

// USB driver structure
static struct usb_driver simple_usb_driver = {
    .name       = "usb_ttl_logger",
    .id_table   = simple_table,
    .probe      = simple_usb_probe,
    .disconnect = simple_usb_disconnect,
};

// Register driver
static int __init usb_ttl_init(void)
{
    printk(KERN_INFO "[USB TTL] Initializing driver...\n");
    return usb_register(&simple_usb_driver);
}

// Deregister driver
static void __exit usb_ttl_exit(void)
{
    printk(KERN_INFO "[USB TTL] Exiting driver...\n");
    usb_deregister(&simple_usb_driver);
}

module_init(usb_ttl_init);
module_exit(usb_ttl_exit);

