# Overview
This is all the code that runs on numbat's embedded module computers

It contains a few key components:

- **Module-specific codebases** - these define the behaviour of different numbat modules. This is found in the `modules` directory.
- **eChronos** - an open-source Real Time Operating System that provides cool stuff like task scheduling & prioritisation. (See https://github.com/echronos/echronos). An echronos distribution is found in the `echronos` directory.
- **The TI software framework** - A bunch of driver libraries that make developing on our hardware easier, abstracting away lots of painful operations. This can be found in the `libs/ti` directory.

**To grab the source**:

    git clone https://github.com/bluesat/numbat-embedded.git
    cd numbat-embedded

# Getting up & running

First, make sure you have the `arm-none-eabi` GNU toolchain and `arm-none-eabi-gdb` for building and debugging the RTOS for ARM. These can be found in ubuntu/debian/arch package repositories.

If you decide to use a different ARM toolchain, you may have to modify the `.compiler_makedefs` file to suit your system.

In the root folder, run:

    make

to build all modules.

If you are having problems, check your prerequisites are being met.

If everything goes well, you will end up with a bunch of .elf files in the build directory in the repository root folder. These are ready to flash to the hardware!

# Running modules on real hardware

To flash your elf files and test your code, you'll need OpenOCD. This is an open-source debugging interface tool that allows you to use GDB to flash images and debug your code.

You have 2 options here. You can install OpenOCD from your distributions package manager, or install OpenOCD from source. See (https://github.com/ntfreak/openocd) for the second option

Once you have OpenOCD installed
    - connect the EK-TM4C123GXL board to your computer via the USB connector labelled 'debug'
    - Make sure the 'PWR SELECT' switch is set to 'DEBUG' and the jumpers are in their default places.

Open a connection to the board:

    sudo openocd -f board/ek-tm4c1294xl.cfg

If everything goes well, you shouldn't see any error messages. If you are using OpenOCD from source, your commands look a little different. If you built it at `~/openocd`, your commands will look something like:

    cd ~/openocd/tcl
    sudo ~/openocd/src/openocd -f ~/openocd/tcl/board/ek-tm4c1294xl.cfg

If you get stuck, there are plenty of tutorials on the internet for getting OpenOCD up and running.

### Flashing & Debugging

Once you have started OpenOCD, it's time to flash and debug your image! This repository includes a script that does this all for you, `flash.sh`. Take a look at it to see what it's doing if you're curious.

To try the blinky example, (making sure you have built everything!):

    ./flash.sh build/blinky.elf

GDB should fire up, connect to your OpenOCD server, flash the image, reset and then break on your `main` function. To see cool stuff happen, press `c` to continue execution.

Hopefully, you get some blinky lights!

Note that you can also use any of the conventional GDB commands - stepping, watching, breakpoints etc. Plenty of resources on the internet for this.

Use CTRL-C and CTRL-D to exit GDB.

### Serial terminal & `printf`

Any use of the `UARTPrintf` instructions in your codebase will spit out strings on a serial device. You can observe these by firing up a serial terminal, usually the device is called `ttyACMX` where X is some number. I enjoy `picocom`, but your choice of serial terminal is up to you. `gtkterm` is also good. Make sure you set your baud rates and settings to `115200-8-n-1`. E.g for picocom:

    picocom -b 115200 /dev/ttyACM0

If you have issues, make sure your user has been added to the `dialout` group or equivalent - or just run `picocom` as sudo.

To test this, you can try the `blinky` example - you should see something like:

    Starting RTOS...
    Entered blinky task
    Initialized GPIOS
    ..................................

# eChronos configuration

Each numbat module contains a `.prx` file, which is an eChronos configuration file. The `.prx` file is used for declaring OS primitives, like tasks, mutexes, semaphores, interrupts, etc. When a numbat module is built, a customized version of eChronos is constructed from this file.
For information as to how to use this file, see the 'Configuration Reference' section of the eChronos manual `docs/echronos/*`

# Adding new modules

Adding new numbat modules involves creating a new folder in the `modules` directory with at least one `.c` and `.prx` file, as well as modifying the top-level makefile.

To add a new module to the Makefile, open it up and copy one of the existing modules. You will need to change the `MODULE_NAME`, as well as the compilation steps. For each source file in your module, you will want to:

- Add an `.o` (object) target for each `.c` (source file). i.e:

        $(BUILD_DIR)/echronos_test.o: $(MODULE_DIR)/echronos_test.c $(MODULE_ECHRONOS)

- Add the object target to your final elf.

        $(BUILD_DIR)/$(MODULE_NAME).elf: \
            $(BUILD_DIR)/echronos_test.o \ # <-- Like this
            $(LIB_BUILD)/$(MODULE_NAME)-echronos.a

The eChronos & TI header files and sources will automagically be attached to your elf file, so you don't need to worry about that.

Finally, so that your new module actually gets looked at, you will need to add to to the `TARGETS` line as a new elf target:

    TARGETS=\
        $(BUILD_DIR)/blinky.elf \
        $(BUILD_DIR)/echronos_test.elf # <-- Like this

Hit make, and all compiling well you should get your shiny new module in `build`

# Help & Documentation

The following manuals are kept in this repository **in case of upstream changes**. We do **not** claim ownership over any of the contents of these manuals.:

* **eChronos manual**: `docs/echronos/*`. Note that this is for the kochab (feature set) armv7m (platform) echronos variant *only*, which is what we are using.

* **TI library user guide**: `docs/ti/*`. API documentation for the various TI libraries available to numbat modules.

* Note: **TI example code**: This can be found on TI's website inside their software package (TivaWare for EK-TM4C123GXL). Useful for figuring out how to use some peripherals. It is not distributed here for copyright reasons but mostly because it is huge.
