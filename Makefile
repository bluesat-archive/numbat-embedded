
LIB_BUILD = libs/build

BUILD_PRINT = @echo -e "\e[1;32mBuilding $< -> $@\e[0m"
CLEAN_PRINT = @echo -e "\e[1;32mCleaning -> $@\e[0m"

# BEGIN ECHRONOS BUILD MACHINERY

SYSTEM_NAME = numbat-system
PRJ_TOOL = echronos/bin/prj.sh
ECHRONOS_BUILD = echronos/build
ECHRONOS_LIB = echronos-kochab.o

echronos_lib: $(SYSTEM_NAME).prx
	$(BUILD_PRINT)
	$(PRJ_TOOL) \
		--no-project \
		--search-path echronos/share/packages \
		--search-path . \
		--output $(ECHRONOS_BUILD) \
		build $(SYSTEM_NAME)
	cp $(ECHRONOS_BUILD)/system $(LIB_BUILD)/$(ECHRONOS_LIB)
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

PART=TM4C123GH6PM
CFLAGSgcc=-DTARGET_IS_TM4C123_RB1
include makedefs
SRC=src
BUILD_DIR=build
IPATH=$(TI_LIBS)
IPATH+=$(ECHRONOS_BUILD)

build_dir:
	mkdir -p $(BUILD_DIR)

# NUMBAT MODULE SOURCES BEGIN HERE

build/main.o: $(SRC)/main.c build_dir
	${CC} ${CFLAGS} -D${COMPILER} -o ${@} ${<}


build/numbat_module.elf: build/main.o
	${LD} -T $(ECHRONOS_BUILD)/default.ld       \
		  $(BUILD_DIR)/main.o					\
		  $(LIB_BUILD)/libusb.a                 \
		  $(LIB_BUILD)/libdriver.a		    	\
		  $(LIB_BUILD)/libsensor.a		    	\
		  $(LIB_BUILD)/$(ECHRONOS_LIB)	    	\
	      '${LIBM}' '${LIBC}' '${LIBGCC}'

# NUMBAT MODULE SOURCES END HERE

.DEFAULT_GOAL := all
all: echronos_lib ti_libs

clean: echronos_lib_clean ti_libs_clean
