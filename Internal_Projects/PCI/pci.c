#include <linux/module.h>
#include <linux/pci.h>

#define DRV_NAME "my_multi_pci_driver"

/* 
 * Add all devices you want to handle here.
 * Example: Intel Ethernet + Intel Audio + Intel PCI Bridge 
 * Replace VendorID/DeviceID with ones from your `lspci -nn`.
 */
static const struct pci_device_id pci_ids[] = {
    { PCI_DEVICE(0x8086, 0x9a14) },  
    { PCI_DEVICE(0x8086, 0xa0e8) },  
    { PCI_DEVICE(0x8086, 0xa0c8) },  
    { 0, } 
};
MODULE_DEVICE_TABLE(pci, pci_ids);

static int my_pci_probe(struct pci_dev *pdev, const struct pci_device_id *ent)
{
    pr_info("%s: probe called for device [%04x:%04x]\n",
            DRV_NAME, pdev->vendor, pdev->device);
    return 0;
}

static void my_pci_remove(struct pci_dev *pdev)
{
    pr_info("%s: remove called for device [%04x:%04x]\n",
            DRV_NAME, pdev->vendor, pdev->device);
}

static struct pci_driver my_pci_driver = {
    .name     = DRV_NAME,
    .id_table = pci_ids,
    .probe    = my_pci_probe,
    .remove   = my_pci_remove,
};

module_pci_driver(my_pci_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Priya");
MODULE_DESCRIPTION("Minimal safe PCI driver for multiple devices");

