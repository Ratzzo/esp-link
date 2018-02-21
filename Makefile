#neutered esp-link Makefile

include environment.mk

# --------------- toolchain configuration ---------------

# Base directory for the compiler. Needs a / at the end.
# Typically you'll install https://github.com/pfalcon/esp-open-sdk
# IMPORTANT: use esp-open-sdk `make STANDALONE=n`: the SDK bundled with esp-open-sdk will *not* work!
XTENSA_TOOLS_ROOT ?= $(abspath ../xtensa-lx106-elf/bin)/

# Firmware version 
# WARNING: if you change this expect to make code adjustments elsewhere, don't expect
# that esp-link will magically work with a different version of the SDK!!!
SDK_VERS ?= esp_iot_sdk_v2.1.0

# Try to find the firmware manually extracted, e.g. after downloading from Espressif's BBS,
# http://bbs.espressif.com/viewforum.php?f=46
# USING THE SDK BUNDLED WITH ESP-OPEN-SDK WILL NOT WORK!!!
SDK_BASE ?= $(wildcard ../$(SDK_VERS))

# If the firmware isn't there, see whether it got downloaded as part of esp-open-sdk
# This used to work at some point, but is not supported, uncomment if you feel lucky ;-)
#ifeq ($(SDK_BASE),)
#SDK_BASE := $(wildcard $(XTENSA_TOOLS_ROOT)/../../$(SDK_VERS))
#endif

# Clean up SDK path
SDK_BASE := $(abspath $(SDK_BASE))
$(info SDK     is $(SDK_BASE))

# Path to bootloader file
BOOTFILE	?= $(SDK_BASE)/../ESP8266_NONOS_SDK_V2.0.0_16_08_10/bin/boot_v1.6.bin

# Esptool.py path and port, only used for 1-time serial flashing
# Typically you'll use https://github.com/themadinventor/esptool
# Windows users use the com port i.e: ESPPORT ?= com3
ESPTOOL		?= $(abspath ../esp-open-sdk/esptool/esptool.py)
ESPPORT		?= /dev/ttyUSB0
ESPBAUD		?= 921600

# --------------- chipset configuration   ---------------

# Pick your flash size: "512KB", "1MB", or "4MB"
# beware of sdk/tools/gen_appbin.py overriding these settings!
FLASH_SIZE ?= 4MB

ESP_FLASH_MAX       ?= 4000000  # max bin file

ifeq ("$(FLASH_SIZE)","512KB")
# Winbond 25Q40 512KB flash, typ for esp-01 thru esp-11
ESP_SPI_SIZE        ?= 0       # 0->512KB (256KB+256KB)
ESP_FLASH_MODE      ?= 0       # 0->QIO
ESP_FLASH_FREQ_DIV  ?= 0       # 0->40Mhz
ET_FS               ?= 4m      # 4Mbit flash size in esptool flash command
ET_FF               ?= 40m     # 40Mhz flash speed in esptool flash command
ET_BLANK            ?= 0x7E000 # where to flash blank.bin to erase wireless settings

else ifeq ("$(FLASH_SIZE)","1MB")
# ESP-01E
ESP_SPI_SIZE        ?= 2       # 2->1MB (512KB+512KB)
ESP_FLASH_MODE      ?= 0       # 0->QIO
ESP_FLASH_FREQ_DIV  ?= 15      # 15->80MHz
ET_FS               ?= 8m      # 8Mbit flash size in esptool flash command
ET_FF               ?= 80m     # 80Mhz flash speed in esptool flash command
ET_BLANK            ?= 0xFE000 # where to flash blank.bin to erase wireless settings

else ifeq ("$(FLASH_SIZE)","2MB")
# Manuf 0xA1 Chip 0x4015 found on wroom-02 modules
# Here we're using two partitions of approx 0.5MB because that's what's easily available in terms
# of linker scripts in the SDK. Ideally we'd use two partitions of approx 1MB, the remaining 2MB
# cannot be used for code (esp8266 limitation).
ESP_SPI_SIZE        ?= 4       # 6->4MB (1MB+1MB) or 4->4MB (512KB+512KB)
ESP_FLASH_MODE      ?= 0       # 0->QIO, 2->DIO
ESP_FLASH_FREQ_DIV  ?= 15      # 15->80Mhz
ET_FS               ?= 16m     # 16Mbit flash size in esptool flash command
ET_FF               ?= 80m     # 80Mhz flash speed in esptool flash command
ET_BLANK            ?= 0x1FE000 # where to flash blank.bin to erase wireless settings

