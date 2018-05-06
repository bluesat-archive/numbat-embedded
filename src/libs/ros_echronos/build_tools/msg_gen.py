#!/usr/bin/env python
"""
Modified version of ros's message generation tool to be used for generating messages compatible with our
bin system.

The code is dual licensed under the BSD License and AGPLv3 license.
Copyright of code written after 13/05/2017 is BLUEsat UNSW, 2017.


This program was based on msg_gen.py from the ros project U{https://github.com/ros/ros_comm/}
The original source code was released under the BSD License below. This license is compatible with
the AGPLv3 License used in this project. See: https://www.gnu.org/licenses/license-compatibility.en.html

  Software License Agreement (BSD License)

  Copyright (c) 2009, Willow Garage, Inc.
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:

   * Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   * Redistributions in binary form must reproduce the above
     copyright notice, this list of conditions and the following
     disclaimer in the documentation and/or other materials provided
     with the distribution.
   * Neither the name of Willow Garage, Inc. nor the names of its
     contributors may be used to endorse or promote products derived
     from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.
"""

## ROS message source code generation for C++
## 
## Converts ROS .msg files in a package into C++ source code implementations.

import sys
import os
import traceback

import roslib.msgs 
import roslib.packages
import roslib.gentools
from rospkg import RosPack

try:
    from cStringIO import StringIO #Python 2.x
except ImportError:
    from io import StringIO #Python 3.x

MSG_TYPE_TO_CPP = {'byte': 'int8_t', 'char': 'uint8_t',
                   'bool': 'uint8_t',
                   'uint8': 'uint8_t', 'int8': 'int8_t', 
                   'uint16': 'uint16_t', 'int16': 'int16_t', 
                   'uint32': 'uint32_t', 'int32': 'int32_t',
                   'uint64': 'uint64_t', 'int64': 'int64_t',
                   'float32': 'float',
                   'float64': 'double',
                   'string': 'ros_echronos::String',
                   'time': 'ros::Time',
                   'duration': 'ros::Duration'}

def msg_type_to_cpp(type):
    """
    Converts a message type (e.g. uint32, std_msgs/String, etc.) into the C++ declaration
    for that type (e.g. uint32_t, std_msgs::String_<ContainerAllocator>)
    
    @param type: The message type
    @type type: str
    @return: The C++ declaration
    @rtype: str
    """
    (base_type, is_array, array_len) = roslib.msgs.parse_type(type)
    cpp_type = None
    if (roslib.msgs.is_builtin(base_type)):
        cpp_type = MSG_TYPE_TO_CPP[base_type]
    elif (len(base_type.split('/')) == 1):
        if (roslib.msgs.is_header_type(base_type)):
            cpp_type = ' ::std_msgs::Header_'
        else:
            cpp_type = '%s_'%(base_type)
    else:
        pkg = base_type.split('/')[0]
        msg = base_type.split('/')[1]
        cpp_type = ' ::%s::%s_'%(pkg, msg)
        
    if (is_array):
        if (array_len is None):
            return 'ros_echronos::Array<%s> '%(cpp_type)
        else:
            return 'ros_echronos::Array<%s> '%(cpp_type)
    else:
        return cpp_type
    
def cpp_message_declarations(name_prefix, msg):
    """
    Returns the different possible C++ declarations for a message given the message itself.
    
    @param name_prefix: The C++ prefix to be prepended to the name, e.g. "std_msgs::"
    @type name_prefix: str
    @param msg: The message type
    @type msg: str
    @return: A tuple of 3 different names.  cpp_message_decelarations("std_msgs::", "String") returns the tuple
        ("std_msgs::String_", "std_msgs::String_<ContainerAllocator>", "std_msgs::String")
    @rtype: str 
    """
    pkg, basetype = roslib.names.package_resource_name(msg)
    cpp_name = ' ::%s%s'%(name_prefix, msg)
    if (pkg):
        cpp_name = ' ::%s::%s'%(pkg, basetype)
    return ('%s_'%(cpp_name), '%s_ '%(cpp_name), '%s'%(cpp_name))

