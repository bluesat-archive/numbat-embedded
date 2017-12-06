#include "boilerplate.h"
#include "rtos-kochab.h"
#include "ros.hpp"
#include "owr_messages/pwm.hpp"
#include "Publisher.hpp"

owr_messages::pwm pwm_buffer[5];
ros_echronos::Publisher<owr_messages::pwm> * pub;

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

bool sent_message;

static uint32_t error_flag;

/**
 * Used to handle interups from can0
 */
extern "C" void can0_int_handler(void) {
    uint32_t can_status = 0;

    // read the register
    can_status = CANIntStatus(CAN0_BASE, CAN_INT_STS_CAUSE);

    // in this case we are reciving a status interupt
    if(can_status == CAN_INT_INTID_STATUS) {
        // read the error status and store it to be handled latter
        error_flag = CANStatusGet(CAN0_BASE, CAN_STS_CONTROL);
        // clear so we can continue
        CANIntClear(CAN0_BASE, 1);
    } else {
        // we are reciving a message, TODO: handle this
        // for now we clear the interup so we can continue
        CANIntClear(CAN0_BASE, 1);

        // clear the error flag (otherwise we will store recive or write statuses)
        error_flag = 0;

        // if we haven't just sent a message read.
        if(!sent_message) {
            CANMessageGet(CAN0_BASE, can_status, &rx_object, 0);
            UARTprintf("Received: %c\n", can_input_buffer);
        }
        sent_message = false;
    }

    //UARTprintf("A Error Code %x\n", can_status);
}

extern "C" void task_ros_test_fn(void) {

    UARTprintf("Entered CAN task. Initializing...\n");
    ros_echronos::NodeHandle nh;
    UARTprintf("Done init\n");
    nh.init("ros_test_fn", "ros_test_fn", nullptr);

    UARTprintf("pub init\n");
    pub->init(nh);
    owr_messages::pwm msg;
    while(true) {
        UARTprintf("Next loop!\n");
        msg.currentPos+=1.5;
        msg.targetVel = 2;
        msg.pwm = 100;
        msg.targetPos+=2;
        memcpy(msg.joint, "the joint", 9);
        UARTprintf("pub pub\n");
        pub->publish(msg);
        UARTprintf("pub done\n");

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
    ros_echronos::Publisher<owr_messages::pwm> _pub("aaa", (owr_messages::pwm*)pwm_buffer, 5, false);
    pub = &_pub;

    init_can();

    alloc::init_mm(RTOS_MUTEX_ID_ALLOC);

    // Actually start the RTOS
    UARTprintf("Starting RTOS...\n");
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
    CANIntEnable(CAN0_BASE, CAN_INT_MASTER | CAN_INT_ERROR | CAN_INT_STATUS);
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
