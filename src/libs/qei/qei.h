#ifndef QEILIB_H
#define QEILIB_H

enum qei_status {
    QEI_SUCCESS,
    QEI_FAILURE,
};

void qei_init(uint16_t qei_num, uint32_t time_period, void *handler(uint32_t));
#endif