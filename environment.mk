#this file defines the environment, assuming the esp open sdk root is the parent directory.

ESP_OPEN_SDK_ROOT=.. #change this to either an absolute path or a relative path

ESPTOOL_BAUD=921600 
OPEN_SDK_BASE=$(shell cd $(ESP_OPEN_SDK_ROOT);pwd;) #get the absolute path of ESP_OPEN_SDK_ROOT
SDK_BASE=$(strip $(OPEN_SDK_BASE))/sdk
ESPTOOL=$(strip $(OPEN_SDK_BASE))/esptool/esptool-cb.py #esptool-cb is a modified esptool where the env #! was changed to python2
export PATH := $(strip $(OPEN_SDK_BASE))/binfixes:$(strip $(OPEN_SDK_BASE))/xtensa-lx106-elf/bin:$(PATH) #shove binary paths into PATH

