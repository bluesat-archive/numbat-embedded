#!/bin/sh

if [ $# -eq 0 ]
then
    echo "Usage: sudo $0 <module>"
    exit 1
fi

if [ ! `ls elfs | grep "^$1\.elf$"` ]
then
    echo "No module called \"$1\""
    exit 1
fi

openocd="sudo openocd -f board/ek-tm4c123gxl.cfg"
serial="sleep 1; picocom -b 115200 /dev/ttyACM0"
debug="sleep 2; ./flash.sh elfs/$1.elf"

gnome-terminal --tab --command="bash -c '$openocd; $SHELL'" \
               --tab --command="bash -c '$serial; $SHELL'" \
               --tab --command="bash -c '$debug; $SHELL'" \
               --disable-factory