def write_begin(s, spec, file):
    """
    Writes the beginning of the header file: a comment saying it's auto-generated and the include guards
    
    @param s: The stream to write to
    @type s: stream
    @param spec: The spec
    @type spec: roslib.msgs.MsgSpec
    @param file: The file this message is being generated for
    @type file: str
    """
    s.write("/* Auto-generated by genmsg_cpp for file %s */\n"%(file))
    s.write('#ifndef %s_MESSAGE_%s_H\n'%(spec.package.upper(), spec.short_name.upper()))
    s.write('#define %s_MESSAGE_%s_H\n'%(spec.package.upper(), spec.short_name.upper()))
    
def write_end(s, spec):
    """
    Writes the end of the header file: the ending of the include guards
    
    @param s: The stream to write to
    @type s: stream
    @param spec: The spec
    @type spec: roslib.msgs.MsgSpec
    """
    s.write('#endif // %s_MESSAGE_%s_H\n'%(spec.package.upper(), spec.short_name.upper()))
    
def write_generic_includes(s):
    """
    Writes the includes that all messages need
    
    @param s: The stream to write to
    @type s: stream
    """
    s.write('#include "ros.hpp"\n')
    s.write('#include "Message.hpp"\n')
    s.write('#include <new>\n')
    #s.write('#include "ros/serialization.h"\n')
    #s.write('#include "ros/builtin_message_traits.h"\n')
    #s.write('#include "ros/message_operations.h"\n')
    #s.write('#include "ros/time.h"\n\n')
    #s.write('#include "ros/macros.h"\n\n')
    #s.write('#include "ros/assert.h"\n\n')
    
def write_includes(s, spec):
    """
    Writes the message-specific includes
    
    @param s: The stream to write to
    @type s: stream
    @param spec: The message spec to iterate over
    @type spec: roslib.msgs.MsgSpec
    """
    for field in spec.parsed_fields():
        if (not field.is_builtin):
            if (field.is_header):
                s.write('#include "std_msgs/Header.h"\n')
            else:
                (pkg, name) = roslib.names.package_resource_name(field.base_type)
                pkg = pkg or spec.package # convert '' to package
                s.write('#include "%s/%s.h"\n'%(pkg, name))
                
    s.write('\n') 

def write_struct(s, spec, cpp_name_prefix, extra_deprecated_traits = {}):
    """
    Writes the entire message struct: declaration, constructors, members, constants and (deprecated) member functions
    @param s: The stream to write to
    @type s: stream
    @param spec: The message spec
    @type spec: roslib.msgs.MsgSpec
    @param cpp_name_prefix: The C++ prefix to use when referring to the message, e.g. "std_msgs::"
    @type cpp_name_prefix: str
    """
    
    msg = spec.short_name
    s.write('class %s_ : public ros_echronos::Message {\n'%(msg))
    s.write('  typedef %s_ Type;\n\n'%(msg))
    s.write('  public:\n')

    declare_constructors(s, spec, cpp_name_prefix)
    declare_deconstructor(s, spec, cpp_name_prefix)
    declare_virtual_functions(s, spec, cpp_name_prefix)
    write_members(s, spec)

    #rospack = RosPack()
    #gendeps_dict = roslib.gentools.get_dependencies(spec, spec.package, compute_files=False, rospack=rospack)
    #md5sum = roslib.gentools.compute_md5(gendeps_dict, rospack=rospack)
    #full_text = compute_full_text_escaped(gendeps_dict)
    
    # write_deprecated_member_functions(s, spec, dict(list({'MD5Sum': md5sum, 'DataType': '%s/%s'%(spec.package, spec.short_name), 'MessageDefinition': full_text}.items()) + list(extra_deprecated_traits.items())))
    
    (cpp_msg_unqualified, cpp_msg_with_alloc, cpp_msg_base) = cpp_message_declarations(cpp_name_prefix, msg)
    s.write('  typedef %s * Ptr;\n'%(cpp_msg_with_alloc))
    s.write('  typedef %s * const ConstPtr;\n'%(cpp_msg_with_alloc))

    s.write('}; // class %s\n'%(msg))
    
    s.write('typedef %s_ %s;\n\n'%(cpp_msg_base, msg))
    s.write('typedef %s %sPtr;\n'%(cpp_msg_base, msg))
    s.write('typedef %s const %sConstPtr;\n\n'%(cpp_msg_base, msg))


