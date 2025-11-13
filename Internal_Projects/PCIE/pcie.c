#include <linux/module.h>
#include <linux/pci.h>
#include <linux/init.h>
#include <linux/kernel.h>

#define MY_VENDOR_ID 0x8086
#define MY_DEVICE_ID 0x100e

static const struct pci_device_id pci_ids[] = {
    { PCI_DEVICE(MY_VENDOR_ID, MY_DEVICE_ID), },
    { 0, }
};
MODULE_DEVICE_TABLE(pci, pci_ids);

static int pci_skel_probe(struct pci_dev *pdev, const struct pci_device_id *ent)
{
    int err, i;
    u32 value;

    pr_info("PCIe skeleton driver: probe called for %04x:%04x\n",
            pdev->vendor, pdev->device);

    // Enable the device
    err = pci_enable_device(pdev);
    if (err) {
        pr_err("Failed to enable PCI device\n");
        return err;
    }

    // Read vendor & device ID from config space
    pci_read_config_dword(pdev, PCI_VENDOR_ID, &value);
    pr_info("Vendor/Device ID (from config space): 0x%x\n", value);

    // Loop through BARs
    for (i = 0; i < 6; i++) {
        resource_size_t start = pci_resource_start(pdev, i);
        resource_size_t len   = pci_resource_len(pdev, i);

        if (!start || !len)
            continue; // BAR not used

        pr_info("BAR[%d]: start=0x%llx, len=%llu\n",
                i, (unsigned long long)start,
                (unsigned long long)len);
    }

    return 0;
}

static void my_remove(struct pci_dev *pdev)
{
    pr_info("PCIe skeleton driver: remove called\n");
    pci_disable_device(pdev);
}

static struct pci_driver my_pci_driver = {
    .name = "pci_skel",
    .id_table = pci_ids,
    .probe = pci_skel_probe,
    .remove = my_remove,
};

static int __init my_init(void)
{
    pr_info("PCIe skeleton driver loaded\n");
    return pci_register_driver(&my_pci_driver);
}

static void __exit my_exit(void)
{
    pci_unregister_driver(&my_pci_driver);
    pr_info("PCIe skeleton driver unloaded\n");
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Hemanth");
MODULE_DESCRIPTION("Simple PCIe driver skeleton for VirtualBox Intel NIC");
