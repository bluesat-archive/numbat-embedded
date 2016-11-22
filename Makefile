LIB_BUILD = libs/build
MODULES_DIR = modules

include .utility_makedefs
include .echronos_makedefs
include .lib_makedefs
include .numbat_makedefs

# NUMBAT MODULE SOURCES BEGIN HERE

# Add an object target for each source file that you
# wish to compile. TODO: Perhaps streamline this.

# *************
# BLINKY MODULE
# *************

MODULE_NAME=blinky
MODULE_DIR=$(MODULES_DIR)/$(MODULE_NAME)
MODULE_ECHRONOS=$(LIB_BUILD)/$(MODULE_NAME)-echronos.a

$(BUILD_DIR)/blinky.o: $(MODULE_DIR)/blinky.c $(MODULE_ECHRONOS)
$(BUILD_DIR)/$(MODULE_NAME).elf: \
	$(BUILD_DIR)/blinky.o \
	$(LIB_BUILD)/$(MODULE_NAME)-echronos.a
$(MODULE_ECHRONOS): $(MODULE_DIR)/$(MODULE_NAME).prx

# ********************
# ECHRONOS TEST MODULE
# ********************

MODULE_NAME=echronos_test
MODULE_DIR=$(MODULES_DIR)/$(MODULE_NAME)
MODULE_ECHRONOS=$(LIB_BUILD)/$(MODULE_NAME)-echronos.a

$(BUILD_DIR)/echronos_test.o: $(MODULE_DIR)/echronos_test.c $(MODULE_ECHRONOS)
$(BUILD_DIR)/$(MODULE_NAME).elf: \
	$(BUILD_DIR)/echronos_test.o \
	$(LIB_BUILD)/$(MODULE_NAME)-echronos.a
$(MODULE_ECHRONOS): $(MODULE_DIR)/$(MODULE_NAME).prx

# WHAT WE SHOULD TRY AND BUILD WHEN MAKE IS RUN
TARGETS=\
	$(BUILD_DIR)/blinky.elf \
	$(BUILD_DIR)/echronos_test.elf

# NUMBAT MODULE SOURCES END HERE

modules_clean:
	$(CLEAN_PRINT)
	rm -rf $(BUILD_DIR)/*

.DEFAULT_GOAL := all

dirs:
	mkdir -p $(BUILD_DIR)
	mkdir -p $(LIB_BUILD)

all: dirs ti_libs $(TARGETS)

clean: modules_clean echronos_lib_clean ti_libs_clean
	$(CLEAN_PRINT)
	rm -rf $(BUILD_DIR) $(LIB_BUILD)