def default_value(type):
    """
    Returns the value to initialize a message member with.  0 for integer types, 0.0 for floating point, false for bool,
    empty string for everything else
    
    @param type: The type
    @type type: str
    """
    if type in ['byte', 'int8', 'int16', 'int32', 'int64',
                'char', 'uint8', 'uint16', 'uint32', 'uint64']:
        return '0'
    elif type in ['float32', 'float64']:
        return '0.0'
    elif type == 'bool':
        return 'false'
        
    return ""

def takes_allocator(type):
    """
    Returns whether or not a type can take an allocator in its constructor.  False for all builtin types except string.
    True for all others.
    
    @param type: The type
    @type: str
    """
    return not type in ['byte', 'int8', 'int16', 'int32', 'int64',
                        'char', 'uint8', 'uint16', 'uint32', 'uint64',
                        'float32', 'float64', 'bool', 'time', 'duration']

def write_initializer_list(s, spec, container_gets_allocator):
    """
    Writes the initializer list for a constructor
    
    @param s: The stream to write to
    @type s: stream
    @param spec: The message spec
    @type spec: roslib.msgs.MsgSpec
    @param container_gets_allocator: Whether or not a container type (whether it's another message, a vector, array or string)
        should have the allocator passed to its constructor.  Assumes the allocator is named _alloc.
    @type container_gets_allocator: bool
    """
    
    i = 0
    for field in spec.parsed_fields():
        if (i == 0):
            s.write('  : ')
        else:
            s.write('  , ')
            
        val = default_value(field.base_type)
        use_alloc = takes_allocator(field.base_type)
        if (field.is_array):
            if (field.array_len is None and container_gets_allocator):
                s.write('%s(_alloc)\n'%(field.name))
            else:
                s.write('%s()\n'%(field.name))
        else:
            if (container_gets_allocator and use_alloc):
                s.write('%s(_alloc)\n'%(field.name))
            else:
                s.write('%s(%s)\n'%(field.name, val))
        i = i + 1
        
def write_fixed_length_assigns(s, spec, container_gets_allocator, cpp_name_prefix):
    """
    Initialize any fixed-length arrays
    
    @param s: The stream to write to
    @type s: stream
    @param spec: The message spec
    @type spec: roslib.msgs.MsgSpec
    @param container_gets_allocator: Whether or not a container type (whether it's another message, a vector, array or string)
        should have the allocator passed to its constructor.  Assumes the allocator is named _alloc.
    @type container_gets_allocator: bool
    @param cpp_name_prefix: The C++ prefix to use when referring to the message, e.g. "std_msgs::"
    @type cpp_name_prefix: str
    """
    # Assign all fixed-length arrays their default values
    for field in spec.parsed_fields():
        if (not field.is_array or field.array_len is None):
            continue
        
        val = default_value(field.base_type)
        if (container_gets_allocator and takes_allocator(field.base_type)):
            # String is a special case, as it is the only builtin type that takes an allocator
            if (field.base_type == "string"):
                string_cpp = msg_type_to_cpp("string")
                s.write('    %s.assign(%s(_alloc));\n'%(field.name, string_cpp))
            else:
                (cpp_msg_unqualified, cpp_msg_with_alloc, _) = cpp_message_declarations(cpp_name_prefix, field.base_type)
                s.write('    %s.assign(%s(_alloc));\n'%(field.name, cpp_msg_with_alloc))
        elif (len(val) > 0):
            s.write('    %s.assign(%s);\n'%(field.name, val))

def write_constructors(s, spec, cpp_name_prefix):
    """
    Writes any necessary constructors for the message
    
    @param s: The stream to write to
    @type s: stream
    @param spec: The message spec
    @type spec: roslib.msgs.MsgSpec
    @param cpp_name_prefix: The C++ prefix to use when referring to the message, e.g. "std_msgs::"
    @type cpp_name_prefix: str
    """
    
    msg = spec.short_name
    
    # Default constructor
    s.write(' %s%s_::%s_()\n'%(cpp_name_prefix,msg, msg))
    write_initializer_list(s, spec, False)
    s.write('  {\n')
    write_fixed_length_assigns(s, spec, False, cpp_name_prefix)
    s.write('  }\n\n')


