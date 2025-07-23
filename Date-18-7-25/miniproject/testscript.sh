#!/bin/bash

PWM_CHIP=/sys/class/pwm/pwmchip0
PWM=$PWM_CHIP/pwm0

# Export PWM0 if not already exported
if [ ! -d "$PWM" ]; then
    echo 0 | sudo tee $PWM_CHIP/export
    sleep 0.5
fi

# Disable before configuring
echo 0 | sudo tee $PWM/enable

# Set low frequency (visible blink)
sudo tee $PWM/period <<< 500000000    # 0.5s period (2Hz)
sudo tee $PWM/duty_cycle <<< 250000000  # 50% duty (LED ON 0.25s, OFF 0.25s)

# Enable PWM
echo 1 | sudo tee $PWM/enable

echo "✅ PWM started on GPIO18 (pin 12). LED should blink at 2Hz."
