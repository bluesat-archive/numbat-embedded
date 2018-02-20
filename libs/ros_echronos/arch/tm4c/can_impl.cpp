/**
 * @date: 22/09/17
 * @author: (original author) Harry J.E Day <harry@dayfamilyweb.com>
 * @authors: (Editors)
 * @details: Purpose: This provides the implementation of can for the tm4c
 * @copydetails: This code is released under dual LGPLv3 License and BSD license.
 * @copyright: Copyright BLUEsat UNSW, 2017
 */
#include "can_impl.hpp"
#include "boilerplate.h"
#include "ros.hpp"

#define NUM_CAN_OBJS 32
#define CAN_FIFO_QUEUE_LENGTH 10
// reserve 0 for sending messages
#define CAN_ID_START 1
#define CAN_DEVICE_BASE CAN0_BASE
#define CAN_RECEIVE_FLAGS (MSG_OBJ_RX_INT_ENABLE | MSG_OBJ_USE_ID_FILTER |  MSG_OBJ_EXTENDED_ID)

/**
 * The CAN msg objects we can use to store ids in
 */
static tCANMsgObject msgs[NUM_CAN_OBJS];
/**
 * Bump pointer for IDs
 */
static uint8_t current_id = CAN_ID_START;



using namespace ros_echronos::can;

void ros_echronos::can::send_can(CAN_ROS_Message & msg) {

    //wait for the buffer to be empty before starting
    UARTprintf("Waiting for CAN bus to have no pending sends\n");
    while(CANStatusGet(CAN_DEVICE_BASE, CAN_STS_TXREQUEST)) {
        //rtos_sleep(1);
    }

    tCANMsgObject can_tx_message;
    can_tx_message.ui32MsgID = msg.head.bits;
    can_tx_message.ui32MsgIDMask = 0;
    can_tx_message.ui32Flags =  MSG_OBJ_EXTENDED_ID;
    can_tx_message.ui32MsgLen = msg.body_bytes;
    can_tx_message.pui8MsgData = msg.body;

    CANMessageSet(CAN_DEVICE_BASE, 1, &can_tx_message, MSG_OBJ_TYPE_TX);
    UARTprintf("Sent\n");
}

can_sub_id ros_echronos::can::subscribe_can(uint32_t id_mask, uint32_t mask_bits) {
    can_sub_id id = current_id;
    for(int i = 0; i < CAN_FIFO_QUEUE_LENGTH; ++i) {
        msgs[current_id].ui32Flags = CAN_RECEIVE_FLAGS;
        if (i < CAN_FIFO_QUEUE_LENGTH-1) {
            msgs[current_id].ui32Flags |= MSG_OBJ_FIFO;
        }
        msgs[current_id].ui32MsgID = id_mask;
        msgs[current_id].ui32MsgIDMask = mask_bits;
        msgs[current_id].ui32MsgLen = CAN_MESSAGE_MAX_LEN; //TODO: check this allows shorter messages
        CANMessageSet(CAN_DEVICE_BASE, current_id, msgs + current_id, MSG_OBJ_TYPE_RX);
        ++current_id;
    }
    return id;
}

void ros_echronos::can::unsubscribe_can(can_sub_id id) {
    msgs[id].ui32MsgID = 0;
    msgs[id].ui32MsgIDMask = MSG_OBJ_NO_FLAGS;
    CANMessageClear(CAN_DEVICE_BASE, id);
    //TODO: make a less dodgey bump pointer
    // TODO: account for FIFO
    if(current_id == id) {
        current_id--;
    }
}

static int error_flag = 0;


extern "C" void ros_can_interupt_handler(void) {

    uint32_t can_status = 0;

    // read the register
    can_status = CANIntStatus(CAN_DEVICE_BASE, CAN_INT_STS_CAUSE);

    // in this case we are reciving a status interupt
    if(can_status == CAN_INT_INTID_STATUS) {
        // read the error status and store it to be handled latter
        error_flag = CANStatusGet(CAN_DEVICE_BASE, CAN_STS_CONTROL);
        //UARTprintf("CAN error %u\n", error_flag);
    } else if (can_status != 0 && can_status <32) {
        // we use slot 0 for sending
        tCANMsgObject msg;
        uint8_t data[CAN_MESSAGE_MAX_LEN];
        msg.pui8MsgData = data;
        msg.ui32MsgLen = CAN_MESSAGE_MAX_LEN;
        CANMessageGet(CAN_DEVICE_BASE, can_status, &msg, true);
        //begin the copy
        ros_echronos::can::input_buffer.start_counter++;
        ros_echronos::can::input_buffer.buffer.head.bits = msg.ui32MsgID;
        ros_echronos::can::input_buffer.buffer.body_bytes = msg.ui32MsgLen;
        memcpy(ros_echronos::can::input_buffer.buffer.body, msg.pui8MsgData, msg.ui32MsgLen);
        ros_echronos::can::input_buffer.end_counter++;
        if(ros_echronos::can::node_handle_ready) {
            //UARTprintf("nh ready %d\n", ros_echronos::can::can_interupt_event);
            // raise the event
            rtos_interrupt_event_raise(ros_echronos::can::can_interupt_event);
        }

    } else {
        UARTprintf("No Interupt!\n");
    }

}
