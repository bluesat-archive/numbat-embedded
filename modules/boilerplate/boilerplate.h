#ifndef BOILERPLATE_H
#define BOILERPLATE_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "inc/hw_can.h"
#include "inc/hw_ints.h"
#include "inc/hw_nvic.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/rom.h"
#include "driverlib/systick.h"
#include "driverlib/uart.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/timer.h"
#include "driverlib/can.h"
#include "driverlib/interrupt.h"
#include "utils/uartstdio.h"


#ifdef __cplusplus
extern "C" {
#endif

void InitializeUARTStdio(void);

void InitializeFPU(void);


void fatal(const uint8_t error_id);

#ifdef __cplusplus
}
#endif

#endif
