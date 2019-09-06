#include <stdint.h>
#include "pwm.h"

#ifdef __cplusplus
extern "C" {
#endif
typedef double servo_deg;
typedef double servo_rad;

#define PI ((double)3.14159265359)
#define SERVO_COUNT 3

enum servos {
    HS_785HB,
    HS_311,
    GENERIC
};

// initialise servo signal and set to neutral
void servo_init(enum servos servo, enum pwm_pin pin);

/**
 * @param servo: the servo to use
 * @param pin the pin to use
 * @param position the position in degrees
 * set servo to position in degrees from neutral
 */
void servo_write(enum servos servo, enum pwm_pin pin, servo_deg position);

/**
 * @param servo: the servo to use
 * @param pin the pin to use
 * @param position the position in radians
 * set servo to position in radians from neutral
 */
inline void servo_write_rads(enum servos servo, enum pwm_pin pin, servo_rad position) {
    servo_write(servo, pin, position * 180.0 / PI);
}

#ifdef __cplusplus
}
#endif
