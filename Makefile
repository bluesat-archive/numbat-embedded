LIB_BUILD = libs/build
LIB_DIR = libs
MODULES_DIR = modules
BUILD_DIR=build
ECHRONOS_BUILD = echronos/build

include .compiler_makedefs
include .utility_makedefs
include .echronos_makedefs
include .lib_makedefs
include .numbat_makedefs

# Add an object target for each source file that you
# wish to compile. TODO: Perhaps streamline this.

# NUMBAT MODULE SOURCES BEGIN HERE

# ****************
# tlsf
# ****************

$(BUILD_DIR)/tlsf.o: $(LIB_DIR)/tlsf/tlsf.c $(LIB_DIR)/tlsf/tlsf.h
tlsf: $(BUILD_DIR)/tlsf.o

# ****************
# BOILERPLATE CODE
# ****************

$(BUILD_DIR)/boilerplate.o: $(MODULES_DIR)/boilerplate/boilerplate.c
#these need to be C++ even though they are
$(BUILD_DIR)/crtn.opp: $(MODULES_DIR)/boilerplate/crtn.cpp
$(BUILD_DIR)/crti.opp: $(MODULES_DIR)/boilerplate/crti.cpp


# ****************
# ROS LIB
# ****************

# *************
# BLINKY MODULE
# *************

MODULE_NAME=blinky
include .construct_numbat_module

$(BUILD_DIR)/blinky.opp: $(MODULE_DIR)/blinky.cpp $(MODULE_ECHRONOS) ti_libs ros_echronos tlsf
$(BUILD_DIR)/$(MODULE_NAME).elf: \
	$(BUILD_DIR)/blinky.opp \
	$(BUILD_DIR)/boilerplate.o \
	$(LIB_BUILD)/$(MODULE_NAME)-echronos.a \
	$(BUILD_DIR)/tlsf.o

# *************
# ROS_TEST MODULE
# *************

MODULE_NAME=ros_test
include .construct_numbat_module

$(BUILD_DIR)/ros_test.opp: $(MODULE_DIR)/ros_test.cpp $(MODULE_ECHRONOS) ti_libs ros_echronos tlsf
$(BUILD_DIR)/$(MODULE_NAME).elf: \
	$(BUILD_DIR)/ros_test.opp \
	$(BUILD_DIR)/boilerplate.o \
	$(LIB_BUILD)/$(MODULE_NAME)-echronos.a \
	$(BUILD_DIR)/tlsf.o

# *************
# ROS_SUB_TEST MODULE
# *************

MODULE_NAME=ros_sub_test
include .construct_numbat_module

$(BUILD_DIR)/ros_sub_test.opp: $(MODULE_DIR)/ros_sub_test.cpp $(MODULE_ECHRONOS) ti_libs ros_echronos tlsf
$(BUILD_DIR)/can_wait_task.oop: $(MODULE_DIR)/can_wait_task.cpp
$(BUILD_DIR)/$(MODULE_NAME).elf: \
	$(BUILD_DIR)/ros_sub_test.opp \
	$(BUILD_DIR)/can_wait_task.opp \
	$(BUILD_DIR)/boilerplate.o \
	$(LIB_BUILD)/$(MODULE_NAME)-echronos.a \
	$(BUILD_DIR)/tlsf.o

# *****************
# TIMER TEST MODULE
# *****************

MODULE_NAME=timer_test
include .construct_numbat_module

$(BUILD_DIR)/timer_test.o: $(MODULE_DIR)/timer_test.c $(MODULE_ECHRONOS) ti_libs tlsf
$(BUILD_DIR)/$(MODULE_NAME).elf: \
	$(BUILD_DIR)/timer_test.o \
	$(BUILD_DIR)/boilerplate.o \
	$(LIB_BUILD)/$(MODULE_NAME)-echronos.a \
	$(BUILD_DIR)/tlsf.o

# ***************
# CAN TEST MODULE
# ***************

MODULE_NAME=can_test
include .construct_numbat_module

$(BUILD_DIR)/can_test.o: $(MODULE_DIR)/can_test.c $(MODULE_ECHRONOS) ti_libs tlsf
$(BUILD_DIR)/$(MODULE_NAME).elf: \
	$(BUILD_DIR)/can_test.o \
	$(BUILD_DIR)/boilerplate.o \
	$(LIB_BUILD)/$(MODULE_NAME)-echronos.a \
	$(BUILD_DIR)/tlsf.o

# ********************
# ECHRONOS TEST MODULE
# ********************

MODULE_NAME=echronos_test
include .construct_numbat_module

$(BUILD_DIR)/echronos_test.o: $(MODULE_DIR)/echronos_test.c $(MODULE_ECHRONOS) ti_libs tlsf
$(BUILD_DIR)/$(MODULE_NAME).elf: \
	$(BUILD_DIR)/echronos_test.o \
	$(BUILD_DIR)/boilerplate.o \
	$(LIB_BUILD)/$(MODULE_NAME)-echronos.a \
	$(BUILD_DIR)/tlsf.o

# **********************
# WHAT TO ACTUALLY BUILD
# **********************

TARGETS=\
	$(BUILD_DIR)/blinky.elf \
	$(BUILD_DIR)/timer_test.elf \
	$(BUILD_DIR)/can_test.elf \
	$(BUILD_DIR)/echronos_test.elf \
	$(BUILD_DIR)/ros_test.elf

# NUMBAT MODULE SOURCES END HERE


.DEFAULT_GOAL := all
all: dirs ti_libs $(TARGETS)

dirs:
	mkdir -p $(BUILD_DIR)
	mkdir -p $(LIB_BUILD)

modules_clean:
	$(CLEAN_PRINT)
	rm -rf $(BUILD_DIR)/*

clean: modules_clean echronos_lib_clean ti_libs_clean
	$(CLEAN_PRINT)
	rm -rf $(BUILD_DIR) $(LIB_BUILD)

docs: all
	doxygen doxygen.conf