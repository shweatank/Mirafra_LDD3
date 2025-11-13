#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/usb.h>
#include <linux/mod_devicetable.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Team1");
MODULE_DESCRIPTION("Simple USB Driver - Detect PL2303 USB-TTL Cable");
MODULE_VERSION("1.1");

// Called when a matching USB device is plugged in
static int simple_usb_probe(struct usb_interface *interface, const struct usb_device_id *id)
{
    struct usb_device *dev = interface_to_usbdev(interface);
    char manufacturer[256], product[256];

    printk(KERN_INFO "[USB TTL] Connected Device: VID = 0x%04x, PID = 0x%04x\n",
           dev->descriptor.idVendor, dev->descriptor.idProduct);

    // Try to fetch Manufacturer and Product strings
    if (dev->descriptor.iManufacturer) {
        if (usb_string(dev, dev->descriptor.iManufacturer, manufacturer, sizeof(manufacturer)) > 0)
            printk(KERN_INFO "[USB TTL] Manufacturer: %s\n", manufacturer);
    }

    if (dev->descriptor.iProduct) {
        if (usb_string(dev, dev->descriptor.iProduct, product, sizeof(product)) > 0)
            printk(KERN_INFO "[USB TTL] Product: %s\n", product);
    }

    // Print interface class details (optional)
    printk(KERN_INFO "[USB TTL] Interface Class = 0x%02x, SubClass = 0x%02x, Protocol = 0x%02x\n",
           interface->cur_altsetting->desc.bInterfaceClass,
           interface->cur_altsetting->desc.bInterfaceSubClass,
           interface->cur_altsetting->desc.bInterfaceProtocol);

    return 0;
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

static int __init usb_ttl_init(void)
{
    printk(KERN_INFO "[USB TTL] Initializing driver...\n");
    return usb_register(&simple_usb_driver);
}

static void __exit usb_ttl_exit(void)
{
    printk(KERN_INFO "[USB TTL] Exiting driver...\n");
    usb_deregister(&simple_usb_driver);
}

module_init(usb_ttl_init);
module_exit(usb_ttl_exit);
