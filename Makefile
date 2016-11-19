
LIB_BUILD = libs/build

BUILD_PRINT = @echo -e "\e[1;32mBuilding $< -> $@\e[0m"

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

all: echronos_lib