def write_deconstructor(s, spec, cpp_name_prefix):
    """
    Writes any necessary deconstructors for the message

    @param s: The stream to write to
    @type s: stream
    @param spec: The message spec
    @type spec: roslib.msgs.MsgSpec
    @param cpp_name_prefix: The C++ prefix to use when referring to the message, e.g. "std_msgs::"
    @type cpp_name_prefix: str
    """

    msg = spec.short_name

    # Default deconstructor
    s.write('  %s%s_::~%s_() {\n'%(cpp_name_prefix, msg, msg))
    s.write("  } //deconstructor\n\n")
    # TODO(hjed): work out if we need to kill the Message Descriptor here

def declare_constructors(s, spec, cpp_name_prefix):
    """
    Declares any necessary constructors for the message
    
    @param s: The stream to write to
    @type s: stream
    @param spec: The message spec
    @type spec: roslib.msgs.MsgSpec
    @param cpp_name_prefix: The C++ prefix to use when referring to the message, e.g. "std_msgs::"
    @type cpp_name_prefix: str
    """

    msg = spec.short_name

    # Default constructor
    s.write('  %s_();\n'%(msg))


def declare_deconstructor(s, spec, cpp_name_prefix):
    """
    Declares any necessary deconstructors for the message
    
    @param s: The stream to write to
    @type s: stream
    @param spec: The message spec
    @type spec: roslib.msgs.MsgSpec
    @param cpp_name_prefix: The C++ prefix to use when referring to the message, e.g. "std_msgs::"
    @type cpp_name_prefix: str
    """

    msg = spec.short_name

    # Default deconstructor
    s.write('  ~%s_();\n'%(msg))

def declare_virtual_functions(s, spec, cpp_name_prefix):
    """
    Declares any necessary virtual functions for the message
    
    @param s: The stream to write to
    @type s: stream
    @param spec: The message spec
    @type spec: roslib.msgs.MsgSpec
    @param cpp_name_prefix: The C++ prefix to use when referring to the message, e.g. "std_msgs::"
    @type cpp_name_prefix: str
    """

    msg = spec.short_name

    # Default functions
    s.write('  virtual void generate_block_impl();\n')
    s.write('  virtual ros_echronos::Message_Descriptor * generate_descriptor();\n')

def write_member(s, field):
    """
    Writes a single member's declaration and type typedef
    
    @param s: The stream to write to
    @type s: stream
    @param type: The member type
    @type type: str
    @param name: The name of the member
    @type name: str
    """
    cpp_type = msg_type_to_cpp(field.type)
    s.write('  typedef %s _%s_type;\n'%(cpp_type, field.name))
    s.write('  %s %s;\n\n'%(cpp_type, field.name))

def write_members(s, spec):
    """
    Write all the member declarations
    
    @param s: The stream to write to
    @type s: stream
    @param spec: The message spec
    @type spec: roslib.msgs.MsgSpec
    """
    [write_member(s, field) for field in spec.parsed_fields()]

    # write descriptor pointer
    s.write('  ros_echronos::Message_Descriptor * desc = NULL;\n')


