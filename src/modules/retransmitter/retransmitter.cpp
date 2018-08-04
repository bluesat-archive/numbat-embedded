#include <rtos-kochab.h>
#include "boilerplate.h"
#include "rtos-kochab.h"
#include "ros.hpp"
#include "NodeHandle.hpp"
#include "std_msgs/Float64.hpp"
#include "Publisher.hpp"
#include "message.h"

#define BUF_SIZE 8


char *topics[NUM_MSG] = {
    "/front_left_wheel_axel_controller/command",
    "/front_right_wheel_axel_controller/command",
    "/back_left_wheel_axel_controller/command",
    "/back_right_wheel_axel_controller/command",
    "/front_left_wheel_swerve_controller/command",
    "/front_right_wheel_swerve_controller/command",
    "/back_left_wheel_swerve_controller/command",
    "/back_right_wheel_swerve_controller/command"
};


#define SYSTICKS_PER_SECOND     100

#define CAN_BITRATE 500000
#define CAN_MSG_LEN 8

static tCANMsgObject rx_object;
static uint8_t can_input_buffer[CAN_MSG_LEN];

static void init_can(void);
static void write_can(uint32_t message_id, uint8_t *buffer, uint32_t buffer_size);

extern "C" bool tick_irq(void) {
    rtos_timer_tick();
    return true;
}

static void wait_for_msg() {
    while (true) {
        //while (UARTgetc() != startMagic[0]) {
        //    ;
        //}
        volatile unsigned char inv;
        do {
            inv = UARTgetc();
        } while(inv != startMagic[0]);

        bool retry = false;
        for (int i = 1; i <= 3; i++) {
            inv = UARTgetc();
            if (inv != startMagic[i]) {
                retry = true;
                break;
            }
        }

        if (!retry) {
            break;
        }
    }
}

struct messageAdapter serial;
ros_echronos::NodeHandle * shared_nh;
ros_echronos::Publisher<std_msgs::Float64> * publishers[NUM_MSG];

union Data buf_reading;
union Data buf_ready;
union Data buf_sending;

volatile bool is_buffer_ready = false;

extern "C" void task_retransmitter_fn(void) {
    ros_echronos::NodeHandle nh;
    shared_nh = &nh;
    nh.init("retransmit_fn", "retransmit_fn", RTOS_INTERRUPT_EVENT_ID_CAN_RECEIVE_EVENT, 0);

    // you would have to use the new operator to initialise these in a loop and we can't do that
    std_msgs::Float64 msg_buf_front_left_a[BUF_SIZE];
    ros_echronos::Publisher<std_msgs::Float64> front_left_a(topics[0], (std_msgs::Float64*)msg_buf_front_left_a, BUF_SIZE, false);
    publishers[0] = &front_left_a;
    std_msgs::Float64 msg_buf_front_right_a[BUF_SIZE];
    ros_echronos::Publisher<std_msgs::Float64> front_right_a(topics[1], (std_msgs::Float64*)msg_buf_front_right_a, BUF_SIZE, false);
    publishers[1] = &front_right_a;
    std_msgs::Float64 msg_buf_back_left_a[BUF_SIZE];
    ros_echronos::Publisher<std_msgs::Float64> back_left_a(topics[2], (std_msgs::Float64*)msg_buf_back_left_a, BUF_SIZE, false);
    publishers[2] = &back_left_a;
    std_msgs::Float64 msg_buf_back_right_a[BUF_SIZE];
    ros_echronos::Publisher<std_msgs::Float64> back_right_a(topics[3], (std_msgs::Float64*)msg_buf_back_right_a, BUF_SIZE, false);
    publishers[3] = &back_right_a;
    std_msgs::Float64 msg_buf_front_left_s[BUF_SIZE];
    ros_echronos::Publisher<std_msgs::Float64> front_left_s(topics[3], (std_msgs::Float64*)msg_buf_front_left_s, BUF_SIZE, false);
    publishers[4] = &front_left_s;
    std_msgs::Float64 msg_buf_front_right_s[BUF_SIZE];
    ros_echronos::Publisher<std_msgs::Float64> front_right_s(topics[4], (std_msgs::Float64*)msg_buf_front_right_s, BUF_SIZE, false);
    publishers[5] = &front_right_s;
    std_msgs::Float64 msg_buf_back_left_s[BUF_SIZE];
    ros_echronos::Publisher<std_msgs::Float64> back_left_s(topics[5], (std_msgs::Float64*)msg_buf_back_left_s, BUF_SIZE, false);
    publishers[6] = &back_left_s;
    std_msgs::Float64 msg_buf_back_right_s[BUF_SIZE];
    ros_echronos::Publisher<std_msgs::Float64> back_right_s(topics[6], (std_msgs::Float64*)msg_buf_back_right_s, BUF_SIZE, false);
    publishers[7] = &back_right_s;

    for (size_t i = 0; i < NUM_MSG; i++) {
        publishers[i]->init(nh);
    }

    while(true) {

        wait_for_msg();
        for (size_t i = 0; i < sizeof(struct message); i++) {
            serial.data.structBytes[i] = (uint8_t) UARTgetc();
            buf_reading.structBytes[i] = serial.data.structBytes[i];
        }
        rtos_mutex_lock(RTOS_MUTEX_ID_BUF);
        buf_ready = buf_reading;
        rtos_mutex_unlock(RTOS_MUTEX_ID_BUF);
        is_buffer_ready = true;
        rtos_signal_wait(RTOS_SIGNAL_ID_UART_RECEIVE_SIGNAL);
        UARTwrite((const char*)startMagic, 4);
    }
}

