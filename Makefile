LIB_BUILD = libs/build

BUILD_PRINT = @echo -e "\e[1;32mBuilding $< -> $@\e[0m"
CLEAN_PRINT = @echo -e "\e[1;32mCleaning -> $@\e[0m"
MODULE_PRINT = @echo -e "\e[1;32mSuccessfully built module -> $@\e[0m"

# BEGIN ECHRONOS BUILD MACHINERY

SYSTEM_NAME = numbat-system
PRJ_TOOL = echronos/bin/prj.sh
ECHRONOS_BUILD = echronos/build
ECHRONOS_LIB = echronos-kochab.a

echronos_lib: $(SYSTEM_NAME).prx
	$(BUILD_PRINT)
	$(PRJ_TOOL) \
		--no-project \
		--search-path echronos/share/packages \
		--search-path . \
		--output $(ECHRONOS_BUILD) \
		build $(SYSTEM_NAME)
	mkdir -p $(LIB_BUILD)
	cp $(ECHRONOS_BUILD)/system.a $(LIB_BUILD)/$(ECHRONOS_LIB)
	rm -r out

echronos_lib_clean:
	$(CLEAN_PRINT)
	rm -r -f out
	rm -r -f $(ECHRONOS_BUILD)
	rm -f $(LIB_BUILD)/$(ECHRONOS_LIB)

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
IPATH+=$(ECHRONOS_BUILD)
IPATH+=$(SRC)

build_dir:
	mkdir -p $(BUILD_DIR)

# NUMBAT MODULE SOURCES BEGIN HERE

# Add an object target for each source file that you
# wish to compile. TODO: Perhaps streamline this.
# It is easy to have more modules with shared code
# by making new linker lines with different elf names.
# HOWEVER need to support multiple .prx files as well (TODO)

$(BUILD_DIR)/main.o: $(SRC)/main.c build_dir
	$(BUILD_PRINT)
	${CC} ${CFLAGS} -D${COMPILER} -o ${@} ${<}


$(BUILD_DIR)/numbat_module.elf: build/main.o
	$(BUILD_PRINT)
	${LD} \
		  $(BUILD_DIR)/main.o					\
		  $(LIB_BUILD)/libusb.a                 \
		  $(LIB_BUILD)/libdriver.a				\
		  $(LIB_BUILD)/libsensor.a				\
		  $(LIB_BUILD)/$(ECHRONOS_LIB)			\
		  '${LIBM}' '${LIBC}' '${LIBGCC}'       \
		  -T $(ECHRONOS_BUILD)/default.ld       \
		  -o ${@}
	$(MODULE_PRINT)

# NUMBAT MODULE SOURCES END HERE
modules_clean:
	$(CLEAN_PRINT)
	rm $(BUILD_DIR)/*


.DEFAULT_GOAL := all
all: echronos_lib ti_libs build/numbat_module.elf

clean: modules_clean echronos_lib_clean ti_libs_clean