def write_deserialiser(s, spec, cpp_name_prefix):
    """
    Writes the deserialisation function for the message class.
    
    Approach
    
    # declare new message class
    # Store a list of structs storing pointer to a message variable and the size of the variable. Include in the list
      items for array/string size values that point to the size variable in the next struct. Have a flag for variable length
      variables.
    # Loop through each can message packet, storing an index of where we are in the array of varriables. 
    # Write to the variables byte by byte, using the counter in the struct. 
    
    @param s: The stream to write to
    @type s: stream
    @param spec: The message spec
    @type spec: roslib.msgs.MsgSpec
    @param cpp_name_prefix: the prefix in cpp
    @type cpp_name_prefix: str
    """

    num_messages = 0 # TODO: calculate how many can messages this ros message should take up

    msg = spec.short_name
    s.write('  static %s%s %s%s_::msg_from_CAN(can::CAN_ROS_MSG * msg, size_t msgs) {\n' % (cpp_name_prefix, msg, cpp_name_prefix, msg))


    s.write('     %s%s msg;' % (cpp_name_prefix, msg))

    num_var_fields = 0 # TODO: count these

    s.write('     _Field_Store fields[%d];' % len(spec.parsed_fields()) + num_var_fields)
    index = 0
    for field in spec.parsed_fields():

        (base_type, is_array, array_len) = roslib.msgs.parse_type(field.type)

        if is_array:

            s.write('      fields[%d].start_of_field = &(msg.%s.size);' % (index, field.name)) # stores the size field as a pointer
            s.write('      fields[%d].size = sizeof(msg.%s.size);' % (index, field.name))
            s.write('      fields[%d].is_var_length = false;' % (index)) # store that it is an array
            index+=1
            s.write('      fields[%d].start_of_field = &(msg.%s.value);' % (index, field.name)) # store the address of the field
            s.write('      fields[%d].size = 0;' % (index)) # store the size (0 for an array)
            s.write('      fields[%d].is_var_length = true;' % (index)) # store that it is an array
        else:
            s.write('      fields[%d].start_of_field = &(msg.%s);' % (index, field.name)) # stores the address of the field
            s.write('      fields[%d].size = sizeof(msg.%s);' % (index, field.name))
            s.write('      fields[%d].is_var_length = false;' % (index)) # store that it is an array
        index+=1


    s.write('     size_t msg_index = 0;')
    s.write('     size_t msg_offset = 0;')
    s.write('     for(int i = 0; i < %d; ++i) { ' % num_var_fields)
    # copy the length from the size field into the read arrays length
    s.write('         if(fields[i].is_var_length) { '
            '             fields[i].size = *((uint16_t)fields[i-1].start_of_field);'
            '         }') # TODO: the variable length array's size field should NOT be in bytes
    s.write('         for(int field_index = 0; field_index < fields[i].size; ++field_index, ++message_index) {'
            '             if(message_index >= CAN_MESSAGE_MAX_LEN) { ++msg_index; msg_offset=0; }'
            '             fields[i].start_of_field[field_index] =  msg[msg_index].body[msg_offset];'
            '         }')

    s.write('     } // for loop')
    s.write('     return msg;')

    s.write('  } // msg_from_CAN')


def write_virtual_functions(s, spec, cpp_name_prefix):
    """
    Writes the virtual functions of the message class
    
    @param s: The stream to write to
    @type s: stream
    @param spec: The message spec
    @type spec: roslib.msgs.MsgSpec
    @param cpp_name_prefix: the prefix in cpp
    @type cpp_name_prefix: str
    """

    msg = spec.short_name
    s.write('  void %s%s_::generate_block_impl() {\n' % (cpp_name_prefix, msg))
    output = ""
    sizes=[]
    for field in spec.parsed_fields():

        (base_type, is_array, array_len) = roslib.msgs.parse_type(field.type)
        if is_array:
            sizes.append("%s.bytes+2" % field.name)
            output+='      memcpy(block+offset, sizeof(short), %s.size);\n' % (field.name)
            output+='      memcpy(block+offset+sizeof(short), %s.values, %s.bytes);\n' % (field.name, field.name)
            output+='      offset+=%s.bytes+sizeof(short);\n' % (field.name)
        else:
            sizes.append("sizeof(%s)" % field.name)
            output+='      memcpy(block+offset, &%s, sizeof(%s));\n' % (field.name, field.name)
            output+='      offset+=sizeof(%s);\n' % (field.name)
    s.write('      size_t offset = 0;\n')
    s.write('      size = %s;\n' %("+".join(sizes)))
    s.write('      block = (uint8_t *) alloc::malloc(size);\n')
    s.write(output);
    s.write('  } // generate_block\n')

    num_fields = len(spec.parsed_fields())
    s.write('  ros_echronos::Message_Descriptor * %s%s_::generate_descriptor() {\n' % (cpp_name_prefix, msg))
    s.write('    void * desc = alloc::malloc(sizeof(ros_echronos::Message_Descriptor_Fixed<%d>));\n' % num_fields)
    s.write('    ros_echronos::Message_Descriptor_Fixed<%d> * descriptor = new (desc) ros_echronos::Message_Descriptor_Fixed<%d>();\n' % (num_fields, num_fields))
    i = 0
    for field in spec.parsed_fields():
        (base_type, is_array, array_len) = roslib.msgs.parse_type(field.type)
        s.write('    descriptor->fixed_field_ptrs[%d] = &%s;\n' % (i, field.name))
        if is_array:
            s.write('    descriptor->fixed_field_sizes[%d] = 0;\n' % (i))
        else:
            s.write('    descriptor->fixed_field_sizes[%d] = sizeof(%s);\n' % (i, field.name))
        i+=1
    s.write('    return descriptor;\n')
    s.write('  }\n')

