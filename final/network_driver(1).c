#include <linux/module.h>         // For all kernel modules
#include <linux/netdevice.h>     // For struct net_device and related APIs
#include <linux/etherdevice.h>   // For Ethernet-specific helper functions
#include <linux/skbuff.h>        // For struct sk_buff (socket buffer)

#define DRV_NAME "dummy_net"     // Driver name used in printk messages

static struct net_device *dummy_netdev;  // Pointer to represent our dummy network device

// Transmit function (Tx path)
static netdev_tx_t dummy_start_xmit(struct sk_buff *skb, struct net_device *dev)
{
    // Log the packet length to kernel log
    pr_info("%s: Simulated TX packet, len = %u\n", DRV_NAME, skb->len);

    // Free the skb since we are not actually transmitting it
    dev_kfree_skb(skb);

    // Update device statistics
    dev->stats.tx_packets++;     // Increment total packets sent
    dev->stats.tx_bytes += skb->len; // Add bytes sent

    return NETDEV_TX_OK;         // Indicate successful transmission
}

// Called when device is brought up
static int dummy_open(struct net_device *dev)
{
    pr_info("%s: device opened\n", DRV_NAME);

    // Allow the network stack to send packets via this interface
    netif_start_queue(dev);

    return 0; // Success
}

// Called when device is brought down
static int dummy_stop(struct net_device *dev)
{
    pr_info("%s: device stopped\n", DRV_NAME);

    // Stop packet transmission
    netif_stop_queue(dev);

    return 0; // Success
}

// Assign our custom open/close/transmit functions
static const struct net_device_ops dummy_netdev_ops = {
    .ndo_open       = dummy_open,        // on 'ifconfig up'
    .ndo_stop       = dummy_stop,        // on 'ifconfig down'
    .ndo_start_xmit = dummy_start_xmit,  // on send packet
};

// Setup function: called during alloc_netdev()
static void dummy_setup(struct net_device *dev)
{
    ether_setup(dev);                     // Setup Ethernet-like device (sets MTU, etc.)

    dev->netdev_ops = &dummy_netdev_ops;  // Assign our operation callbacks

    dev->flags |= IFF_NOARP;              // Disable ARP for this dummy device

    dev->features |= NETIF_F_HW_CSUM;     // Pretend we support hardware checksumming

    eth_hw_addr_random(dev);              // Assign a random MAC address to the device
}

// Module init function - registers the device
static int __init dummy_init(void)
{
    // Allocate a network device with no private data, name dummy%d
    dummy_netdev = alloc_netdev(0, "dummy%d", NET_NAME_UNKNOWN, dummy_setup);
    if (!dummy_netdev)
        return -ENOMEM;   // Out of memory

    // Register the network device with the kernel
    if (register_netdev(dummy_netdev)) {
        free_netdev(dummy_netdev); // Free if registration fails
        return -ENODEV;
    }

    pr_info("%s: loaded\n", DRV_NAME);
    return 0;
}

// Module exit function - unregisters device
static void __exit dummy_exit(void)
{
    unregister_netdev(dummy_netdev); // Remove from kernel
    free_netdev(dummy_netdev);       // Free memory
    pr_info("%s: unloaded\n", DRV_NAME);
}

// Register init and exit functions
module_init(dummy_init);
module_exit(dummy_exit);

// Module metadata
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Batch 4");
MODULE_DESCRIPTION("Minimal Dummy Network Driver");

