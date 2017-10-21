#include "pwmlib.h"
#include "pwm_hw.h"

static enum pwm_prescale_values prescale = DIV1;

bool pwm_valid(enum pwm_pin val) {
    return (val >= PWM0 && val <= PWM7);
}

bool pwm_pair_valid(enum pwm_pin_pair val) {
    return (val >= PWM_PAIR0 && val <= PWM_PAIR3);
}

bool pwm_prescale_valid(enum pwm_prescale_values val) {
    return (val >= DIV1 && val <= DIV64);
}    

enum pwm_status pwm_set_prescaler(enum pwm_prescale_values pre) {
    ASSERT(pwm_prescale_valid(pre));

    prescale = pre;
    
    return PWM_SUCCESS;
}

/* Input: abstract PWM pin
 * Output: status code
 * Side Effects (in order):
 *     - Enables PWM module
 *     - Enables GPIO port
 *     - Configures GPIO pin for pwm output
 *     - Sets PWM module clock divider
 *     - Configures PWM generator
 *     - Disables PWM pin output */
enum pwm_status pwm_init(enum pwm_pin pwm) {
    ASSERT(pwm_valid(pwm));

    // if pwm module note enabled
    if (SysCtlPeripheralReady(sysctl_module) == false) {
        // enable PWM module
        SysCtlPeripheralEnable(sysctl_module);
        // Wait until module is responsive
        while (SysCtlPeripheralReady(sysctl_module) == false)
            ;
    }
    
    // disable PWM output, return if failure
    if (pwm_disable(pwm) == PWM_FAILURE)
        return PWM_FAILURE;

    // if gpio port not enabled
    if (SysCtlPeripheralReady(gpio_pin[pwm].sysctl) == false) {
        // enable PWM port
        SysCtlPeripheralEnable(gpio_pin[pwm].sysctl);
        // Wait until port is responsive
        while (SysCtlPeripheralReady(gpio_pin[pwm].sysctl) == false)
            ;
    }

    // set gpio pin type
    GPIOPinTypePWM(gpio_pin[pwm].base, gpio_pin[pwm].pin);

    // configure gpio pin
    GPIOPinConfigure(gpio_pin[pwm].config);

    // Set pwm sysclk prescaler to /64
    SysCtlPWMClockSet(pwm_prescale[prescale].flag);

    // Configure PWM generator
    PWMGenConfigure(pwm_module, pwm_gen[pwm], pwm_config);
    
    // Enable PWM generator
    PWMGenEnable(pwm_module, pwm_gen[pwm]);

    return PWM_SUCCESS;
}

/* Input: abstract PWM pin pair, PWM carrier period in milliseconds
 * Output: status code */
enum pwm_status pwm_set_period(enum pwm_pin_pair pwm_pair, period_ms period) {
    ASSERT(pwm_pair_valid(pwm_pair));

    uint32_t f_pwm = SysCtlClockGet() / pwm_prescale[prescale].value;
    uint32_t period_counts = (uint32_t)(period / 1000.0 * f_pwm);
    
    PWMGenPeriodSet(pwm_module, pwm_pair_gen[pwm_pair], period_counts);

    return PWM_SUCCESS;
}

/* Input: abstract PWM pin pair
 * Output: PWM carrier period in milliseconds */
period_ms pwm_get_period(enum pwm_pin_pair pwm_pair) {
    ASSERT(pwm_pair_valid(pwm_pair));

    uint32_t period_counts = PWMGenPeriodGet(pwm_module, pwm_pair_gen[pwm_pair]);
    uint32_t f_pwm_mhz = SysCtlClockGet() / 1000.0 / pwm_prescale[prescale].value;

    return (period_ms)(period_counts / f_pwm_mhz);
}

/* Input: abstract PWM pin, PWM duty in percent
 * Output: status code */
enum pwm_status pwm_set_duty(enum pwm_pin pwm, duty_pct duty) {
    ASSERT(pwm_valid(pwm));
    ASSERT(duty >= 0.0 && duty <= 100.0);

    uint32_t period_counts = PWMGenPeriodGet(pwm_module, pwm_gen[pwm]);
    uint32_t duty_period = (uint32_t)(period_counts * duty / 100.0);

    // 100% duty requires that the output be set to to 0% and inverted
    if (duty_period >= period_counts) {
        PWMPulseWidthSet(pwm_module, pwm_out[pwm].out, 0);
        PWMOutputInvert(pwm_module, pwm_out[pwm].bit, true);
    } else {
        PWMOutputInvert(pwm_module, pwm_out[pwm].bit, false);
        PWMPulseWidthSet(pwm_module, pwm_out[pwm].out, duty_period);
    }

    return PWM_SUCCESS;
}

/* Input: abstract PWM pin
 * Output: PWM duty in percent */
duty_pct pwm_get_duty(enum pwm_pin pwm) {
    ASSERT(pwm_valid(pwm));

    uint32_t period_counts = PWMGenPeriodGet(pwm_module, pwm_gen[pwm]);
    uint32_t duty_period = PWMPulseWidthGet(pwm_module, pwm_out[pwm].out);

    return ((duty_pct)duty_period) / ((duty_pct)period_counts) * 100.0;
}

/* Input: abstract PWM pin
 * Output: status code
 * Side Effect: enables PWM pin */
enum pwm_status pwm_enable(enum pwm_pin pwm) {
    ASSERT(pwm_valid(pwm));

    PWMOutputState(pwm_module, pwm_out[pwm].bit, true);

    return PWM_SUCCESS;
}

/* Input: abstract PWM pin
 * Output: status code
 * Side Effect: disables PWM pin */
enum pwm_status pwm_disable(enum pwm_pin pwm) {
    ASSERT(pwm_valid(pwm));

    PWMOutputState(pwm_module, pwm_out[pwm].bit, false);

    return PWM_SUCCESS;
}
