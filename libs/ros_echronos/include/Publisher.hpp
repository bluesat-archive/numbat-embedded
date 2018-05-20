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
#include "Message_Buffer.hpp"
#include "ListNode.hpp"

/*
 * ROS Echronos Name space
 */
namespace ros_echronos {

    class _Publisher : public ListNode {
        public:
            /**
             * Retrieves the next can message from the publisher
             *
             * IMPORTANT: An individual ROS Message can contain many can messages
             *
             * @param has_next if there are more messages in the buffer
             * @param empty if the returned message is empty
             *
             * @return the next can message
             */
            virtual ros_echronos::can::CAN_ROS_Message get_next_message(bool & has_next, bool &empty) = 0;

        protected:
            /**
             * Stores the topic "number" on the can bus
             */
            uint8_t topic_id;
    };

    /**
     * Mirrors ros::Publisher
     * Responsible for handeling the publishing of messages
     * @tparam T the message type to publish
     */
    template <class T> class Publisher : public _Publisher {

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
            Publisher(char * topic_name, T * const write_buffer, uint8_t buffer_size, bool latch) __attribute__((used));

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
            void publish(T & message, uint8_t priority = 0);

            virtual ros_echronos::can::CAN_ROS_Message get_next_message(bool &has_next, bool &empty)  __attribute__((used));



        private:
            /**
             * Null terminated topic name
             */
            char * topic_name;
            /**
             * Buffer of messages
             */
            Message_Buffer<T> buffer;
            /**
             * The node handle this publisher is attached to
             */
            NodeHandle * nh;
            /**
             * Stores the header for this topic
             */
             can::CAN_Header header;

            /**
             * the message currently being published
             */
            T current_message;

            /**
             * If there is a message in the process of being serialised
             */
            bool message_in_progress = false;

            /**
             * The sequence number for the current encoding
             */
            uint16_t seq_num = 0;
            /**
             * The number of the current message, note that this is cyclical and inteded to overflow
             */
            uint8_t msg_num : 2; //NOTE: it doesn't actually matter what this is initalised to
            /**
             * which error handling mode we are in
             */
            Transmission_Mode mode = DROP_MISSING;

    };
}


#endif //NUMBAT_EMBEDDED_PUBLISHER_HPP
