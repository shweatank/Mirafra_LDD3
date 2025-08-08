#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/pwm.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include <linux/of.h>

#define PWM_PERIOD_NS 1000000     // 1ms period = 1kHz
#define PWM_DUTY_NS 500000        // 50% duty
#define BLINK_INTERVAL_MS 500     // Toggle every 500ms

static struct pwm_device *pwm;
static struct hrtimer blink_timer;
static bool led_on = true;

static enum hrtimer_restart blink_timer_callback(struct hrtimer *timer)
{
    struct pwm_state state;
    ktime_t interval;

    pwm_get_state(pwm, &state);

    if (led_on) {
        state.duty_cycle = 0; // LED OFF
        led_on = false;
    } else {
        state.duty_cycle = PWM_DUTY_NS; // LED ON (50%)
        led_on = true;
    }

    state.period = PWM_PERIOD_NS;
    state.enabled = true;
    pwm_apply_state(pwm, &state);

    interval = ktime_set(0, BLINK_INTERVAL_MS * 1000000);
    hrtimer_forward_now(timer, interval);

    return HRTIMER_RESTART;
}

static int __init pwm_led_init(void)
{
    ktime_t interval;
    struct pwm_state state;

    pr_info("PWM LED Blink Init (new API)\n");

    // You should get from devm or OF ideally. Here just assume pwmchip0/pwm0 for simplicity.
    pwm = pwm_get(0, "led_pwm");
    if (IS_ERR(pwm)) {
        pr_err("Failed to request PWM\n");
        return PTR_ERR(pwm);
    }

    pwm_get_state(pwm, &state);
    state.period = PWM_PERIOD_NS;
    state.duty_cycle = PWM_DUTY_NS;
    state.enabled = true;

    pwm_apply_state(pwm, &state);

    interval = ktime_set(0, BLINK_INTERVAL_MS * 1000000);
    hrtimer_init(&blink_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
    blink_timer.function = blink_timer_callback;
    hrtimer_start(&blink_timer, interval, HRTIMER_MODE_REL);

    return 0;
}

static void __exit pwm_led_exit(void)
{
    struct pwm_state state;

    pr_info("Exiting PWM LED Blink Driver\n");

    hrtimer_cancel(&blink_timer);

    pwm_get_state(pwm, &state);
    state.enabled = false;
    pwm_apply_state(pwm, &state);

    pwm_put(pwm);
}

module_init(pwm_led_init);
module_exit(pwm_led_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("bharath");
MODULE_DESCRIPTION("PWM LED Blink using new PWM API + hrtimer");

