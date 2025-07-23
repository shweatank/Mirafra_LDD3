#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/pwm.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include <linux/platform_device.h>
#include <linux/of.h>

#define PWM_CHIP 0           // pwmchip0
#define PWM_CHANNEL 0        // pwm0
#define PWM_PERIOD_NS 1000000   // 1 ms period = 1 KHz
#define PWM_DUTY_NS 500000      // 50% duty = medium brightness
#define BLINK_INTERVAL_MS 500   // 500ms ON/OFF toggle

static struct pwm_device *pwm;
static struct hrtimer blink_timer;
static bool led_on = true;

static enum hrtimer_restart blink_timer_callback(struct hrtimer *timer)
{
    ktime_t interval;

    if (led_on) {
        pwm_config(pwm, 0, PWM_PERIOD_NS);  // LED off
        led_on = false;
    } else {
        pwm_config(pwm, PWM_DUTY_NS, PWM_PERIOD_NS);  // LED on (50% brightness)
        led_on = true;
    }

    interval = ktime_set(0, BLINK_INTERVAL_MS * 1000000);  // convert ms to ns
    hrtimer_forward_now(timer, interval);
    return HRTIMER_RESTART;
}

static int __init pwm_led_driver_init(void)
{
    struct pwm_device *p;
    ktime_t interval;
    int ret;

    pr_info("PWM LED Driver Init (no DTS, no ioremap)\n");

    // Use pwm_request_from_chip if available
    p = pwm_get(PWM_CHANNEL, "led_pwm");
    if (IS_ERR(p)) {
        pr_err("Failed to request PWM channel %d\n", PWM_CHANNEL);
        return PTR_ERR(p);
    }

    pwm = p;

    ret = pwm_config(pwm, PWM_DUTY_NS, PWM_PERIOD_NS);
    if (ret < 0) {
        pr_err("Failed to configure PWM\n");
        pwm_put(pwm);
        return ret;
    }

    ret = pwm_enable(pwm);
    if (ret < 0) {
        pr_err("Failed to enable PWM\n");
        pwm_put(pwm);
        return ret;
    }

    // Initialize high-resolution timer
    interval = ktime_set(0, BLINK_INTERVAL_MS * 1000000);
    hrtimer_init(&blink_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
    blink_timer.function = blink_timer_callback;
    hrtimer_start(&blink_timer, interval, HRTIMER_MODE_REL);

    pr_info("PWM LED started blinking\n");
    return 0;
}

static void __exit pwm_led_driver_exit(void)
{
    pr_info("Exiting PWM LED driver\n");

    hrtimer_cancel(&blink_timer);

    pwm_disable(pwm);
    pwm_put(pwm);
}

module_init(pwm_led_driver_init);
module_exit(pwm_led_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("TechDhaba - Prashant");
MODULE_DESCRIPTION("LED PWM driver using PWM + hrtimer without DTS or ioremap");