def escape_string(str):
    str = str.replace('\\', '\\\\')
    str = str.replace('"', '\\"')
    return str
        
def write_constant_declaration(s, constant):
    """
    Write a constant value as a static member
    
    @param s: The stream to write to
    @type s: stream
    @param constant: The constant
    @type constant: roslib.msgs.Constant
    """
    
    # integral types get their declarations as enums to allow use at compile time
    if (constant.type in ['byte', 'int8', 'int16', 'int32', 'int64', 'char', 'uint8', 'uint16', 'uint32', 'uint64']):
        s.write('  enum { %s = %s };\n'%(constant.name, constant.val))
    else:
        s.write('  static const %s %s;\n'%(msg_type_to_cpp(constant.type), constant.name))

def write_constant_definitions(s, spec):
    """
    Write all the constants from a spec as static members
    
    @param s: The stream to write to
    @type s: stream
    @param spec: The message spec
    @type spec: roslib.msgs.MsgSpec
    """
    [write_constant_definition(s, spec, constant) for constant in spec.constants]
    s.write('\n')
        
def is_fixed_length(spec):
    """
    Returns whether or not the message is fixed-length
    
    @param spec: The message spec
    @type spec: roslib.msgs.MsgSpec
    @param package: The package of the
    @type package: str
    """
    types = []
    for field in spec.parsed_fields():
        if (field.is_array and field.array_len is None):
            return False
        
        if (field.base_type == 'string'):
            return False
        
        if (not field.is_builtin):
            types.append(field.base_type)
            
    types = set(types)
    for type in types:
        type = roslib.msgs.resolve_type(type, spec.package)
        (_, new_spec) = roslib.msgs.load_by_type(type, spec.package)
        if (not is_fixed_length(new_spec)):
            return False
        
    return True
    
def write_deprecated_member_functions(s, spec, traits):
    """
    Writes the deprecated member functions for backwards compatibility
    """
    for field in spec.parsed_fields():
        if (field.is_array):
            s.write('  ROS_DEPRECATED uint32_t get_%s_size() const { return (uint32_t)%s.size(); }\n'%(field.name, field.name))
            
            if (field.array_len is None):
                s.write('  ROS_DEPRECATED void set_%s_size(uint32_t size) { %s.resize((size_t)size); }\n'%(field.name, field.name))
                s.write('  ROS_DEPRECATED void get_%s_vec(%s& vec) const { vec = this->%s; }\n'%(field.name, msg_type_to_cpp(field.type), field.name))
                s.write('  ROS_DEPRECATED void set_%s_vec(const %s& vec) { this->%s = vec; }\n'%(field.name, msg_type_to_cpp(field.type), field.name))
    
    for k, v in traits.items():
        s.write('private:\n')
        s.write('  static const char* __s_get%s_() { return "%s"; }\n'%(k, v))
        s.write('public:\n')
        s.write('  ROS_DEPRECATED static const std::string __s_get%s() { return __s_get%s_(); }\n\n'%(k, k))
        s.write('  ROS_DEPRECATED const std::string __get%s() const { return __s_get%s_(); }\n\n'%(k, k))
    
    s.write('  ROS_DEPRECATED virtual uint8_t *serialize(uint8_t *write_ptr, uint32_t seq) const\n  {\n')
    s.write('    ros::serialization::OStream stream(write_ptr, 1000000000);\n')
    for field in spec.parsed_fields():
        s.write('    ros::serialization::serialize(stream, %s);\n'%(field.name))
    s.write('    return stream.getData();\n  }\n\n')
    
    s.write('  ROS_DEPRECATED virtual uint8_t *deserialize(uint8_t *read_ptr)\n  {\n')
    s.write('    ros::serialization::IStream stream(read_ptr, 1000000000);\n');
    for field in spec.parsed_fields():
        s.write('    ros::serialization::deserialize(stream, %s);\n'%(field.name))
    s.write('    return stream.getData();\n  }\n\n')
    
    s.write('  ROS_DEPRECATED virtual uint32_t serializationLength() const\n  {\n')
    s.write('    uint32_t size = 0;\n');
    for field in spec.parsed_fields():
        s.write('    size += ros::serialization::serializationLength(%s);\n'%(field.name))
    s.write('    return size;\n  }\n\n')

