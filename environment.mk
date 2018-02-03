#this file defines the environment, assuming the esp open sdk root is in the parent directory.

ESP_OPEN_SDK_ROOT=.. #change this to either an absolute path 

ESPTOOL_BAUD=921600 
OPEN_SDK_BASE=$(shell cd $(ESP_OPEN_SDK_ROOT);pwd;) #get parent dir absolute path
SDK_BASE=$(strip $(OPEN_SDK_BASE))/sdk
ESPTOOL=$(strip $(OPEN_SDK_BASE))/esptool/esptool-cb.py #esptool-cb is a modified esptool where the env #! was changed to python2
export PATH := $(strip $(OPEN_SDK_BASE))/binfixes:$(strip $(OPEN_SDK_BASE))/xtensa-lx106-elf/bin:$(PATH) #shove binary into PATH

