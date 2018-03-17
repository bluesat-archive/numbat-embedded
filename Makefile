LIB_BUILD = libs/build
LIBS_DIR = libs
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
# BOILERPLATE CODE
# ****************

$(BUILD_DIR)/boilerplate.o: $(MODULES_DIR)/boilerplate/boilerplate.c

# ***********
# ADC LIBRARY
# ***********

$(BUILD_DIR)/adc.o: $(LIBS_DIR)/adc/adc.c

# *************
# BLINKY MODULE
# *************

MODULE_NAME=blinky
include .construct_numbat_module

$(BUILD_DIR)/blinky.o: $(MODULE_DIR)/blinky.c $(MODULE_ECHRONOS) ti_libs
$(BUILD_DIR)/$(MODULE_NAME).elf: \
	$(BUILD_DIR)/blinky.o \
	$(BUILD_DIR)/boilerplate.o \
	$(LIB_BUILD)/$(MODULE_NAME)-echronos.a

# *****************
# TIMER TEST MODULE
# *****************

MODULE_NAME=timer_test
include .construct_numbat_module

$(BUILD_DIR)/timer_test.o: $(MODULE_DIR)/timer_test.c $(MODULE_ECHRONOS) ti_libs
$(BUILD_DIR)/$(MODULE_NAME).elf: \
	$(BUILD_DIR)/timer_test.o \
	$(BUILD_DIR)/boilerplate.o \
	$(LIB_BUILD)/$(MODULE_NAME)-echronos.a

# ***************
# CAN TEST MODULE
# ***************

MODULE_NAME=can_test
include .construct_numbat_module

$(BUILD_DIR)/can_test.o: $(MODULE_DIR)/can_test.c $(MODULE_ECHRONOS) ti_libs
$(BUILD_DIR)/$(MODULE_NAME).elf: \
	$(BUILD_DIR)/can_test.o \
	$(BUILD_DIR)/boilerplate.o \
	$(LIB_BUILD)/$(MODULE_NAME)-echronos.a

# ********************
# ECHRONOS TEST MODULE
# ********************

MODULE_NAME=echronos_test
include .construct_numbat_module

$(BUILD_DIR)/echronos_test.o: $(MODULE_DIR)/echronos_test.c $(MODULE_ECHRONOS) ti_libs
$(BUILD_DIR)/$(MODULE_NAME).elf: \
	$(BUILD_DIR)/echronos_test.o \
	$(BUILD_DIR)/boilerplate.o \
	$(LIB_BUILD)/$(MODULE_NAME)-echronos.a

# ********************
# ADC TEST MODULE
# ********************

MODULE_NAME=adc_test
include .construct_numbat_module

$(BUILD_DIR)/adc_test.o: $(MODULE_DIR)/adc_test.c $(MODULE_ECHRONOS) ti_libs
$(BUILD_DIR)/$(MODULE_NAME).elf: \
	$(BUILD_DIR)/adc_test.o \
	$(BUILD_DIR)/boilerplate.o \
	$(LIB_BUILD)/$(MODULE_NAME)-echronos.a \
        $(BUILD_DIR)/adc.o

# **********************
# WHAT TO ACTUALLY BUILD
# **********************

TARGETS=\
	$(BUILD_DIR)/blinky.elf \
	$(BUILD_DIR)/timer_test.elf \
	$(BUILD_DIR)/can_test.elf \
	$(BUILD_DIR)/echronos_test.elf \
        $(BUILD_DIR)/adc_test.elf

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
