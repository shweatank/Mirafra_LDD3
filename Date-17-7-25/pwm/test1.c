#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/pwm.h>
#include <linux/hrtimer.h>
#include <linux/of.h>

#define PWM_PERIOD_NS      1000000     // 1ms = 1KHz
#define PWM_DUTY_NS        500000      // 50% duty cycle
#define BLINK_INTERVAL_MS  500         // 500 ms blink rate

static struct pwm_device *pwm;
static struct hrtimer blink_timer;
static bool led_on = true;

static enum hrtimer_restart blink_timer_callback(struct hrtimer *timer)
{
    ktime_t interval;

    if (led_on) {
        pwm_config(pwm, 0, PWM_PERIOD_NS);  // LED OFF
        led_on = false;
    } else {
        pwm_config(pwm, PWM_DUTY_NS, PWM_PERIOD_NS);  // LED ON
        led_on = true;
    }

    interval = ktime_set(0, BLINK_INTERVAL_MS * 1000000);
    hrtimer_forward_now(timer, interval);
    return HRTIMER_RESTART;
}

static int __init pwm_led_driver_init(void)
{
    ktime_t interval;
    int ret;

    pr_info("PWM LED Driver Init (modern API)\n");

    pwm = pwm_get(NULL, "pwm0");  // "pwm0" corresponds to pwmchip0/pwm0
    if (IS_ERR(pwm)) {
        pr_err("Failed to get PWM device\n");
        return PTR_ERR(pwm);
    }

    ret = pwm_config(pwm, PWM_DUTY_NS, PWM_PERIOD_NS);
    if (ret < 0) {
        pr_err("PWM config failed\n");
        pwm_put(pwm);
        return ret;
    }

    ret = pwm_enable(pwm);
    if (ret < 0) {
        pr_err("PWM enable failed\n");
        pwm_put(pwm);
        return ret;
    }

    interval = ktime_set(0, BLINK_INTERVAL_MS * 1000000);
    hrtimer_init(&blink_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
    blink_timer.function = blink_timer_callback;
    hrtimer_start(&blink_timer, interval, HRTIMER_MODE_REL);

    pr_info("PWM LED blinking started\n");
    return 0;
}

static void __exit pwm_led_driver_exit(void)
{
    pr_info("PWM LED Driver Exit\n");

    hrtimer_cancel(&blink_timer);
    pwm_disable(pwm);
    pwm_put(pwm);
}

module_init(pwm_led_driver_init);
module_exit(pwm_led_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Harsha sri sampath Kadali");
MODULE_DESCRIPTION("PWM LED Blinker using modern PWM API (kernel 6.x+)");

