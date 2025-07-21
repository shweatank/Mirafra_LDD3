// SPDX-License-Identifier: GPL-2.0
#include <linux/module.h>       // Needed for all modules
#include <linux/kernel.h>       // Needed for KERN_INFO
#include <linux/uaccess.h>      // For copy_from_user and copy_to_user
#include <linux/gpio.h>
#include<linux/timer.h>
#define GPIO_LED 17+512

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Simple /proc driver LED ON and OFF");
MODULE_VERSION("1.0");
static struct timer_list my_timer;

int led_state=1;
static void timer_handler(struct timer_list *t)
{
	if(led_state)
	{
		gpio_set_value(GPIO_LED,1);
		printk("LED ON\n");
		led_state=0;
	}
	else{
		gpio_set_value(GPIO_LED,0);
		printk("LED OFF\n");
		led_state=1;
	}
    
    // Re-arm the timer for 5 seconds
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(3000));
}

// Module init
static int __init driver_init(void)
{

    if (!gpio_is_valid(GPIO_LED)) {
        pr_err("Invalid GPIO %d\n", GPIO_LED);
        return -ENODEV;
    }

    if (gpio_request(GPIO_LED, "LED_GPIO")) {
        pr_err("GPIO %d request failed\n", GPIO_LED);
        return -EBUSY;
    }

    gpio_direction_output(GPIO_LED, 0); // Set as output and turn OFF
	timer_setup(&my_timer, timer_handler, 0);
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(2000));

    return 0;
}

// Module exit
static void __exit driver_exit(void)
{
    del_timer_sync(&my_timer);
    gpio_set_value(GPIO_LED, 0); // Ensure LED is off
    gpio_free(GPIO_LED);
    //pr_info("procfs_driver: Unloaded. /proc/%s removed. GPIO %d released.\n", PROC_NAME, GPIO_LED);
}

module_init(driver_init);
module_exit(driver_exit);
