#include "pwmlib.h"

#include <stdbool.h>
#include <stdint.h>

#include "driverlib/pwm.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "inc/hw_memmap.h"
#include "driverlib/pin_map.h"


#define PC 8 // number of pwm pins


/* TODO:
 *  - Check SysCtlPWMClockSet vs PWMClockSet
 *  - Figure out how to calculate f_cpu
 */


// Shared constants
static const uint32_t sysctl_module = SYSCTL_PERIPH_PWM0;
static const uint32_t pwm_module = PWM0_BASE;
static const uint32_t pwm_prescale = PWM_SYSCLK_DIV_64;
static const uint32_t pwm_prescale_value = 64;
static const uint32_t 
    pwm_generator_lut[PC] = {PWM_GEN_0, PWM_GEN_0, PWM_GEN_1, PWM_GEN_1,
                             PWM_GEN_2, PWM_GEN_2, PWM_GEN_3, PWM_GEN_3};
static const uint32_t
    pwm_generator_pair_lut[PC/2] = {PWM_GEN_0, PWM_GEN_1, PWM_GEN_2,
                                    PWM_GEN_3};
static const uint32_t 
    pwm_config = PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC 
               | PWM_GEN_MODE_DBG_RUN | PWM_GEN_MODE_GEN_NO_SYNC;
static const uint32_t
    pwm_out_lut[PC] = {PWM_OUT_0, PWM_OUT_1, PWM_OUT_2, PWM_OUT_3,
                       PWM_OUT_4, PWM_OUT_5, PWM_OUT_6, PWM_OUT_7};
static const uint32_t 
    pwm_out_bit_lut[PC] = {PWM_OUT_0_BIT, PWM_OUT_1_BIT, PWM_OUT_2_BIT,
                           PWM_OUT_3_BIT, PWM_OUT_4_BIT, PWM_OUT_5_BIT,
                           PWM_OUT_6_BIT, PWM_OUT_7_BIT};

// choose board type
#define PWMLIB_TEST_BOARD
#ifdef PWMLIB_TEST_BOARD
// Test board constants
static const uint32_t 
    sysctl_gpio_lut[PC] = {SYSCTL_PERIPH_GPIOB, SYSCTL_PERIPH_GPIOB,
                           SYSCTL_PERIPH_GPIOB, SYSCTL_PERIPH_GPIOB,
                           SYSCTL_PERIPH_GPIOE, SYSCTL_PERIPH_GPIOE,
                           SYSCTL_PERIPH_GPIOC, SYSCTL_PERIPH_GPIOC};
static const uint32_t 
    gpio_port_lut[PC] = {GPIO_PORTB_BASE, GPIO_PORTB_BASE,
                         GPIO_PORTB_BASE, GPIO_PORTB_BASE,
                         GPIO_PORTE_BASE, GPIO_PORTE_BASE,
                         GPIO_PORTC_BASE, GPIO_PORTC_BASE};
static const uint32_t 
    gpio_pin_lut[PC] = {GPIO_PIN_6, GPIO_PIN_7, GPIO_PIN_4, GPIO_PIN_5,
                        GPIO_PIN_4, GPIO_PIN_5, GPIO_PIN_4, GPIO_PIN_5};
static const uint32_t
    gpio_config_lut[PC] = {GPIO_PB6_M0PWM0, GPIO_PB7_M0PWM1,
                           GPIO_PB4_M0PWM2, GPIO_PB5_M0PWM3,
                           GPIO_PE4_M0PWM4, GPIO_PE5_M0PWM5,
                           GPIO_PC4_M0PWM6, GPIO_PC5_M0PWM7};
#else
// Generic PCB constants
static const uint32_t 
    sysctl_gpio_lut[PC] = {SYSCTL_PERIPH_GPIOH, SYSCTL_PERIPH_GPIOH,
                           SYSCTL_PERIPH_GPIOH, SYSCTL_PERIPH_GPIOH,
                           SYSCTL_PERIPH_GPIOH, SYSCTL_PERIPH_GPIOH,
                           SYSCTL_PERIPH_GPIOH, SYSCTL_PERIPH_GPIOH};
static const uint32_t 
    gpio_port_lut[PC] = {GPIO_PORTH_BASE, GPIO_PORTH_BASE,
                         GPIO_PORTH_BASE, GPIO_PORTH_BASE,
                         GPIO_PORTH_BASE, GPIO_PORTH_BASE,
                         GPIO_PORTH_BASE, GPIO_PORTH_BASE};
static const uint32_t 
    gpio_pin_lut[PC] = {GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_3,
                        GPIO_PIN_4, GPIO_PIN_5, GPIO_PIN_6, GPIO_PIN_7};
