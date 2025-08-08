#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/pwm.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>

#define PWM_CHANNEL        0            // Use PWM channel 0
#define PWM_PERIOD_NS      1000000      // 1ms (1kHz) period
#define PWM_DUTY_NS        300000       // 30% brightness (adjustable)
#define BLINK_INTERVAL_MS  500          // 500ms ON/OFF interval

static struct pwm_device *pwm;
static struct hrtimer blink_timer;
static bool is_on = true;

// Timer callback — toggles PWM on/off
static enum hrtimer_restart blink_callback(struct hrtimer *timer)
{
    ktime_t interval;

    if (is_on) {
        pwm_config(pwm, 0, PWM_PERIOD_NS); // LED off
        is_on = false;
    } else {
        pwm_config(pwm, PWM_DUTY_NS, PWM_PERIOD_NS); // LED on with brightness
        is_on = true;
    }

    interval = ktime_set(0, BLINK_INTERVAL_MS * 1000000); // ms to ns
    hrtimer_forward_now(timer, interval);
    return HRTIMER_RESTART;
}

static int __init led_pwm_init(void)
{
    ktime_t interval;
    int ret;

    pr_info("LED PWM Module (no ioremap, no DTS)\n");

    pwm = pwm_request(PWM_CHANNEL, "led_pwm_hw");
    if (IS_ERR(pwm)) {
        pr_err("PWM request failed\n");
        return PTR_ERR(pwm);
    }

    // Set initial brightness
    ret = pwm_config(pwm, PWM_DUTY_NS, PWM_PERIOD_NS);
    if (ret < 0) {
        pr_err("PWM config failed\n");
        pwm_free(pwm);
        return ret;
    }

    ret = pwm_enable(pwm);
    if (ret < 0) {
        pr_err("PWM enable failed\n");
        pwm_free(pwm);
        return ret;
    }

    // Start hardware timer (hrtimer uses hardware timers)
    interval = ktime_set(0, BLINK_INTERVAL_MS * 1000000);
    hrtimer_init(&blink_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
    blink_timer.function = blink_callback;
    hrtimer_start(&blink_timer, interval, HRTIMER_MODE_REL);

    pr_info("LED PWM blinking started\n");
    return 0;
}

static void __exit led_pwm_exit(void)
{
    pr_info("Exiting LED PWM module\n");

    hrtimer_cancel(&blink_timer);
    pwm_disable(pwm);
    pwm_free(pwm);
}

module_init(led_pwm_init);
module_exit(led_pwm_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Bharath");
MODULE_DESCRIPTION("LED control with hardware PWM and hardware timer (no DTS, no ioremap)");
