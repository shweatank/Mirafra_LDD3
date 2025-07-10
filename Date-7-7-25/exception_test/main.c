#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/notifier.h>
#include <linux/kallsyms.h>

static int my_die_handler(struct notifier_block *nb, unsigned long val, void *data)
{
    pr_alert("Custom Exception Handler Invoked! Reason code: %lu\n", val);
    return NOTIFY_DONE;
}

static struct notifier_block my_nb = {
    .notifier_call = my_die_handler,
    .next = NULL,
    .priority = 0,
};

static int __init exception_module_init(void)
{
    int a = 1, b = 0, c;

    pr_info("Registering die notifier...\n");
    register_die_notifier(&my_nb);

    pr_info("Simulating exception: division by zero...\n");

    c = a / b; // This causes a divide-by-zero exception
    pr_info("Result: %d\n", c);

    return 0;
}

static void __exit exception_module_exit(void)
{
    pr_info("Unregistering die notifier...\n");
    unregister_die_notifier(&my_nb);
}

module_init(exception_module_init);
module_exit(exception_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Harsha - Demo");
MODULE_DESCRIPTION("Kernel Exception Handler Example (Divide by Zero)");

