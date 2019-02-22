/**
 * @date: 04/05/17
 * @author: (original author) Harry J.E Day <harry@dayfamilyweb.com>
 * @authors: (Editors)
 * @details: Implements a class for managing subscribing to ros messages
 * @copydetails: This code is released under the AGPLv3 License.
 * @copyright: Copyright BLUEsat UNSW, 2017
 */

#ifndef NUMBAT_EMBEDDED_SUBSCRIBER_HPP
#define NUMBAT_EMBEDDED_SUBSCRIBER_HPP

#include "ros.hpp"
#include "can_impl.hpp"
#include "ListNode.hpp"
#include "Message_Buffer.hpp"

namespace ros_echronos {
    /**
     * None generic functions for the Subscriber class
     */
    class _Subscriber : public ListNode {
        public:
            /**
             * the topic id of the subscription
             */
            uint8_t topic_id : 6;

            /**
             * Handles receiving a message for a given topic
             * @param msg the message to receive
             */
            virtual void receive_message(ros_echronos::can::CAN_ROS_Message & msg) = 0;

            /**
             * Calls the callback for all waiting complete messages in the buffer
             */
            virtual void call_callback() = 0;

    };

    /**
     * Mirrors ros::Subscriber
     * Responsible for handeling recieving messages on a specific topic
     * @tparam T the message type to subscribe to
     */
    template <class T> class Subscriber : public _Subscriber {

        public:
            /**
             * Creates a new Subscriber instance
             * Does not register it with the node
             *
             * @param topic_name the name of the topic to publish on as a null terminated string.
             *        the topic name *must* start with `/`
             * @param read_buffer the buffer used to store incoming messages. The caller should not use this pointer
             *  once it is passed. This buffer will be split and half used for message construction and half used for
         *      messages awaiting a callback
             * @param buffer_size the size of incoming buffer
             * @param callback the callback to be called when a message is recieved
             */
            Subscriber(char * topic_name, T * const read_buffer, int buffer_size, void (*callback)(const T &));

            /**
             * Destroys the subscriber, disconnects from the controller if it has been connected
             */
            ~Subscriber();

            /**
             * Called to enable the subscriber
             * Does all the setup that needs to be done after node initalisation and registers with the controller
             * @param node_handle the node handle to use
             * @precondition the Node Handle has been initalised and is in communication with the controller
             */
            void init(ros_echronos::NodeHandle &node_handle, RtosSignalId ctrl_wait_sig);

            /**
             * unsubscribes the subscriber
             */
            void unsubscribe();
            /**
             * Function called when a can message is received
             * @param msg the msg that has been recieved
             */
            virtual void receive_message(ros_echronos::can::CAN_ROS_Message & msg);
            virtual void call_callback();
            // TODO: remove this
            /**
             * Used to set the topic id in the absence of the control commands
             * @param id the id of the topic
             */
            void set_topic_id(int id);

        private:
            ros_echronos::can::can_sub_id sub_id;
            /**
             * Buffer of incoming messages
             */
            T * const incoming_msgs;
            /**
             * Bitmask of free buffer slots in incoming messages
             */
            uint32_t mask = 0;
            /**
             * Max size of buffer
             */
            const size_t message_construction_buff_size;
            /**
             * Buffer of ready messages
             */
            Message_Buffer<T> ready_msgs;
            /**
             * The subscribers node handle
             */
            NodeHandle * nh = NULL;

            /**
             * The callback
             */
            void (*callback)(const T &);

            inline T * next_construction_msg();
            inline void clear_slot(T * msg_ptr);

            /**
             * Internal function to register a node
             * @param signal_wait the signal to wait on
             */
            void register_topic(const RtosSignalId signal_wait);
            /**
             * which error handling mode we are in
             */
            Transmission_Mode mode = DROP_MISSING;

            /**
             * Empty message for fast clearing of slots
             */
            const T EMPTY_MSG;

            /**
             * The topic name
             */
            char *  const  topic_name;

    };
}


#endif //NUMBAT_EMBEDDED_SUBSCRIBER_HPP
