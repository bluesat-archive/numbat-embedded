/**
 * @date: 04/05/17
 * @author: (original author) Harry J.E Day <harry@dayfamilyweb.com>
 * @authors: (Editors)
 * @details: Implements a class for managing publishing ros messages
 * @copydetails: This code is released under the AGPLv3 License.
 * @copyright: Copyright BLUEsat UNSW, 2017
 */

#ifndef NUMBAT_EMBEDDED_PUBLISHER_HPP
#define NUMBAT_EMBEDDED_PUBLISHER_HPP

#include "ros.hpp"

/*
 * ROS Echronos Name space
 */
namespace ros_echronos {
    /**
     * Mirrors ros::Publisher
     * Responsible for handeling the publishing of messages
     * @tparam T the message type to publish
     */
    template <class T> class Publisher {

        public:
            /**
             * Creates a new Publisher instance
             * Does not register it with the node
             *
             * @param topic_name the name of the topic to publish on as a null terminated string
             * @param write_buffer the buffer used to store outgoing messages. The caller should not use this pointer
             *  once it is passed.
             * @param buffer_size the size of write_buffer
             * @param latch if messages should be resent by the controller when knew nodes connect
             */
            Publisher(char * topic_name, T * const write_buffer, int buffer_size, bool latch);

            /**
             * Destroys the publisher, disconnects from the controller if it has been connected
             */
            ~Publisher();

            /**
             * Called to enable the publisher
             * Does all the setup that needs to be done after node initalisation and registers with the controller
             * @param node_handle the node handle to use
             * @precondition the Node Handle has been initalised and is in communication with the controller
             */
            void init(ros_echronos::NodeHandle & node_handle);

            /**
             * Publish a message on the topic provided
             * @param message the message to publish
             * @param priority the priority of the message
             */
            void publish(T message, uint8_t priority = 0);

    };
}


#endif //NUMBAT_EMBEDDED_PUBLISHER_HPP
