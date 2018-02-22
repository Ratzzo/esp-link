OUTDIR=out/

COBJECTS=$(CSOURCES:%.c=%.o)
REFS=$(CSOURCES:%.c=$(OUTDIR)%.rel)

.PHONY: all clean flash dumpram viewram unlock editram

DEVICE?=STM8S103

Build: all

all: $(OBJECT)

cleanBuild: clean


clean:
	rm -f *.o
	rm -f $(OUTDIR)*
	rm -f *.bin
	@bash -c "$(foreach source, $(LIBS), rm -f $(basename $(source)).o;)"

Flash: flash

flash: $(OBJECT)
	@stm8flash -c$(PROG) -p$(PART) -w $(OUTDIR)$(OBJECT); if [ $$? == 0 ]; then echo Device flashed successfully.; else echo -e \\nError flashing device.; fi;

#echo fools the makefile into thinking that the .o has been generated, sdcc generates its own files
.c.o:
	$(SDCC) -D $(DEVICE)=1 -c $(INCLUDEDIRS) -lstm8 -mstm8 $< -o $(OUTDIR)$(notdir $@)
	@echo > $(basename $@).o
#	@cp $(OUTDIR)$(notdir $(basename $@)).rel $(basename $@).o

unlock:
	stm8flash -c$(PROG) -p$(PART) -sopt -w ../lib/opt.good

dumpflash:
	stm8flash -c$(PROG) -p$(PART) -sflash -r eeprom.bin
	cp eeprom.bin eeprom-$(shell date +%s).bin

#ram stuff
dumpram:
	stm8flash -c$(PROG) -p$(PART) -n -sram -r ram.bin
	cp ram.bin ram-$(shell date +%s).bin
	
viewram: dumpram
	@okteta ram.bin 2> /dev/null 1> /dev/null &
editram: dumpram
	@okteta ram.bin 2> /dev/null 1> /dev/null
	stm8flash -c$(PROG) -p$(PART) -n -sram -w ram.bin

#eeprom stuff
dumpeeprom:
	stm8flash -c$(PROG) -p$(PART) -seeprom -r eeprom.bin
	cp eeprom.bin eeprom-$(shell date +%s).bin
vieweeprom: dumpeeprom
	@okteta eeprom.bin 2> /dev/null 1> /dev/null &
	
editeeprom: dumpeeprom
	@okteta eeprom.bin 2> /dev/null 1> /dev/null
	stm8flash -c$(PROG) -p$(PART) -seeprom -w eeprom.bin
	stm8flash -c$(PROG) -p$(PART) -seeprom -r eeprom.
	
debug: $(OBJECT)-debug

$(OBJECT): $(COBJECTS)
	@$(foreach source, $(COBJECTS), if [ ! -f $(OUTDIR)$(basename $(notdir $(source))).rel ]; then rm $(source); make $(source); fi;)
	$(SDCC) -D $(DEVICE)=1 -o $(OUTDIR)$(OBJECT) $(INCLUDEDIRS)  -lstm8 -mstm8 --out-fmt-ihx $(CFLAGS) $(LDFLAGS) $(addprefix $(OUTDIR), $(notdir $(REFS)))
	@objcopy -I ihex $(OUTDIR)$(OBJECT) -O binary $(OUTDIR)out.bin
	@echo -e "\n\n\n\n\n"
	@ls -l $(OUTDIR)out.bin
	@echo -e "\n\n\n\n\n"

#not working, yet
$(OBJECT)-debug: $(COBJECTS)
	$(SDCC) -D $(DEVICE)=1 -o $(OUTDIR)$(OBJECT) $(INCLUDEDIRS)  -lstm8 -mstm8 --out-fmt-ihx $(CFLAGS) $(LDFLAGS) $(addprefix $(OUTDIR), $(notdir $(REFS)))
	@objcopy -I ihex $(OUTDIR)$(OBJECT) -O binary $(OUTDIR)out.bin
	@echo -e "\n\n\n\n\n"
	@ls -l $(OUTDIR)out.bin
	@echo -e "\n\n\n\n\n"
	#start openocd
	#../inc/openocd &
	#../inc/gdb $(OUTDIR)debug.elf
	#killall openocd
