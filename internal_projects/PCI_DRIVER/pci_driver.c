/*
 * pci_driver.c - Generic PCI Device Driver
 * 
 * This driver demonstrates PCI device initialization, memory mapping,
 * interrupt handling, and character device interface
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/pci.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/slab.h>

#define DRIVER_NAME "pci_demo"
#define DEVICE_NAME "pcidev"

/* PCI Vendor and Device IDs - Change these to match your device */
#define PCI_VENDOR_ID_EXAMPLE    0x8086  /* Intel vendor ID */
#define PCI_DEVICE_ID_EXAMPLE    0x100E  /* Intel e1000 for demo */

/* Device structure */
struct pci_dev_data {
    struct pci_dev *pdev;
    struct cdev cdev;
    dev_t devt;
    struct class *class;
    struct device *device;
    
    /* Memory mapped I/O */
    void __iomem *mmio_base;
    resource_size_t mmio_start;
    resource_size_t mmio_len;
    
    /* DMA buffer */
    void *dma_buffer;
    dma_addr_t dma_handle;
    size_t dma_size;
    
    /* IRQ */
    int irq;
    int irq_enabled;
    
    /* Status */
    int opened;
    spinlock_t lock;
};

static int major_number;
static struct pci_dev_data *pci_data;

/* Function prototypes */
static int pci_probe(struct pci_dev *pdev, const struct pci_device_id *id);
static void pci_remove(struct pci_dev *pdev);

/* PCI device ID table */
static struct pci_device_id pci_ids[] = {
    { PCI_DEVICE(PCI_VENDOR_ID_EXAMPLE, PCI_DEVICE_ID_EXAMPLE) },
    { 0, }
};
MODULE_DEVICE_TABLE(pci, pci_ids);

/* Interrupt handler */
static irqreturn_t pci_irq_handler(int irq, void *dev_id)
{
    struct pci_dev_data *data = (struct pci_dev_data *)dev_id;
    unsigned long flags;
    
    spin_lock_irqsave(&data->lock, flags);
    
    pr_info("%s: Interrupt received on IRQ %d\n", DRIVER_NAME, irq);
    
    /* Handle interrupt - read status registers, etc. */
    /* For demo purposes, we just acknowledge it */
    
    spin_unlock_irqrestore(&data->lock, flags);
    
    return IRQ_HANDLED;
}

/* File operations - open */
static int pci_open(struct inode *inode, struct file *file)
{
    struct pci_dev_data *data;
    unsigned long flags;
    
    data = container_of(inode->i_cdev, struct pci_dev_data, cdev);
    file->private_data = data;
    
    spin_lock_irqsave(&data->lock, flags);
    if (data->opened) {
        spin_unlock_irqrestore(&data->lock, flags);
        return -EBUSY;
    }
    data->opened = 1;
    spin_unlock_irqrestore(&data->lock, flags);
    
    pr_info("%s: Device opened\n", DRIVER_NAME);
    return 0;
}

/* File operations - release */
static int pci_release(struct inode *inode, struct file *file)
{
    struct pci_dev_data *data = file->private_data;
    unsigned long flags;
    
    spin_lock_irqsave(&data->lock, flags);
    data->opened = 0;
    spin_unlock_irqrestore(&data->lock, flags);
    
    pr_info("%s: Device closed\n", DRIVER_NAME);
    return 0;
}

/* File operations - read */
static ssize_t pci_read(struct file *file, char __user *buf, 
                        size_t count, loff_t *ppos)
{
    struct pci_dev_data *data = file->private_data;
    char message[256];
    size_t len;
    
    if (!data->mmio_base) {
        return -ENODEV;
    }
    
    /* Create status message */
    len = snprintf(message, sizeof(message),
                   "PCI Device Status:\n"
                   "Vendor ID: 0x%04x\n"
                   "Device ID: 0x%04x\n"
                   "IRQ: %d\n"
                   "MMIO Base: 0x%llx\n"
                   "MMIO Length: 0x%llx\n"
                   "DMA Buffer: %p\n",
                   data->pdev->vendor,
                   data->pdev->device,
                   data->irq,
                   (unsigned long long)data->mmio_start,
                   (unsigned long long)data->mmio_len,
                   data->dma_buffer);
    
    if (*ppos >= len)
        return 0;
    
    if (count > len - *ppos)
        count = len - *ppos;
    
    if (copy_to_user(buf, message + *ppos, count))
        return -EFAULT;
    
    *ppos += count;
    return count;
}

