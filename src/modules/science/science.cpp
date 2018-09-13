/**
 * Date Started: 8/9/18
 * @author: (Original Author) Alan Nguyen, Edward Dai
 * @description: Science module, see readme for usage
 * @copyright: This code is released under the MIT [GPL for embeded] License. Copyright BLUEsat UNSW, 2018
 */

#include <rtos-kochab.h>
#include "boilerplate.h"
#include "ros.hpp"
#include "Subscriber.hpp"
#include "Publisher.hpp"
#include "NodeHandle.hpp"
#include "owr_messages/science.hpp"
#include "std_msgs/Int16.hpp"
#include "science-mod/LIS3MDL.h"
#include "science-mod/SI7021.h"
#include "science-mod/TCS34725.h"
#include "science-mod/HX711.h"
#include "science-mod/TCA9548A.h"
#include "adc.h"
#include "servo.h"

#define NUM_MODULES 4
#define SCIENCE_SERVO_PIN PWM0

#define NEUTRAL_POS  0
#define MODULE_1_POS 52.5
#define MODULE_2_POS 126.4 // (should be 135) - will need to recalibrate if mech issues are fixed
#define MODULE_3_POS -132.5 // (should be -135)
#define MODULE_4_POS -52.5

ros_echronos::NodeHandle * volatile nh_ptr = NULL;

#define SYSTICKS_PER_SECOND     100
#define CAN_BITRATE 500000
#define CAN_MSG_LEN 8


static tCANMsgObject rx_object;
static uint8_t can_input_buffer[CAN_MSG_LEN];
static void init_can(void);
static void write_can(uint32_t message_id, uint8_t *buffer, uint32_t buffer_size);
void data_request_callback(const std_msgs::Int16 &msg);
void servo_position_callback(const std_msgs::Int16 &msg);
void tare_callback(const std_msgs::Int16 &msg);

extern "C" bool tick_irq(void) {
    rtos_timer_tick();
    return true;
}

bool sent_message;

static uint32_t error_flag;

static ros_echronos::Publisher<owr_messages::science> *science_pub_ptr;
static TCS34725 *tcs34725_ptr;
static LIS3MDL *lis3mdl_ptr;
static SI7021 *si7021_ptr;
static HX711 *hx711_ptr;
uint32_t moisture_buffer[NUM_MODULES] = {0};


