#ifndef PWMLIB_H
#define PWMLIB_H


// explicit types for input values
typedef double period_ms;
typedef double duty_pct;


/* pwm pins
 * 0-3 are dedicated. 
 * 4-7 are shared with QEI, SSI, and timer capture and are on PH4-PH7.
 * 8-15 are not connected/on a dedicated line for another peripheral. */
enum pwm_pin {
     PWM_0,
     PWM_1,
     PWM_2,
     PWM_3,
     PWM_4,
     PWM_5,
     PWM_6,
     PWM_7
};

// pwm pin pairs for generator dependent operations
enum pwm_pin_pair {
    PWM_PAIR_0,
    PWM_PAIR_1,
    PWM_PAIR_2,
    PWM_PAIR_3
};

// pwm function status return values
enum pwm_status {
    PWM_SUCCESS,
    PWM_FAILURE
};


/* Takes a pwm port and attempts to enable it with output disabled.
 * Returns PWM_FAILURE if there was an immediate problem, otherwise 
 * PWM_SUCCESS if status pending. */
extern enum pwm_status pwm_init(enum pwm_pin pwm);

/* Sets the base period for pwm pin pair in milliseconds. Returns 
 * status PWM_SUCCESS, or PWM_FAILURE. */
extern enum pwm_status pwm_set_period(enum pwm_pin_pair pwm_pair, 
                                      period_ms period);

/* Returns the pwm base period for a given output pair */
extern period_ms pwm_get_period(enum pwm_pin_pair pwm_pair);

/* Sets the duty cycle for a pwm output in percent of the base period.
 * Returns status PWM_SUCCESS or PWM_FAILURE. */
extern enum pwm_status pwm_set_duty(enum pwm_pin pwm, duty_pct duty);

/* Returns the duty cycle for a pwm output in percent of the base 
 * period. */
extern duty_pct pwm_get_duty(enum pwm_pin pwm);

/* Enables output on given pwm pin. Returns status PWM_SUCCESS or
 * PWM_FAILURE */
extern enum pwm_status pwm_enable(enum pwm_pin pwm);

/* Disables output on given pwm pin. Returns status PWM_SUCCESS or
 * PWM_FAILURE */
extern enum pwm_status pwm_disable(enum pwm_pin pwm);


#endif
