#include "servo.h"

static const period_ms period[SERVO_COUNT] = {10.0};
static const duty_pct neutral[SERVO_COUNT] = {15.0};
static const double conversion[SERVO_COUNT] = {155.0};

// initialise servo signal and set to neutral
void servo_init(enum servos servo, enum pwm_pin pin) {
    pwm_init(pin);
    pwm_set_prescaler(DIV64);
    pwm_set_period((uint32_t)(pin / 2), period[servo]);
    pwm_set_duty(pin, neutral[servo]);
    pwm_enable(pin);
}

/**
 * @param servo: the servo to use
 * @param pin the pin to use
 * @param servo_deg the position in degrees
 * set servo to position in degrees from neutral
 */
void servo_write(enum servos servo, enum pwm_pin pin, servo_deg position) {
    period_ms duty = neutral[servo] + (position / conversion[servo]);
    pwm_set_duty(pin, duty);
}