extern "C" void task_science_fn(void) {
    // this creates a node handle
    ros_echronos::ROS_INFO("Entered CAN task. Initializing...\n");
    ros_echronos::NodeHandle nh;
    nh.init("science_fn", "science_fn", RTOS_INTERRUPT_EVENT_ID_CAN_RECEIVE_EVENT, RTOS_SIGNAL_ID_CAN_RECEIVE_SIGNAL);
    nh_ptr = &nh;

    // Create the subscriber
    ros_echronos::ROS_INFO("Data request sub init\n");
    std_msgs::Int16 request_buffer_in[5];
    ros_echronos::Subscriber<std_msgs::Int16> request_sub("science/request", request_buffer_in, 5, data_request_callback);
    request_sub.set_topic_id(0);
    request_sub.init(nh);

    ros_echronos::ROS_INFO("Servo position sub init\n");
    std_msgs::Int16 servo_buffer_in[5];
    ros_echronos::Subscriber<std_msgs::Int16> servo_pos_sub("science/servo", servo_buffer_in, 5, servo_position_callback);
    servo_pos_sub.set_topic_id(1);
    servo_pos_sub.init(nh);
    
    std_msgs::Int16 tare_buffer_in[5];
    ros_echronos::Subscriber<std_msgs::Int16> weight_tare_sub("science/tare", tare_buffer_in, 5, tare_callback);
    weight_tare_sub.set_topic_id(2);
    weight_tare_sub.init(nh);

    // Create the publisher
    ros_echronos::ROS_INFO("Data pub init\n");
    owr_messages::science science_buffer_out[5];
    ros_echronos::Publisher<owr_messages::science> science_pub("science/data", science_buffer_out, 5, false);
    science_pub_ptr = &science_pub;
    science_pub_ptr->init(nh);

    ros_echronos::ROS_INFO("Science servo init\n");
    servo_init(SCIENCE_SERVO, SCIENCE_SERVO_PIN);
    servo_write(SCIENCE_SERVO, SCIENCE_SERVO_PIN, NEUTRAL_POS);

    ros_echronos::ROS_INFO("ADC init\n");
    enum adc_pin moisture_pins[NUM_MODULES] = {AIN0, AIN1, AIN2, AIN3};
    adc_init_pins(moisture_pins, NUM_MODULES, false);

    TCS34725 tcs34725(I2C0);
    tcs34725_ptr = &tcs34725;
    LIS3MDL lis3mdl(I2C0);
    lis3mdl_ptr = &lis3mdl;
    SI7021 si7021(I2C0);
    si7021_ptr = &si7021;
    ros_echronos::ROS_INFO("Init sensors of each module\n");
    bool success = false;
    i2c_init(I2C0, FAST);
    // initialise sensors of each module
    for (uint8_t i = 0; i < NUM_MODULES; i++) {
        ros_echronos::ROS_INFO("Module %d\n", i);
        i2c_select(I2C0, i); // select multiplexer output
        success = tcs34725_ptr->init();
        if (success) {
            ros_echronos::ROS_INFO("TCS34725 successfully initialised\n");
        } else {
            ros_echronos::ROS_INFO("TCS34725 failed to initialise\n");
        }

        success = lis3mdl_ptr->init();
        if (success) {
            ros_echronos::ROS_INFO("LIS3MDL successfully initialised\n");
        } else {
            ros_echronos::ROS_INFO("LIS3MDL failed to initialise\n");
        }

        success = si7021_ptr->init();
        if (success) {
            ros_echronos::ROS_INFO("SI7021 successfully initialised\n");
        } else {
            ros_echronos::ROS_INFO("SI7021 failed to initialise\n");
        }
    }

    HX711 hx711(PORTA, PIN_7, PORTA, PIN_6);
    hx711_ptr = &hx711;
    hx711_ptr->init();
    float scale = -67.29; // CALIBRATE THIS
    hx711_ptr->set_scale(scale);
    ros_echronos::ROS_INFO("HX711 weight sensor initialised\n");

    while(true) {
        // this causes the callbacks to be called
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

    ros_echronos::write_mutex = RTOS_MUTEX_ID_PRINT;
    ros_echronos::write_mutex_set = true;

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
    CANIntEnable(CAN0_BASE, CAN_INT_MASTER | CAN_INT_ERROR); //| CAN_INT_STATUS);
    IntEnable(INT_CAN0);

    //start CAN
    CANEnable(CAN0_BASE);

}

/*
 * Reads measurements specified module and publishes the data
 *
 * Module number should be an integer between 1-4 (zero not included to be consistent
 * with the servo position module number)
 */
void data_request_callback(const std_msgs::Int16 &msg) {
    if (msg.data <= 0) {
        UARTprintf("Message value should be between 1-4\n");
        return;
    }
    // switch multiplexer output to the desired module
    i2c_select(I2C0, msg.data-1); // subtract 1 since multiplexer output starts from 0
    // create a message containing sensor data
    owr_messages::science data_msg;
    // read temperature and humidity
    data_msg.temperature = si7021_ptr->read_temperature();
    data_msg.humidity = si7021_ptr->read_humidity();
    // magnetism
    lis3mdl_ptr->read_magnetism(&data_msg.mag_x, &data_msg.mag_y, &data_msg.mag_z);
    // colour sensor
    uint16_t r, g, b, c;
    tcs34725_ptr->read_raw_data(&r, &g, &b, &c);
    data_msg.colour_temperature = (uint32_t) tcs34725_ptr->calculate_colour_temperature(r, g, b);
    data_msg.illuminance = (uint32_t) tcs34725_ptr->calculate_lux(r, g, b);
    // moisture
    adc_capture_polling(moisture_buffer);
    // get the moisture value corresponding to the desired module
    data_msg.moisture = moisture_buffer[msg.data-1]; // index starts from 0
    // weight (averaging 30 samples)
    data_msg.weight = hx711_ptr->read_scaled_avg(30);

    // publish data message
    science_pub_ptr->publish(data_msg, 0);
}

/*
 * See hash defines for exact angle positions of each module
 */
void servo_position_callback(const std_msgs::Int16 &msg) {
    UARTprintf("Change servo position to %d\n", msg.data);
    switch (msg.data) {
        case 0:
            servo_write(SCIENCE_SERVO, SCIENCE_SERVO_PIN, NEUTRAL_POS);
            break;
        case 1:
            servo_write(SCIENCE_SERVO, SCIENCE_SERVO_PIN, MODULE_1_POS);
            break;
        case 2:
            servo_write(SCIENCE_SERVO, SCIENCE_SERVO_PIN, MODULE_2_POS);
            break;
        case 3:
            servo_write(SCIENCE_SERVO, SCIENCE_SERVO_PIN, MODULE_3_POS);
            break;
        case 4:
            servo_write(SCIENCE_SERVO, SCIENCE_SERVO_PIN, MODULE_4_POS);
            break;
    }
}

/* Recalibrate zero reference (tare) for weight sensor */
void tare_callback(const std_msgs::Int16 &msg) {
    UARTprintf("Received command to recalibrate zero weight reference\n");
    hx711_ptr->tare(50);
}
