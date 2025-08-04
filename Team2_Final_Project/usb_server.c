#include <stdio.h>                        // For standard input/output functions
#include <stdlib.h>                       // For general utilities like EXIT_FAILURE
#include <libusb-1.0/libusb.h>            // For libusb functions and types

// Flag used to indicate if a USB device has been detected
static int device_detected = 0;

// Callback function that's triggered on USB hotplug events (device insert/remove)
static int hotplug_callback(struct libusb_context *ctx, struct libusb_device *dev,
                            libusb_hotplug_event event, void *user_data) {
    struct libusb_device_descriptor desc; // Structure to hold device descriptor
    (void)ctx;                            // Unused parameter (avoiding compiler warning)
    (void)user_data;                      // Unused user data

    // If a new device has arrived
    if (event == LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED) {
        // Retrieve the device descriptor
        if (libusb_get_device_descriptor(dev, &desc) == 0) {
            printf("New USB device inserted:\n");
            printf("  Vendor ID : %04x\n", desc.idVendor);   // Print Vendor ID
            printf("  Product ID: %04x\n", desc.idProduct);  // Print Product ID
            device_detected = 1; // Set flag to indicate detection (exit condition)
        }
    }
    return 0; // Return 0 to continue handling events
}

int main(void) {
    libusb_context *ctx = NULL;                // libusb context pointer
    int rc;                                    // Return code variable
    libusb_hotplug_callback_handle handle;     // Handle for the callback

    // Initialize libusb and get a context
    rc = libusb_init(&ctx);
    if (rc != 0) {
        // If initialization fails, print error and exit
        fprintf(stderr, "Failed to init libusb: %s\n", libusb_error_name(rc));
        return EXIT_FAILURE;
    }

    // Check if the system supports hotplug events
    if (!libusb_has_capability(LIBUSB_CAP_HAS_HOTPLUG)) {
        fprintf(stderr, "Hotplug not supported\n");
        libusb_exit(ctx); // Clean up context before exiting
        return EXIT_FAILURE;
    }

    // Register the hotplug callback to detect device arrival
    rc = libusb_hotplug_register_callback(ctx,
                                          LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED, // Event: device inserted
                                          0,                                   // No flags
                                          LIBUSB_HOTPLUG_MATCH_ANY,            // Match any vendor ID
                                          LIBUSB_HOTPLUG_MATCH_ANY,            // Match any product ID
                                          LIBUSB_HOTPLUG_MATCH_ANY,            // Match any device class
                                          hotplug_callback,                    // Our callback function
                                          NULL,                                // No user data
                                          &handle);                            // Output: callback handle

    // If callback registration fails
    if (rc != LIBUSB_SUCCESS) {
        fprintf(stderr, "Failed to register callback: %s\n", libusb_error_name(rc));
        libusb_exit(ctx);
        return EXIT_FAILURE;
    }

    printf("Waiting for new USB device...\n");

    // Loop until a device is detected (flag is set)
    while (!device_detected) {
        libusb_handle_events(ctx); // Handle pending events (blocking call)
    }

    // Once device is detected, clean up: remove callback and exit libusb
    libusb_hotplug_deregister_callback(ctx, handle);
    libusb_exit(ctx);
    return 0;
}

