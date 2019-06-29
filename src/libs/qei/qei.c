#include <stdbool.h>
#include <stdint.h>

#include "driverlib/qei.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "inc/hw_memmap.h"
#include "driverlib/pin_map.h"
#include "driverlib/debug.h"

enum qei_status {
    QEI_SUCCESS,
    QEI_FAILURE,
};
enum qei_status qei_init();

const uint32_t sysctl_module = SYSCTL_PERIPH_QEI0;
void *ext_handler();
void int_handler(void *fp());

enum qei_status qei_init(uint16_t qei_num, uint32_t time_period, void *ext_int_handler()) {
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
    QEIVELOCITYCONFIGURE(QEI0_BASE, QEI_VELDIV_1, 1000);
    QEIVelocityEnable(QEI0_BASE);
    QEIPositionSet(QEI0_BASE, 0);
    QEIEnable(QEI0_BASE);

    return QEI_SUCCESS;
}

int32_t qei_velocity() {
    int32_t dir = QEIDirectionGet(QEI0_BASE);
    //vel is number of pulses in the time period
    uint32_t vel = QEIVelocityGet(QEI0_BASE);
    //Since time period is 1
    vel = vel * 60;
    dir = vel * dir;
    return vel;
}