extern "C" void task_publish_buffer_fn(void) {
    ros_echronos::NodeHandle nh = *shared_nh;
    std_msgs::Float64 msg;

    while(true) {
        // wait for buffer to fill up
        while (!is_buffer_ready);
        rtos_mutex_lock(RTOS_MUTEX_ID_BUF);
        buf_sending = buf_ready;
        rtos_mutex_unlock(RTOS_MUTEX_ID_BUF);
        for (size_t i = 0; i < NUM_MSG; i++) {
            msg.data = buf_sending.msg.data[i];
            publishers[i]->publish(msg);
        }
        is_buffer_ready = false;
#ifdef UART_BUFFERED
        UARTFlushTx(false);
#endif
        nh.spin();
    }
}

int main(void) {

    // Initialize the floating-point unit.
    InitializeFPU();

    // Set the clocking to run from the PLL at 50 MHz.
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                       SYSCTL_XTAL_16MHZ);

    // Set up the systick interrupt used by the RTOS
    ROM_SysTickPeriodSet(ROM_SysCtlClockGet() / SYSTICKS_PER_SECOND);
    ROM_SysTickIntEnable();
    ROM_SysTickEnable();

    // Initialize the UART for stdio so we can use UARTPrintf
    InitializeUARTStdio();

    init_can();

    alloc::init_mm(RTOS_MUTEX_ID_ALLOC);

    // Actually start the RTOS
    rtos_start();

    /* Should never reach here, but if we do, an infinite loop is
       easier to debug than returning somewhere random. */
    for (;;) ;
}

void init_can(void) {
    // We enable GPIO E - E4 for RX and E5 for TX
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    GPIOPinConfigure(GPIO_PE4_CAN0RX);
    GPIOPinConfigure(GPIO_PE5_CAN0TX);

    // enables the can function we have just configured on those pins
    GPIOPinTypeCAN(GPIO_PORTE_BASE, GPIO_PIN_4 | GPIO_PIN_5);

    //enable and initalise CAN0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_CAN0);
    CANInit(CAN0_BASE);

    //TODO: change this to use the eChronos clock
    // Set the bitrate for the CAN BUS. It uses the system clock
    CANBitRateSet(CAN0_BASE, ROM_SysCtlClockGet(), CAN_BITRATE);

    // enable can interupts
    CANIntEnable(CAN0_BASE, CAN_INT_MASTER | CAN_INT_ERROR );
    IntEnable(INT_CAN0);

    //start CAN
    CANEnable(CAN0_BASE);

    // enable an object for reciving the latest messages
    // 0 for Msg ID and Filter means recieve all messages
    // for ROS I'm thinking we have a message id per topic
    rx_object.ui32MsgID = 0;
    rx_object.ui32MsgIDMask = 0;
    rx_object.ui32Flags = MSG_OBJ_TX_INT_ENABLE;
    rx_object.ui32MsgLen = CAN_MSG_LEN;
    rx_object.pui8MsgData = (uint8_t *) &can_input_buffer;
}

void write_can(uint32_t message_id, uint8_t *buffer, uint32_t buffer_size) {
    tCANMsgObject can_tx_message;
    can_tx_message.ui32MsgID = message_id;
    can_tx_message.ui32MsgIDMask = 0;
    can_tx_message.ui32Flags = MSG_OBJ_TX_INT_ENABLE;
    can_tx_message.ui32MsgLen = buffer_size;
    can_tx_message.pui8MsgData = buffer;

    CANMessageSet(CAN0_BASE, message_id, &can_tx_message, MSG_OBJ_TYPE_TX);
}
