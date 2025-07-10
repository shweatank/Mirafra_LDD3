#include <linux/module.h>
#include <linux/init.h>
#include <linux/input.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("YourName");
MODULE_DESCRIPTION("Mouse Input Event Logger using Input Handler");

// Forward declaration
static const struct input_device_id mouse_ids[];

struct mouse_handle_data {
    struct input_handle handle;
    struct input_device *dev;
};

// Called when a matching input device is found
static int mouse_connect(struct input_handler *handler, struct input_dev *dev, const struct input_device_id *id)
{
    struct mouse_handle_data *data;

    // Filter only devices with REL_X and REL_Y (mice)
    if (!test_bit(EV_REL, dev->evbit) ||
        !test_bit(REL_X, dev->relbit) ||
        !test_bit(REL_Y, dev->relbit)) {
        return -ENODEV;
    }

    data = kzalloc(sizeof(struct mouse_handle_data), GFP_KERNEL);
    if (!data)
        return -ENOMEM;

    data->dev = dev;
    data->handle.dev = dev;
    data->handle.name = "mouse_input_handle";
    data->handle.handler = handler;
    data->handle.private = data;

    if (input_register_handle(&data->handle)) {
        kfree(data);
        return -ENODEV;
    }

    if (input_open_device(&data->handle)) {
        input_unregister_handle(&data->handle);
        kfree(data);
        return -ENODEV;
    }

    printk(KERN_INFO "[MOUSE] Mouse connected: %s\n", dev->name);
    return 0;
}

// Called on mouse movement/button press
static void mouse_event(struct input_handle *handle, unsigned int type, unsigned int code, int value)
{
    if (type == EV_REL) {
        if (code == REL_X)
            printk(KERN_INFO "[MOUSE] Moved X by %d\n", value);
        else if (code == REL_Y)
            printk(KERN_INFO "[MOUSE] Moved Y by %d\n", value);
    } else if (type == EV_KEY) {
        if (code == BTN_LEFT || code == BTN_RIGHT)
            printk(KERN_INFO "[MOUSE] Button %s %s\n",
                   (code == BTN_LEFT) ? "Left" : "Right",
                   value ? "Pressed" : "Released");
    }
}

// Called when the input device is removed
static void mouse_disconnect(struct input_handle *handle)
{
    struct mouse_handle_data *data = handle->private;

    printk(KERN_INFO "[MOUSE] Mouse disconnected: %s\n", handle->dev->name);

    input_close_device(handle);
    input_unregister_handle(handle);
    kfree(data);
}

// Filter table (match all devices, filtered in connect)
static const struct input_device_id mouse_ids[] = {
    { .driver_info = 1 },
    { }
};

// Input handler setup
static struct input_handler mouse_input_handler = {
    .event     = mouse_event,
    .connect   = mouse_connect,
    .disconnect = mouse_disconnect,
    .name      = "mouse_input_handler",
    .id_table  = mouse_ids,
};

static int __init mouse_init(void)
{
    return input_register_handler(&mouse_input_handler);
}

static void __exit mouse_exit(void)
{
    input_unregister_handler(&mouse_input_handler);
}

module_init(mouse_init);
module_exit(mouse_exit);

