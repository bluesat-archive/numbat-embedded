#!/bin/bash

# This script will flash the first argument (an elf file)
# to our numbat boards using gdb, reset, then halt at the
# beginning of main. User should press c again for normal
# execution

set -e

if [ -z "$1" ]
  then
    echo "Please supply an elf file to flash!"
    exit
fi

#if [ ${1: -4} == ".elf" ]
#  then
#    echo "$1 is an elf file."
#  else
#    echo "File to flash must be an elf file!"
#    exit
#fi

if [ -e "$1" ]
then
    echo "$1 exists."
else
    echo "$1 does not exist!"
    exit
fi

arm-none-eabi-gdb $1 \
    -ex 'target extended localhost:3333' \
    -ex 'monitor reset halt' \
    -ex 'load' \
    -ex 'monitor reset halt' \
    -ex 'b main' \
    -ex 'c' \