/* File operations - write */
static ssize_t pci_write(struct file *file, const char __user *buf,
                         size_t count, loff_t *ppos)
{
    struct pci_dev_data *data = file->private_data;
    char *kernel_buf;
    ssize_t ret = count;
    
    if (!data->mmio_base) {
        return -ENODEV;
    }
    
    if (count > PAGE_SIZE)
        count = PAGE_SIZE;
    
    kernel_buf = kmalloc(count, GFP_KERNEL);
    if (!kernel_buf)
        return -ENOMEM;
    
    if (copy_from_user(kernel_buf, buf, count)) {
        kfree(kernel_buf);
        return -EFAULT;
    }
    
    /* Write data to device - example: copy to DMA buffer */
    if (data->dma_buffer && count <= data->dma_size) {
        memcpy(data->dma_buffer, kernel_buf, count);
        pr_info("%s: Wrote %zu bytes to DMA buffer\n", DRIVER_NAME, count);
    }
    
    kfree(kernel_buf);
    return ret;
}

/* File operations structure */
static struct file_operations pci_fops = {
    .owner = THIS_MODULE,
    .open = pci_open,
    .release = pci_release,
    .read = pci_read,
    .write = pci_write,
};

/* PCI probe function - called when device is found */
static int pci_probe(struct pci_dev *pdev, const struct pci_device_id *id)
{
    int ret;
    struct pci_dev_data *data;
    
    pr_info("%s: Probing PCI device %04x:%04x\n", 
            DRIVER_NAME, pdev->vendor, pdev->device);
    
    /* Allocate device data structure */
    data = kzalloc(sizeof(struct pci_dev_data), GFP_KERNEL);
    if (!data) {
        dev_err(&pdev->dev, "Failed to allocate memory\n");
        return -ENOMEM;
    }
    
    data->pdev = pdev;
    spin_lock_init(&data->lock);
    pci_set_drvdata(pdev, data);
    pci_data = data;
    
    /* Enable PCI device */
    ret = pci_enable_device(pdev);
    if (ret) {
        dev_err(&pdev->dev, "Failed to enable PCI device\n");
        goto err_free_data;
    }
    
    /* Request PCI regions */
    ret = pci_request_regions(pdev, DRIVER_NAME);
    if (ret) {
        dev_err(&pdev->dev, "Failed to request PCI regions\n");
        goto err_disable_device;
    }
    
    /* Set DMA mask */
    ret = dma_set_mask_and_coherent(&pdev->dev, DMA_BIT_MASK(64));
    if (ret) {
        ret = dma_set_mask_and_coherent(&pdev->dev, DMA_BIT_MASK(32));
        if (ret) {
            dev_err(&pdev->dev, "Failed to set DMA mask\n");
            goto err_release_regions;
        }
    }
    
    /* Enable bus mastering */
    pci_set_master(pdev);
    
    /* Map BAR0 (Memory mapped I/O) */
    data->mmio_start = pci_resource_start(pdev, 0);
    data->mmio_len = pci_resource_len(pdev, 0);
    
    if (data->mmio_len > 0) {
        data->mmio_base = pci_iomap(pdev, 0, data->mmio_len);
        if (!data->mmio_base) {
            dev_err(&pdev->dev, "Failed to map BAR0\n");
            ret = -ENOMEM;
            goto err_clear_master;
        }
        dev_info(&pdev->dev, "BAR0 mapped at 0x%llx (length: 0x%llx)\n",
                 (unsigned long long)data->mmio_start,
                 (unsigned long long)data->mmio_len);
    }
    
    /* Allocate DMA buffer */
    data->dma_size = PAGE_SIZE;
    data->dma_buffer = dma_alloc_coherent(&pdev->dev, data->dma_size,
                                          &data->dma_handle, GFP_KERNEL);
    if (!data->dma_buffer) {
        dev_warn(&pdev->dev, "Failed to allocate DMA buffer\n");
    } else {
        dev_info(&pdev->dev, "DMA buffer allocated at 0x%llx\n",
                 (unsigned long long)data->dma_handle);
    }
    
    /* Request IRQ */
    data->irq = pdev->irq;
    if (data->irq > 0) {
        ret = request_irq(data->irq, pci_irq_handler, IRQF_SHARED,
                         DRIVER_NAME, data);
        if (ret) {
            dev_warn(&pdev->dev, "Failed to request IRQ %d\n", data->irq);
            data->irq_enabled = 0;
        } else {
            data->irq_enabled = 1;
            dev_info(&pdev->dev, "IRQ %d registered\n", data->irq);
        }
    }
    
    /* Register character device */
    ret = alloc_chrdev_region(&data->devt, 0, 1, DEVICE_NAME);
    if (ret) {
        dev_err(&pdev->dev, "Failed to allocate char device region\n");
        goto err_free_irq;
    }
    
    major_number = MAJOR(data->devt);
    
    cdev_init(&data->cdev, &pci_fops);
    data->cdev.owner = THIS_MODULE;
    
    ret = cdev_add(&data->cdev, data->devt, 1);
    if (ret) {
        dev_err(&pdev->dev, "Failed to add char device\n");
        goto err_unregister_chrdev;
    }
    
    /* Create device class */
    data->class = class_create(THIS_MODULE, DEVICE_NAME);
    if (IS_ERR(data->class)) {
        ret = PTR_ERR(data->class);
        dev_err(&pdev->dev, "Failed to create device class\n");
        goto err_cdev_del;
    }
    
    /* Create device node */
    data->device = device_create(data->class, &pdev->dev, data->devt, 
                                  NULL, DEVICE_NAME);
    if (IS_ERR(data->device)) {
        ret = PTR_ERR(data->device);
        dev_err(&pdev->dev, "Failed to create device\n");
        goto err_class_destroy;
    }
    
    dev_info(&pdev->dev, "PCI device probed successfully\n");
    dev_info(&pdev->dev, "Device node: /dev/%s\n", DEVICE_NAME);
    
    return 0;

err_class_destroy:
    class_destroy(data->class);
err_cdev_del:
    cdev_del(&data->cdev);
err_unregister_chrdev:
    unregister_chrdev_region(data->devt, 1);
err_free_irq:
    if (data->irq_enabled)
        free_irq(data->irq, data);
    if (data->dma_buffer)
        dma_free_coherent(&pdev->dev, data->dma_size, 
                         data->dma_buffer, data->dma_handle);
    if (data->mmio_base)
        pci_iounmap(pdev, data->mmio_base);
err_clear_master:
    pci_clear_master(pdev);
err_release_regions:
    pci_release_regions(pdev);
err_disable_device:
    pci_disable_device(pdev);
err_free_data:
    kfree(data);
    return ret;
}

