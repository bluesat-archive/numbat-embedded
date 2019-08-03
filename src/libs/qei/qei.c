#include <stdbool.h>
#include <stdint.h>

#include "driverlib/qei.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "inc/hw_memmap.h"
#include "driverlib/pin_map.h"
#include "driverlib/debug.h"

const uint32_t sysctl_module = SYSCTL_PERIPH_QEI0;
void ext_handler();
void (*int_handler)(uint32_t);

void ext_handler() {
    uint32_t status = QEIIntStatus(QEI0_BASE, false);
    int_handler(status);
}

int qei_init(uint16_t qei_num, uint32_t time_period, void *handler(uint32_t)) {

    //If peripheral is not enabled
    if (SysCtlPeripheralReady(sysctl_module) == false) {
        //Enable it
        SysCtlPeripheralEnable(sysctl_module);

        //loop until it is enabled
        while(SysCtlPeripheralReady(sysctl_module) == false) {
            ;
        }
        
    }

    //GPIO config
    if (SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOD) == false) {
        SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);

        while (SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOD) == false) {
            ;
        }
    }
    GPIOPinTypeQEI(GPIO_PORTB_BASE, GPIO_PIN_6);
    GPIOPinConfigure(GPIO_PD6_PHA0);

    GPIOPinTypeQEI(GPIO_PORTB_BASE, GPIO_PIN_7);
    GPIOPinConfigure(GPIO_PD7_PHB0);

    //Configure QEI
    QEIConfigure(QEI0_BASE, QEI_CONFIG_CAPTURE_A | QEI_CONFIG_NO_RESET | QEI_CONFIG_QUADRATURE | QEI_CONFIG_NO_SWAP, 3999);

    //Enable velocity capture
    QEIVelocityConfigure(QEI0_BASE, QEI_VELDIV_1, time_period);
    QEIVelocityEnable(QEI0_BASE);
    QEIPositionSet(QEI0_BASE, 0);
    QEIEnable(QEI0_BASE);

    //Register the interrupt  handler
    int_handler = handler;  
    QEIIntEnable(QEI0_BASE, QEI_INTERROR | QEI_INTTIMER);                                   
    QEIIntRegister(QEI0_BASE, ext_handler);
    
    return QEI0_BASE;
}