else
# Winbond 25Q32 4MB flash, typ for esp-12
# Here we're using two partitions of approx 0.5MB because that's what's easily available in terms
# of linker scripts in the SDK. Id/eally we'd use two partitions of approx 1MB, the remaining 2MB
# cannot be used for code (esp8266 limitation).
ESP_SPI_SIZE        ?= 4       # 6->4MB (1MB+1MB) or 4->4MB (512KB+512KB)
ESP_FLASH_MODE      ?= 0       # 0->QIO, 2->DIO
ESP_FLASH_FREQ_DIV  ?= 15      # 15->80Mhz
ET_FS               ?= 32m     # 32Mbit flash size in esptool flash command
ET_FF               ?= 80m     # 80Mhz flash speed in esptool flash command
ET_BLANK            ?= 0x3FE000 # where to flash blank.bin to erase wireless settings
endif

# --------------- esp-link version        ---------------

# Version-fu :-) This code assumes that a new maj.minor is started using a "vN.M.0" tag on master
# and that thereafter the desired patchlevel number is just the number of commits since the tag.
#
# Get the current branch name if not using travis
TRAVIS_BRANCH?=$(shell git symbolic-ref --short HEAD --quiet)
# Use git describe to get the latest version tag, commits since then, sha and dirty flag, this
# results is something like "v1.2.0-13-ab6cedf-dirty"
NO_TAG ?= "no-tag"
VERSION := $(shell (git describe --tags --match 'v*.0' --long --dirty || echo $(NO_TAG)) | sed -re 's/(\.0)?-/./')
# If not on master then insert the branch name
ifneq ($(TRAVIS_BRANCH),master)
ifneq ($(findstring V%,$(TRAVIS_BRANCH)),)
VERSION := $(shell echo $(VERSION) | sed -e 's/-/-$(TRAVIS_BRANCH)-/')
endif
endif
VERSION :=$(VERSION)
$(info VERSION is $(VERSION))

# Output directors to store intermediate compiled files
# relative to the project directory
BUILD_BASE	= build
FW_BASE		= firmware

# name for the target project
TARGET		= httpd

# espressif tool to concatenate sections for OTA upload using bootloader v1.2+
APPGEN_TOOL	?= gen_appbin.py

CFLAGS=

