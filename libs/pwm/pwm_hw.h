#ifndef PWM_HW
#define PWM_HW

#include <stdbool.h>
#include <stdint.h>

#include "driverlib/pwm.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "inc/hw_memmap.h"
#include "driverlib/pin_map.h"
#include "driverlib/debug.h"
#include "utils/uartstudio.h"

// pwm prescaler, indexed via pwm_prescale_values enumeration
#define NUM_DIV 7
struct pwm_prescale_struct {
    uint32_t flag;
    double value;
};
const struct pwm_prescale_struct pwm_prescale[NUM_DIV];

extern const uint32_t sysctl_module;
extern const uint32_t pwm_module;

extern const uint32_t pwm_config;

#define PC 8 // pin count 
#define PPC 4 // pin pair count
extern const uint32_t pwm_gen[PC];
extern const uint32_t pwm_pair_gen[PPC];

// indexed via PWM pin enumeration
struct pwm_out_struct {
    uint32_t out;
    uint32_t bit;
};
extern const struct pwm_out_struct pwm_out[PC];

// indexed via PWM pin enumeration
struct gpio_pin_struct {
    uint32_t sysctl;
    uint32_t base;
    uint32_t pin;
    uint32_t config;
};
extern const struct gpio_pin_struct gpio_pin[PC];
#endif
