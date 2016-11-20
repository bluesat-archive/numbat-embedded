
LIB_BUILD = libs/build

BUILD_PRINT = @echo -e "\e[1;32mBuilding $< -> $@\e[0m"
CLEAN_PRINT = @echo -e "\e[1;32mCleaning -> $@\e[0m"

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
	cp $(ECHRONOS_BUILD)/system $(LIB_BUILD)/$(ECHRONOS_LIB)
	rm -r out

echronos_lib_clean:
	$(CLEAN_PRINT)
	rm -r $(ECHRONOS_BUILD)
	rm $(LIB_BUILD)/$(ECHRONOS_LIB)

TI_LIBS = libs/ti
TI_LIB_DIRS = $(TI_LIBS)/driverlib \
			  $(TI_LIBS)/sensorlib \
			  $(TI_LIBS)/usblib

ti_libs::
	$(BUILD_PRINT)
	@for i in ${TI_LIB_DIRS};           \
	 do                                 \
	     if [ -f $${i}/Makefile ];      \
	     then                           \
	         make -C $${i} || exit $$?; \
			 cp $${i}/gcc/*.a $(LIB_BUILD); \
	     fi;                            \
	 done

ti_libs_clean::
	$(CLEAN_PRINT)
	@for i in ${TI_LIB_DIRS};           \
	 do                                 \
	     if [ -f $${i}/Makefile ];      \
	     then                           \
	         make -C $${i} clean;       \
	     fi;                            \
	 done
	rm $(LIB_BUILD)/*.a

all: echronos_lib ti_libs

clean: echronos_lib_clean ti_libs_clean