static const uint32_t
    gpio_config_lut[PC] = {GPIO_PH0_M0PWM0, GPIO_PH1_M0PWM1,
                           GPIO_PH2_M0PWM2, GPIO_PH3_M0PWM3,
                           GPIO_PH4_M0PWM4, GPIO_PH5_M0PWM5,
                           GPIO_PH6_M0PWM6, GPIO_PH7_M0PWM7};
#endif
    
                                


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

    uint32_t sysctl_gpio = sysctl_gpio_lut[pwm];
    // if gpio port not enabled
    if (SysCtlPeripheralReady(sysctl_gpio) == false) {
        // enable PWM port
        SysCtlPeripheralEnable(sysctl_gpio);
        // Wait until port is responsive
        while (SysCtlPeripheralReady(sysctl_gpio) == false)
            ;
    }

    uint32_t gpio_port = gpio_port_lut[pwm];
    uint32_t gpio_pin = gpio_pin_lut[pwm];
    // set gpio pin type
    GPIOPinTypePWM(gpio_port, gpio_pin);

    uint32_t gpio_config = gpio_config_lut[pwm];
    // configure gpio pin
    GPIOPinConfigure(gpio_config);

    // Set pwm sysclk prescaler to /64
    PWMClockSet(pwm_module, pwm_prescale);

    uint32_t pwm_generator = pwm_generator_lut[pwm];
    // Configure PWM generator
    PWMGenConfigure(pwm_module, pwm_generator, pwm_config);
    
    // Enable PWM generator
    PWMGenEnable(pwm_module, pwm_generator);

    return PWM_SUCCESS;
}

/* Input: abstract PWM pin
 * Output: status code
 * Checks:
 *     - PWM module clock
 *     - PWM module state
 *     - GPIO port state
 *     - GPIO pin configuration
 *     - PWM generator configuration */
enum pwm_status pwm_init_status(enum pwm_pin pwm) {
    return PWM_SUCCESS;
}

/* Input: abstract PWM pin pair, PWM carrier period in milliseconds
 * Output: status code */
enum pwm_status pwm_set_period(enum pwm_pin_pair pwm_pair, 
                               period_ms period) {
    uint32_t pwm_generator = pwm_generator_pair_lut[pwm_pair];

    uint32_t f_pwm = 50000000;
    uint32_t period_counts = (uint32_t)(period / 1000.0 * f_pwm);
    
    PWMGenPeriodSet(pwm_module, pwm_generator, period_counts);

    return PWM_SUCCESS;
}

/* Input: abstract PWM pin pair
 * Output: PWM carrier period in milliseconds */
period_ms pwm_get_period(enum pwm_pin_pair pwm_pair) {
    uint32_t pwm_generator = pwm_generator_pair_lut[pwm_pair];

    uint32_t period_counts = PWMGenPeriodGet(pwm_module, pwm_generator);
    uint32_t f_pwm_mhz = SysCtlClockGet() / pwm_prescale_value / 1000.0;

    return (period_ms)(period_counts / f_pwm_mhz);
}

/* Input: abstract PWM pin, PWM duty in percent
 * Output: status code */
enum pwm_status pwm_set_duty(enum pwm_pin pwm, duty_pct duty) {
    uint32_t pwm_generator = pwm_generator_lut[pwm];
    uint32_t period_counts = PWMGenPeriodGet(pwm_module, pwm_generator);

    uint32_t duty_period = (uint32_t)(period_counts * duty / 100.0);

    uint32_t pwm_out = pwm_out_lut[pwm];

    if (duty_period > period_counts)
        duty_period = period_counts;

    PWMPulseWidthSet(pwm_module, pwm_out, duty_period);

    return PWM_SUCCESS;
}

/* Input: abstract PWM pin
 * Output: PWM duty in percent */
duty_pct pwm_get_duty(enum pwm_pin pwm) {
    uint32_t pwm_generator = pwm_generator_lut[pwm];
    uint32_t period_counts = PWMGenPeriodGet(pwm_module, pwm_generator);

    uint32_t pwm_out = pwm_out_lut[pwm];

    uint32_t duty_period = PWMPulseWidthGet(pwm_module, pwm_out);

    return ((duty_pct)duty_period) / ((duty_pct)period_counts);
}

/* Input: abstract PWM pin
 * Output: status code
 * Side Effect: enables PWM pin */
enum pwm_status pwm_enable(enum pwm_pin pwm) {
    uint32_t pwm_out_bit = pwm_out_bit_lut[pwm];
    PWMOutputState(pwm_module, pwm_out_bit, true);

    return PWM_SUCCESS;
}

/* Input: abstract PWM pin
 * Output: status code
 * Side Effect: disables PWM pin */
enum pwm_status pwm_disable(enum pwm_pin pwm) {
    uint32_t pwm_out_bit = pwm_out_bit_lut[pwm];
    PWMOutputState(pwm_module, pwm_out_bit, false);

    return PWM_SUCCESS;
}
