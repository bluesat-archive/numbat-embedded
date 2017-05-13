/**
 * @date: 04/05/17
 * @author: (original author) Harry J.E Day <harry@dayfamilyweb.com>
 * @authors: (Editors)
 * @details: Implements a class for managing subscribing to ros messages
 * @copydetails: This code is released under the AGPLv3 License.
 * @copyright: Copyright BLUEsat UNSW, 2017
 */

#ifndef NUMBAT_EMBEDDED_PUBLISHER_HPP
#define NUMBAT_EMBEDDED_PUBLISHER_HPP

#include "ros.hpp"

namespace ros_echronos {
    /**
     * Mirrors ros::Subscriber
     * Responsible for handeling recieving messages on a specific topic
     * @tparam T the message type to subscribe to
     */
    template <class T> class Subscriber {

        public:
            /**
             * Creates a new Subscriber instance
             * Does not register it with the node
             *
             * @param topic_name the name of the topic to publish on as a null terminated string
             * @param read_buffer the buffer used to store outgoing messages. The caller should not use this pointer
             *  once it is passed.
             * @param buffer_size the size of reade_buffer
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
            void init(ros_echronos::NodeHandle & node_handle);

            /**
             * unsubscribes the subscriber
             */
            void unsubscribe();

    };
};


#endif //NUMBAT_EMBEDDED_PUBLISHER_HPP
