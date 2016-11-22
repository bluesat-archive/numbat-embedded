LIB_BUILD = libs/build
MODULES_DIR = modules

BUILD_PRINT = @echo -e "\e[1;32mBuilding $^ -> $@\e[0m"
CLEAN_PRINT = @echo -e "\e[1;32mCleaning -> $@\e[0m"
MODULE_PRINT = @echo -e "\e[1;32mSuccessfully built module -> $@\e[0m"

# BEGIN ECHRONOS BUILD MACHINERY

PRJ_TOOL = echronos/bin/prj.sh
ECHRONOS_BUILD = echronos/build

.SECONDEXPANSION:
$(LIB_BUILD)/%.a: $$(wildcard $(MODULES_DIR)/*/%.prx)
	$(BUILD_PRINT)
	$(PRJ_TOOL) \
		--no-project \
		--search-path echronos/share/packages \
		--search-path . \
		--output $(ECHRONOS_BUILD)/${*} \
		build $(subst .prx,,$(subst /,.,${<}))
	cp $(ECHRONOS_BUILD)/${*}/system.a ${@}
	rm -r out

echronos_lib_clean:
	$(CLEAN_PRINT)
	rm -r -f out
	rm -r -f $(ECHRONOS_BUILD)

# END ECHRONOS BUILD MACHINERY

# BEGIN TI LIBRARY BUILD MACHINERY

TI_LIBS = libs/ti
TI_LIB_DIRS = $(TI_LIBS)/driverlib \
			  $(TI_LIBS)/sensorlib \
			  $(TI_LIBS)/usblib

ti_libs:
	$(BUILD_PRINT)
	@for i in ${TI_LIB_DIRS};           \
	 do                                 \
		 if [ -f $${i}/Makefile ];      \
		 then                           \
			 make -C $${i} || exit $$?; \
			 cp $${i}/gcc/*.a $(LIB_BUILD); \
		 fi;                            \
	 done

ti_libs_clean:
	$(CLEAN_PRINT)
	@for i in ${TI_LIB_DIRS};           \
	 do                                 \
		 if [ -f $${i}/Makefile ];      \
		 then                           \
			 make -C $${i} clean;       \
		 fi;                            \
	 done
	rm -f $(LIB_BUILD)/*.a

# END TI LIBRARY BUILD MACHINERY

# CONFIGURING NUMBAT SOURCE COMPILER OPTIONS
# (These inherit from the ti library compiler options)
# Be careful modifying cflags as they are delicately balanced to
# work. eChronos is compiled with its own set of cflags, as are the
# TI libraries.

PART=TM4C123GH6PM
CFLAGSgcc=-DTARGET_IS_TM4C123_RB1
include makedefs
SRC=src
BUILD_DIR=build
IPATH=$(TI_LIBS)

$(BUILD_DIR)/%.o: $(wildcard $(MODULES_DIR)/*/%.c)
	$(BUILD_PRINT)
	${CC} ${CFLAGS} \
		-I$(ECHRONOS_BUILD)/$(subst modules/,,$(<D))-echronos/ \
		-I$(<D) \
		-D${COMPILER} -o ${@} ${<}

$(BUILD_DIR)/%.elf:
	$(BUILD_PRINT)
	${LD} \
		  ${^} 								    \
		  $(LIB_BUILD)/libusb.a                 \
		  $(LIB_BUILD)/libdriver.a				\
		  $(LIB_BUILD)/libsensor.a				\
		  '${LIBM}' '${LIBC}' '${LIBGCC}'       \
		  -T $(ECHRONOS_BUILD)/$(*F)-echronos/default.ld       \
		  -o ${@}
	$(MODULE_PRINT)

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