/* PCI remove function - called when device is removed */
static void pci_remove(struct pci_dev *pdev)
{
    struct pci_dev_data *data = pci_get_drvdata(pdev);
    
    pr_info("%s: Removing PCI device\n", DRIVER_NAME);
    
    /* Remove device node */
    if (data->device)
        device_destroy(data->class, data->devt);
    
    /* Destroy class */
    if (data->class)
        class_destroy(data->class);
    
    /* Remove character device */
    cdev_del(&data->cdev);
    unregister_chrdev_region(data->devt, 1);
    
    /* Free IRQ */
    if (data->irq_enabled)
        free_irq(data->irq, data);
    
    /* Free DMA buffer */
    if (data->dma_buffer)
        dma_free_coherent(&pdev->dev, data->dma_size,
                         data->dma_buffer, data->dma_handle);
    
    /* Unmap MMIO */
    if (data->mmio_base)
        pci_iounmap(pdev, data->mmio_base);
    
    /* Disable bus mastering */
    pci_clear_master(pdev);
    
    /* Release regions */
    pci_release_regions(pdev);
    
    /* Disable device */
    pci_disable_device(pdev);
    
    /* Free data structure */
    kfree(data);
    
    dev_info(&pdev->dev, "PCI device removed\n");
}

/* PCI driver structure */
static struct pci_driver pci_driver = {
    .name = DRIVER_NAME,
    .id_table = pci_ids,
    .probe = pci_probe,
    .remove = pci_remove,
};

/* Module initialization */
static int __init pci_driver_init(void)
{
    int ret;
    
    pr_info("%s: Initializing PCI driver\n", DRIVER_NAME);
    
    ret = pci_register_driver(&pci_driver);
    if (ret) {
        pr_err("%s: Failed to register PCI driver\n", DRIVER_NAME);
        return ret;
    }
    
    pr_info("%s: PCI driver registered successfully\n", DRIVER_NAME);
    return 0;
}

/* Module cleanup */
static void __exit pci_driver_exit(void)
{
    pr_info("%s: Unloading PCI driver\n", DRIVER_NAME);
    pci_unregister_driver(&pci_driver);
    pr_info("%s: PCI driver unloaded\n", DRIVER_NAME);
}

module_init(pci_driver_init);
module_exit(pci_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("MK");
MODULE_DESCRIPTION("Generic PCI Device Driver");
MODULE_VERSION("1.0");
MODULE_DEVICE_TABLE(pci, pci_ids);
