/*
 * @date: 26/12/17
 * @author: (original author) Harry J.E Day <harry@dayfamilyweb.com>
 * @authors: (editors) 
 * @details: 
 * @copydetails: This code is released under the LGPLv3 and newer License and the BSD License
 * @copyright: Copyright BLUEsat UNSW 2017
 */
#include <type_traits>
#include "ros.hpp"
#include "can.hpp"
#ifndef NUMBAT_EMBEDDED_MESSAGE_DESCRIPTOR_HPP
#define NUMBAT_EMBEDDED_MESSAGE_DESCRIPTOR_HPP

namespace ros_echronos {

    /**
     * Class used to write encoded data into a message class.
     * Note: a descriptor can only be used for one decoding
     */
    class Message_Descriptor {
        public:

            /**
             * Decode the given message into the message that created this descriptor
             * @param msg the msg to decode
             * @note: the message class this class references may be in an inconsistent state
             *  until all messages are decoded
             */
            void decode_msg(const can::CAN_ROS_Message &msg);
        //protected:
            /**
             * Creates a new Message Descriptor with references to the fields
             * @param field_ptrs pointers to each of the fields in the message
             * @param field_size the size of each field, 0 represents a variable length field
             * @param num_fields the number of fields in the message (and thus the number of elements in each array)
             */
            Message_Descriptor(void ** field_ptrs, size_t * field_size, const size_t num_fields);

            /**
             * Destroys the descriptor
             */
            ~Message_Descriptor();

            /**
             * Copy constructor for Message_Descriptor
             * @param to_copy the descriptor to copy
             */
            Message_Descriptor(const Message_Descriptor & to_copy);


        protected:

            /**
             * Creates a new Message Descriptor with references to the fields, has the option to just use
             * pointers rather than copy them. This is designed for use with Message_Descriptor_Fixed to reduce mallocs.
             * @param field_ptrs pointers to each of the fields in the message
             * @param field_size the size of each field, 0 represents a variable length field
             * @param num_fields the number of fields in the message (and thus the number of elements in each array)
             * @param copy if the fields should be copied or just use the existing pointers
             */
            Message_Descriptor(void **field_ptrs, size_t *field_size, Message_Descriptor **sub_descriptor_ptrs,
                               size_t num_fields, const bool copy);
            void ** field_ptrs = NULL;
            Message_Descriptor ** sub_descriptors = NULL;
            size_t * field_size = NULL;
            const size_t num_fields;
            /**
             * The field we are currently on
             */
            uint16_t field_offset = 0;
            /**
             * If we have an offset inside the field
             */
            size_t field_internal_offset = 0;
            /**
             * If we have to split halfway through a length
             */
            bool decoding_len = false;
        private:
            /**
             * If we need to memory manage our arrays
             */
            const bool mem_manage_arrays;

        // allow Messages to use our protected methofs
        //friend class Message;

    };

    class Tuple {
    public:
        Tuple();

        void get_children(int size, Message_Descriptor ** ptrs) {

        }
    };


//    template <typename  T, typename ... Ts> class Tuple_With_Data_1 : Tuple {
//
//        T value;
//
//    public:
//        Tuple_With_Data_1(T value) : value(value) {
//
//        }
//
//        Tuple_With_Data_1();
//
//
//        virtual void get_children(const size_t size, Message_Descriptor ** ptrs) {
//            ptrs[0] = &value;
//        }
//
//    };


    template <typename  T, typename Ts > class Tuple_With_Data {

         T value;

    protected:
         Ts data;

    public:
        Tuple_With_Data(T value, Ts next) : value(value),  data(next) {

        }

        Tuple_With_Data();


        void get_children(const size_t size, Message_Descriptor ** ptrs) {
            ptrs[0] = &value;
            data.get_children(size-1, ptrs+1);
        }

    };

    template <typename Ts>
    class Tuple_Null {

    private:
        Ts data;
    public:


        Tuple_Null(Ts next) :  data(next) {

        }

        Tuple_Null();

        void get_children(int size, Message_Descriptor ** ptrs) {
            ptrs[0] = NULL;
            data.get_children(size-1, ptrs+1);
        }
    };



    /**
     * Template used to avoid doing extra allocations for the fields. Since a message descriptor is initalised at
     * compile time this can be done
     * @tparam FIELDS the number of fields in this descriptor
     */
    template <const size_t FIELDS, typename SUB_DESC_TREE>
    class Message_Descriptor_Fixed : public Message_Descriptor {

    public:
        Message_Descriptor_Fixed();
        void * fixed_field_ptrs[FIELDS];
        size_t fixed_field_sizes[FIELDS];
        SUB_DESC_TREE desc_tree;
        Message_Descriptor * fixed_sub_descriptor_ptrs[FIELDS];

    private:
    };

    template <const size_t FIELDS, typename SUB_DESC_TREE>
    inline Message_Descriptor_Fixed<FIELDS, SUB_DESC_TREE>::Message_Descriptor_Fixed() :
        Message_Descriptor(fixed_field_ptrs, fixed_field_sizes, fixed_sub_descriptor_ptrs, FIELDS, false) {
            desc_tree.get_children(FIELDS, fixed_sub_descriptor_ptrs);
    }

    inline Tuple::Tuple() {}

    template <typename Ts>
    inline Tuple_Null<Ts>::Tuple_Null() {}
    template <typename  T, typename Ts>
    inline Tuple_With_Data<T, Ts>::Tuple_With_Data() {}


}

#endif //NUMBAT_EMBEDDED_MESSAGE_DESCRIPTOR_HPP