def compute_full_text_escaped(gen_deps_dict):
    """
    Same as roslib.gentools.compute_full_text, except that the
    resulting text is escaped to be safe for C++ double quotes

    @param get_deps_dict: dictionary returned by get_dependencies call
    @type  get_deps_dict: dict
    @return: concatenated text for msg/srv file and embedded msg/srv types. Text will be escaped for double quotes
    @rtype: str
    """
    definition = roslib.gentools.compute_full_text(gen_deps_dict)
    lines = definition.split('\n')
    s = StringIO()
    for line in lines:
        line = escape_string(line)
        s.write('%s\\n\\\n'%(line))
        
    val = s.getvalue()
    s.close()
    return val

def is_hex_string(str):
    for c in str:
        if c not in '0123456789abcdefABCDEF':
            return False
        
    return True


def write_template_includes(s, spec, cpp_prefix):
    """
    Writes the includes and template definitions for the message
    @param s: the output stream
    @param spec: the message spec
    @param cpp_prefix: the package cpp prefix
    """
    s.write('#include "templates/Publisher.cpp"\n')
    s.write('#include "templates/Subscriber.cpp"\n')

    s.write('template class ros_echronos::Publisher<%s%s>;\n' % (cpp_prefix, spec.short_name))
    s.write('template class ros_echronos::Subscriber<%s%s>;\n' % (cpp_prefix, spec.short_name))

def write_cpp_body(s, spec, cpp_prefix):
    """
    Writes the cpp body file we need to generate all our templates

    @param s (StringIO): the feed to write to
    @param spec: the message spec
    @param cpp_prefix: the cpp prefix
    """

    # headers
    s.write('#include "%s/%s.hpp"\n' % (spec.package, spec.short_name))

    # due to compiler problems these need to be decleared externally
    write_constructors(s, spec, cpp_prefix)
    write_deconstructor(s, spec, cpp_prefix)
    write_virtual_functions(s, spec, cpp_prefix)
    write_template_includes(s, spec, cpp_prefix)


def generate(msg_path):
    """
    Generate a message
    
    @param msg_path: The path to the .msg file
    @type msg_path: str
    """
    (package_dir, package) = roslib.packages.get_dir_pkg(msg_path)
    #TODO: remove this
    package_dir = "./"

    (_, spec) = roslib.msgs.load_from_file(msg_path, package)
    
    header = StringIO()
    write_begin(header, spec, msg_path)
    write_generic_includes(header)
    write_includes(header, spec)
    
    cpp_prefix = '%s::'%(package)
    
    header.write('namespace %s\n{\n'%(package))
    write_struct(header, spec, cpp_prefix)
    write_constant_definitions(header, spec)
    header.write('} // namespace %s\n\n'%(package))
    write_end(header, spec)

    cpp = StringIO()
    write_cpp_body(cpp, spec, cpp_prefix)
    
    output_dir = '%s/msg_gen/cpp/include/%s'%(package_dir, package)
    if (not os.path.exists(output_dir)):
        # if we're being run concurrently, the above test can report false but os.makedirs can still fail if
        # another copy just created the directory
        try:
            os.makedirs(output_dir)
        except OSError as e:
            pass
         
    f = open('%s/%s.hpp'%(output_dir, spec.short_name), 'w')
    f.write(header.getvalue() + "\n")

    cpp_output_dir = "%s/msg_gen/cpp/%s" % (package_dir, package)
    if (not os.path.exists(cpp_output_dir)):
        # if we're being run concurrently, the above test can report false but os.makedirs can still fail if
        # another copy just created the directory
        try:
            os.makedirs(cpp_output_dir)
        except OSError as e:
            pass

    with open("%s/%s.cpp" % (cpp_output_dir, spec.short_name), 'w') as f:
        f.write(cpp.getvalue() + "\n")
    
    header.close()

def generate_messages(argv):
    for arg in argv[1:]:
        generate(arg)

if __name__ == "__main__":
    roslib.msgs.set_verbose(False)
    generate_messages(sys.argv)

