#########################
## https://www.opensourceforu.com/2012/06/gnu-make-in-detail-for-beginners/
#########################

BOOTMNT ?=/media/foer/bootfs

ARMGNU ?=aarch64-linux-gnu

COPS =-Wall -nostdlib -nostartfiles -ffreestanding \
	-Iinclude -mgeneral-regs-only -g -mno-outline-atomics -lgcc

ASMOPS =-Iinclude

BUILD_DIR=build
SRC_DIR=src

all: kernel8.img

debug: kernel8.elf
	gdb-multiarch build/kernel8.elf -ex "target extended-remote localhost:3333"

ocd:
	openocd -f adafruit-ft232h.cfg -f rpi4_jlink.cfg

clean:
	rm -rf $(BUILD_DIR) *.img
	rm -f $(BUILD_DIR)/__debug.txt

$(BUILD_DIR)/%_c.o: $(SRC_DIR)/%.c
	mkdir -p $(@D)
	$(ARMGNU)-gcc $(COPS) -MMD -c $< -o $@

$(BUILD_DIR)/%_s.o: $(SRC_DIR)/%.S
	mkdir -p $(@D)
	$(ARMGNU)-gcc $(COPS) -MMD -c $< -o $@

C_FILES = $(wildcard $(SRC_DIR)/*.c) $(wildcard $(SRC_DIR)/*/*.c) 
ASM_FILES = $(wildcard $(SRC_DIR)/*.S) $(wildcard $(SRC_DIR)/*/*.S)
OBJ_FILES = $(C_FILES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%_c.o)
OBJ_FILES += $(ASM_FILES:$(SRC_DIR)/%.S=$(BUILD_DIR)/%_s.o)
DEP_FILES = $(OBJ_FILES:%.o=%.d)
-include $(DEP_FILES)


kernel8.elf: $(SRC_DIR)/linker.ld $(OBJ_FILES)
	$(ARMGNU)-ld -T $(SRC_DIR)/linker.ld -o $(BUILD_DIR)/kernel8.elf $(OBJ_FILES)
	$(ARMGNU)-objdump -D -S $(BUILD_DIR)/kernel8.elf > $(BUILD_DIR)/__kernel8.debug.txt

kernel8.img: kernel8.elf
	$(ARMGNU)-objcopy $(BUILD_DIR)/kernel8.elf -O binary kernel8.img 
	$(ARMGNU)-objdump -D -S $(BUILD_DIR)/kernel8.elf > $(BUILD_DIR)/__debug.txt
	cp kernel8.img $(BOOTMNT)/
	cp config.txt $(BOOTMNT)/
	sync