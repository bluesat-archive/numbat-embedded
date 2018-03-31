#include <stdint.h>
#include "pwm.h"

typedef double servo_deg;

#define SERVO_COUNT 1
enum servos {
    HS_785HB
};

// initialise servo signal and set to neutral
void servo_init(enum servos servo, enum pwm_pin pin);

// set servo to position in degrees from neutral
void servo_write(enum servos servo, enum pwm_pin pin, servo_deg position);
