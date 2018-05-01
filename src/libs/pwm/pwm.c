#include "pwm.h"
#include "pwm_hw.h"

#define c_assert(e)    ((e) ? (true) : (tst_debugging("%s,%d: assertion '%s' failed\n", __FILE__, __LINE__, #e), false)) 
#define pwm_assert(e)    if (c_assert(e) == false) return PWM_FAILURE

void tst_debugging(char *frmt_str, char *file, int line, char *err) {
#ifdef DEBUG
#include "utils/uartstdio.h"
    UARTprintf(frmt_str, file, line, err);
#endif 
}

bool pwm_valid(enum pwm_pin val) {
    return (val >= PWM0 && val <= PWM7);
}

bool pwm_pair_valid(enum pwm_pin_pair val) {
    return (val >= PWM_PAIR0 && val <= PWM_PAIR3);
}

bool pwm_prescale_valid(enum pwm_prescale_values val) {
    return (val >= DIV1 && val <= DIV64);
}    

enum pwm_prescale_values get_pre(void) {
    uint32_t ui32Config = SysCtlPWMClockGet();

    switch (ui32Config) {
        case SYSCTL_PWMDIV_1 : return DIV1;
        case SYSCTL_PWMDIV_2 : return DIV2;
        case SYSCTL_PWMDIV_4 : return DIV4;
        case SYSCTL_PWMDIV_8 : return DIV8;
        case SYSCTL_PWMDIV_16 : return DIV16;
        case SYSCTL_PWMDIV_32 : return DIV32;
        case SYSCTL_PWMDIV_64 : return DIV64;
    };

    pwm_assert(false); // should never reach here
}

/* Input: abstract PWM pin
 * Output: status code
 * Side Effects (in order):
 *     - Enables PWM module
 *     - Enables GPIO port
 *     - Configures GPIO pin for pwm output
 *     - Configures PWM generator
 *     - Disables PWM pin output */
enum pwm_status pwm_init(enum pwm_pin pwm) {
    pwm_assert(pwm_valid(pwm));

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

    // Configure PWM generator
    PWMGenConfigure(pwm_module, pwm_gen[pwm], pwm_config);
    
    // Enable PWM generator
    PWMGenEnable(pwm_module, pwm_gen[pwm]);

    return PWM_SUCCESS;
}

/* Input: abstract PWM prescale value
 * Output: status code
 * Side Effects:
 *     - Sets pwm module clock prescale value */
enum pwm_status pwm_set_prescaler(enum pwm_prescale_values pre) {
    pwm_assert(pwm_prescale_valid(pre));

    // Set pwm sysclk prescaler
    SysCtlPWMClockSet(pwm_prescale[pre].flag);
    
    return PWM_SUCCESS;
}

/* Input: abstract PWM pin pair, PWM carrier period in milliseconds
 * Output: status code */
enum pwm_status pwm_set_period(enum pwm_pin_pair pwm_pair, period_ms period) {
    pwm_assert(pwm_pair_valid(pwm_pair));

    enum pwm_prescale_values prescale = get_pre();

    uint32_t f_pwm = SysCtlClockGet() / pwm_prescale[prescale].value;
    uint32_t period_counts = (uint32_t)(period / 1000.0 * f_pwm);

    pwm_assert(period_counts < 65536); // period counter is 16-bits

    PWMGenPeriodSet(pwm_module, pwm_pair_gen[pwm_pair], period_counts);

    return PWM_SUCCESS;
}

/* Input: abstract PWM pin pair
 * Output: PWM carrier period in milliseconds */
period_ms pwm_get_period(enum pwm_pin_pair pwm_pair) {
    pwm_assert(pwm_pair_valid(pwm_pair));

    enum pwm_prescale_values prescale = get_pre();

    uint32_t period_counts = PWMGenPeriodGet(pwm_module, pwm_pair_gen[pwm_pair]);
    uint32_t f_pwm_mhz = SysCtlClockGet() / 1000.0 / pwm_prescale[prescale].value;

    return (period_ms)(period_counts / f_pwm_mhz);
}

/* Input: abstract PWM pin, PWM duty in percent
 * Output: status code */
enum pwm_status pwm_set_duty(enum pwm_pin pwm, duty_pct duty) {
    pwm_assert(pwm_valid(pwm));
    pwm_assert(duty >= 0.0 && duty <= 100.0);

    uint32_t period_counts = PWMGenPeriodGet(pwm_module, pwm_gen[pwm]);
    uint32_t duty_period = (uint32_t)(period_counts * duty / 100.0);

    tst_debugging("%s %d %s\n", "", duty_period, "");

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
    pwm_assert(pwm_valid(pwm));

    uint32_t period_counts = PWMGenPeriodGet(pwm_module, pwm_gen[pwm]);
    uint32_t duty_period = PWMPulseWidthGet(pwm_module, pwm_out[pwm].out);

    return ((duty_pct)duty_period) / ((duty_pct)period_counts) * 100.0;
}

/* Input: abstract PWM pin
 * Output: status code
 * Side Effect: enables PWM pin */
enum pwm_status pwm_enable(enum pwm_pin pwm) {
    pwm_assert(pwm_valid(pwm));

    PWMOutputState(pwm_module, pwm_out[pwm].bit, true);

    return PWM_SUCCESS;
}

/* Input: abstract PWM pin
 * Output: status code
 * Side Effect: disables PWM pin */
enum pwm_status pwm_disable(enum pwm_pin pwm) {
    pwm_assert(pwm_valid(pwm));

    PWMOutputState(pwm_module, pwm_out[pwm].bit, false);

    return PWM_SUCCESS;
}