# which modules (subdirectories) of the project to include in compiling
LIBRARIES_DIR 	= libraries
MODULES1	+= deps
MODULES1	+= $(foreach sdir,$(LIBRARIES_DIR),$(wildcard $(sdir)/*))
MODULES2	+= src
MODULES2	+= $(foreach sdir,$(LIBRARIES_DIR),$(wildcard $(sdir)/*))
EXTRA_INCDIR 	= include .

#we'll parse the image format to move iram to where it belongs :)

#address where this will be flashed
APP_USER2_BASE_ADDR = 0x81000 # higher 512kb portion
#memory address after flashing
APP_USER2_BASE_VADDR = (0x40200000+$(APP_USER2_BASE_ADDR))


# libraries used in this project, mainly provided by the SDK
LIBS += c
LIBS += gcc 
LIBS += hal 
LIBS += phy 
LIBS += pp 
LIBS += net80211 
LIBS += wpa 
LIBS += main 
LIBS += lwip_536 
#LIBS += crypto

# compiler flags using during compilation of source files
CFLAGS	+= -Os -ggdb -std=c99 -Wpointer-arith -Wundef -Wall -Wl,-EL -fno-inline-functions \
	-nostdlib -mlongcalls -mtarget-align -mtext-section-literals -ffunction-sections -fdata-sections \
	-D__ESP8266__ -D__ets__ -DICACHE_FLASH -Wno-address -DFIRMWARE_SIZE=$(ESP_FLASH_MAX) \
	-DMCU_RESET_PIN=$(MCU_RESET_PIN) -DMCU_ISP_PIN=$(MCU_ISP_PIN) \
	-DLED_CONN_PIN=$(LED_CONN_PIN) -DLED_SERIAL_PIN=$(LED_SERIAL_PIN) \
	-DAPP_USER2_BASE_VADDR='$(APP_USER2_BASE_VADDR)' -DVERSION="esp-link $(VERSION)"


# linker flags used to generate the main object file
LDFLAGS1		= -nostdlib -Wl,--no-check-sections -u call_user_start -Wl,-static  
LDFLAGS2		= -nostdlib -Wl,--no-check-sections -u call_user_start -Wl,-static
#-Wl,--gc-sections

# linker script used for the above linker step
LD_SCRIPT 	:= build/eagle.esphttpd.v6.ld
LD_SCRIPT1	:= build/eagle.esphttpd1.v6.ld
LD_SCRIPT2	:= build/eagle.esphttpd2.v6.ld

# various paths from the SDK used in this project
SDK_LIBDIR	= lib
SDK_LDDIR	= ld
SDK_INCDIR	= include include/json
SDK_TOOLSDIR	= tools

# select which tools to use as compiler, librarian and linker
CC		:= $(XTENSA_TOOLS_ROOT)xtensa-lx106-elf-gcc
AR		:= $(XTENSA_TOOLS_ROOT)xtensa-lx106-elf-ar
LD		:= $(XTENSA_TOOLS_ROOT)xtensa-lx106-elf-gcc
OBJCP		:= $(XTENSA_TOOLS_ROOT)xtensa-lx106-elf-objcopy
OBJDP		:= $(XTENSA_TOOLS_ROOT)xtensa-lx106-elf-objdump
ELF_SIZE	:= $(XTENSA_TOOLS_ROOT)xtensa-lx106-elf-size

####
SRC1_DIR	:= $(MODULES1)
SRC2_DIR	:= $(MODULES2)
BUILD_DIR	:= $(addprefix $(BUILD_BASE)/,$(MODULES1))
BUILD_DIR	+= $(addprefix $(BUILD_BASE)/,$(MODULES2))

SDK_LIBDIR	:= $(addprefix $(SDK_BASE)/,$(SDK_LIBDIR))
SDK_LDDIR 	:= $(addprefix $(SDK_BASE)/,$(SDK_LDDIR))
SDK_INCDIR	:= $(addprefix -I$(SDK_BASE)/,$(SDK_INCDIR))
SDK_TOOLS	:= $(addprefix $(SDK_BASE)/,$(SDK_TOOLSDIR))
APPGEN_TOOL	:= $(addprefix $(SDK_TOOLS)/,$(APPGEN_TOOL))

SRC1		:= $(foreach sdir,$(SRC1_DIR),$(wildcard $(sdir)/*.c))
SRC2		:= $(foreach sdir,$(SRC2_DIR),$(wildcard $(sdir)/*.c))
OBJ1		:= $(patsubst %.c,$(BUILD_BASE)/%.o,$(SRC1))
OBJ2		:= $(patsubst %.c,$(BUILD_BASE)/%.o,$(SRC2))
LIBS		:= $(addprefix -l,$(LIBS))
APP_AR1		:= $(addprefix $(BUILD_BASE)/,$(TARGET)_app1.a)
APP_AR2		:= $(addprefix $(BUILD_BASE)/,$(TARGET)_app2.a)
USER1_OUT 	:= $(addprefix $(BUILD_BASE)/,$(TARGET).user1.out)
USER2_OUT 	:= $(addprefix $(BUILD_BASE)/,$(TARGET).user2.out)

INCDIR		+= $(addprefix -I,$(SRC1_DIR))
INCDIR		+= $(addprefix -I,$(SRC2_DIR))
EXTRA_INCDIR	:= $(addprefix -I,$(EXTRA_INCDIR))
MODULE_INCDIR	:= $(addsuffix /include,$(INCDIR))

PROVIDER_GENERATOR = ./tools/generateprovider
FWINFO = ./tools/fwinfo

CTOOLS = $(PROVIDER_GENERATOR) $(FWINFO)

V ?= $(VERBOSE)
ifeq ("$(V)","1")
Q :=
vecho := @true
else
Q := @
vecho := @echo
endif

vpath %.c $(SRC1_DIR)
vpath %.c $(SRC2_DIR)

define compile-objects
$1/%.o: %.c
	$(vecho) "CC $$<"
	$(Q)$(CC) $(INCDIR) $(MODULE_INCDIR) $(EXTRA_INCDIR) $(SDK_INCDIR) $(CFLAGS)  -c $$< -o $$@
endef

.PHONY: checkdirs clean

#stuff in "all" gets automatically parallelized.
all: $(CTOOLS) checkdirs $(FW_BASE)/user2.bin

$(CTOOLS):
	gcc $@.c -o $@

$(USER1_OUT): $(OBJ1) $(LD_SCRIPT1)
	$(vecho) "LD $@"
	$(Q) $(LD) -L$(SDK_LIBDIR) -T$(LD_SCRIPT1) $(LDFLAGS1) -Wl,--start-group $(LIBS) $(OBJ1) -Wl,--end-group   -o $@
	
$(USER2_OUT): $(OBJ2) $(LD_SCRIPT2)
	$(vecho) "LD $@"
	$(Q) $(LD) -L$(SDK_LIBDIR) -T$(LD_SCRIPT2) $(LDFLAGS2)  -Wl,--start-group $(OBJ2) -Wl,--end-group -o $@

$(FW_BASE):
	$(vecho) "FW $@"
	$(Q) mkdir -p $@
	echo $(SRC1_DIR)

	
$(FW_BASE)/user1.bin: $(USER1_OUT)
	$(Q) $(OBJCP) --only-section .text -O binary       $(USER1_OUT) eagle.app.v6.text.bin
	$(Q) $(OBJCP) --only-section .data -O binary       $(USER1_OUT) eagle.app.v6.data.bin
	$(Q) $(OBJCP) --only-section .rodata -O binary     $(USER1_OUT) eagle.app.v6.rodata.bin
	$(Q) $(OBJCP) --only-section .irom0.text -O binary $(USER1_OUT) eagle.app.v6.irom0text.bin
	$(Q) $(ELF_SIZE) -A $(USER1_OUT) |grep -v " 0$$" |grep .
	$(Q) COMPILE=gcc PATH=$(XTENSA_TOOLS_ROOT):$(PATH) python2 $(APPGEN_TOOL) $(USER1_OUT) 2 $(ESP_FLASH_MODE) $(ESP_FLASH_FREQ_DIV) $(ESP_SPI_SIZE) 0 > /dev/null
	$(Q) rm -f eagle.app.v6.*.bin
	$(Q) mv eagle.app.flash.bin $@
	@echo "    user1.bin uses $$(stat -c '%s' $@) bytes of" $(ESP_FLASH_MAX) "available"
	$(Q) if [ $$(stat -c '%s' $@) -gt $$(( $(ESP_FLASH_MAX) )) ]; then echo "$@ too big!"; false; fi
	$(Q) touch $@
	$(Q) $(FWINFO) $@
	
	
#we'll allocate the main application on usr2
$(FW_BASE)/user2.bin: $(BUILD_DIR) $(USER2_OUT) $(FW_BASE)/user1.bin
	$(Q) $(ELF_SIZE) -A $(USER2_OUT) |grep -v " 0$$" |grep .
	$(ESPTOOL) elf2image --version=2 $(USER2_OUT)
	$(Q) cp $(BUILD_BASE)/httpd.user2*.bin $@
	@echo "    user2.bin uses $$(stat -c '%s' $@) bytes of" $(ESP_FLASH_MAX) "available"
	$(Q) rm -f eagle.app.v6.*.bin
	$(Q) if [ $$(stat -c '%s' $@) -gt $$(( $(ESP_FLASH_MAX) )) ]; then echo "$@ too big!"; false; fi
	$(Q) touch $(FW_BASE)/user2.bin
	$(Q) $(FWINFO) $@

checkdirs: $(BUILD_DIR)

$(BUILD_DIR):
	$(Q) mkdir -p $@

baseflash: all
	$(Q) $(ESPTOOL) --port $(ESPPORT) --baud $(ESPBAUD) write_flash 0x01000 $(FW_BASE)/user1.bin
	
rfcal:
	$(Q) $(ESPTOOL) --port $(ESPPORT) --baud $(ESPBAUD) write_flash 0x3fc000 bin/esp_init_data_default.bin
	
dumpconfig:
	$(Q) $(ESPTOOL) --port $(ESPPORT) --baud $(ESPBAUD) read_flash 0x3fc000 16384 dump.bin
	
dumpuser2:
	$(Q) $(ESPTOOL) --port $(ESPPORT) --baud $(ESPBAUD) read_flash $(APP_USER2_BASE_ADDR) 16384 firmware/user2dump.bin

	
restoreconfig:
	$(Q) $(ESPTOOL) --port $(ESPPORT) --baud $(ESPBAUD) write_flash 0x3fc000 dump.bin


#flash main program only
flash: $(FW_BASE)/user2.bin depflash
	$(Q) $(ESPTOOL) --port $(ESPPORT) --baud $(ESPBAUD) write_flash -fs $(ET_FS) -ff $(ET_FF) \
	$(APP_USER2_BASE_ADDR) $(FW_BASE)/user2.bin \
	  $(ET_BLANK) $(SDK_BASE)/bin/blank.bin
	$(Q) touch flash

#flash all the dependencies
depflash: $(FW_BASE)/user1.bin
	$(Q) $(ESPTOOL) --port $(ESPPORT) --baud $(ESPBAUD) write_flash -fs $(ET_FS) -ff $(ET_FF) \
	  0x00000 "$(BOOTFILE)" 0x01000 $(FW_BASE)/user1.bin \
	  $(ET_BLANK) $(SDK_BASE)/bin/blank.bin
	$(Q) touch depflash

cleanuser2:
	rm -rf $(OBJ2)

cleanuser1:
	rm -rf $(OBJ1)

disasm1:
	$(OBJDP) -d build/httpd.user1.out > user1.asm

disasm2:
	$(OBJDP) -d build/httpd.user2.out > user2.asm

# edit the loader script to add the espfs section to the end of irom with a 4 byte alignment.
# we also adjust the sizes of the segments 'cause we need more irom0
# for faster compilation we'll shove dependencies on usr1, and actual program on usr2 since I don't really care about OTA
$(LD_SCRIPT1): $(SDK_LDDIR)/eagle.app.v6.new.1024.app1.ld $(CTOOLS)
	$(Q) sed -e '/\.irom\.text/{' -e 'a . = ALIGN (4);' -e 'a *(.espfs)' -e '}'  \
		-e '/^  irom0_0_seg/ s/6B000/7C000/' \
		-e '/^  iram1_0_seg/ s/8000/7000/' \
		$(SDK_LDDIR)/eagle.app.v6.new.1024.app1.ld >$@
#append a fixed usr_init to this
#	$(Q) echo -e "PROVIDE (app_user_init = 0x40281010);" >> $@
	
$(LD_SCRIPT2): $(USER1_OUT) $(SDK_LDDIR)/eagle.app.v6.new.1024.app1.ld $(CTOOLS)
	$(Q) sed -e '/\.irom\.text/{' -e 'a . = ALIGN (4);' -e 'a *(.espfs)' -e '}'  \
		-e '/^  dram0_0_seg/ s/0x3FFE8000/0x3FFF0000/' \
		-e '/^  irom0_0_seg/ s/6B000/7C000/' \
		-e '/^  irom0_0_seg/ s/0x40281010/$(APP_USER2_BASE_ADDR)+0x40200000+0x10/' \
		-e '/^  iram1_0_seg/ s/8000/1000/' \
		-e '/^  iram1_0_seg/ s/0x40100000/0x40107000/' \
		$(SDK_LDDIR)/eagle.app.v6.new.1024.app2.ld >$@
#	echo -e 'SECTIONS								'	>> $@
#	echo -e '{									'	>> $@
#	echo -e '/* placing my named section at given address: */			'	>> $@
#	echo -e '.app_user_init $(APP_USER_INIT_ADDR) :					'	>> $@
#	echo -e '{									'	>> $@
#	echo -e 'KEEP(*(.app_user_init)) /* keep my variable even if not referenced */	'	>> $@
#	echo -e '} > irom0_0_seg :irom0_0_phdr						'	>> $@
#	echo -e '									'	>> $@
#	echo -e '/* other placements follow here... */					'	>> $@
#	echo -e '}									' 	>> $@
#echo $(USER1_OUT)
#$(Q) echo -e "ENTRY(call_user_start);" >> $@
	$(OBJDP) -t $(USER1_OUT) > $(BUILD_BASE)/symbols.txt 
	$(PROVIDER_GENERATOR) $(BUILD_BASE)/symbols.txt >>$@



clean:
	$(Q) rm -rf build
	$(Q) rm -rf $(CTOOLS)
	$(Q) rm -rf firmware/*
	$(Q) rm -rf depflash
	$(Q) rm -rf flash
	
$(foreach bdir,$(BUILD_DIR),$(eval $(call compile-objects,$(bdir))))
#$(foreach bdir,$(BUILD_DIR),$(eval $(call compile-objects,$(bdir))))


 
 
