
#include <linux/module.h>
#include <linux/init.h>
#include <linux/pci.h>
#include <linux/slab.h>

#define DUMMY_VENDOR_ID 0x1234
#define DUMMY_DEVICE_ID 0x5678

static struct pci_dev *dummy_pdev;

/* probe() called when device matches */
static int dummy_pci_probe(struct pci_dev *pdev, const struct pci_device_id *id)
{
    pr_info("dummy_pci: probe() called for %04x:%04x\n",
            pdev->vendor, pdev->device);
    return 0;
}

/* remove() called when device removed */
static void dummy_pci_remove(struct pci_dev *pdev)
{
    pr_info("dummy_pci: remove() called for %04x:%04x\n",
            pdev->vendor, pdev->device);
}

/* Match table */
static const struct pci_device_id dummy_pci_ids[] = {
    { PCI_DEVICE(DUMMY_VENDOR_ID, DUMMY_DEVICE_ID) },
    { 0, }
};
MODULE_DEVICE_TABLE(pci, dummy_pci_ids);

/* PCI driver struct */
static struct pci_driver dummy_pci_driver = {
    .name     = "dummy_pci_driver",
    .id_table = dummy_pci_ids,
    .probe    = dummy_pci_probe,
    .remove   = dummy_pci_remove,
};

/*
 * Instead of using internal pci_root_ops or pci_device_probe,
 * we’ll simulate a fake PCI device via pci_register_device (not exported)
 * so we use a simpler device_add() to attach to driver core.
 * This gives a dummy “PCI-like” device node for testing driver binding.
 */
static int __init dummy_pci_init(void)
{
    int ret;

    pr_info("dummy_pci: init\n");

    /* Register our dummy PCI driver */
    ret = pci_register_driver(&dummy_pci_driver);
    if (ret)
        return ret;

    /* Create a fake PCI device in software */
    dummy_pdev = kzalloc(sizeof(*dummy_pdev), GFP_KERNEL);
    if (!dummy_pdev) {
        pci_unregister_driver(&dummy_pci_driver);
        return -ENOMEM;
    }

    /* Fill fields to look like a real PCI device */
    dummy_pdev->vendor = DUMMY_VENDOR_ID;
    dummy_pdev->device = DUMMY_DEVICE_ID;
    dummy_pdev->bus = pci_find_bus(0, 0); /* attach to existing bus 0000:00 */
    if (!dummy_pdev->bus)
        pr_warn("dummy_pci: no PCI bus found, still proceeding\n");

    /* Initialize PCI device structure */
    pci_set_drvdata(dummy_pdev, NULL);
    device_initialize(&dummy_pdev->dev);
    dev_set_name(&dummy_pdev->dev, "dummy_pci_device");

    /* Set device type and parent bus */
    dummy_pdev->dev.bus = &pci_bus_type;

    /* Add device to system */
    ret = device_add(&dummy_pdev->dev);
    if (ret) {
        pr_err("dummy_pci: device_add failed (%d)\n", ret);
        put_device(&dummy_pdev->dev);
        kfree(dummy_pdev);
        pci_unregister_driver(&dummy_pci_driver);
        return ret;
    }

    pr_info("dummy_pci: dummy device registered (vendor=%04x device=%04x)\n",
            DUMMY_VENDOR_ID, DUMMY_DEVICE_ID);

    return 0;
}

static void __exit dummy_pci_exit(void)
{
    pr_info("dummy_pci: exit\n");

    if (dummy_pdev) {
        device_del(&dummy_pdev->dev);
        put_device(&dummy_pdev->dev);
        kfree(dummy_pdev);
    }

    pci_unregister_driver(&dummy_pci_driver);
    pr_info("dummy_pci: cleaned up\n");
}

module_init(dummy_pci_init);
module_exit(dummy_pci_exit);

MODULE_AUTHOR("Pavan");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Dummy PCI device driver");
