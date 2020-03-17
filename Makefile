
# Directories
SRC_DIR ?= $(CURDIR)/src
OUT_DIR ?= $(CURDIR)/out

# Sub-directories
BOOT_DIR ?= $(SRC_DIR)/boot
ARCH_DIR ?= $(SRC_DIR)/arch
INC_DIR  ?= $(SRC_DIR)/include
SCH_DIR  ?= $(SRC_DIR)/scheduler
INT_DIR  ?= $(SRC_DIR)/interrupts
MEM_DIR  ?= $(SRC_DIR)/mem
UTI_DIR  ?= $(SRC_DIR)/util
DRV_DIR  ?= $(SRC_DIR)/drivers

# Flags
CFLAGS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra
LFLAGS = -ffreestanding -O2 -nostdlib

# Qemu
QFLAGS = -cdrom
QEMU = /usr/bin/qemu-system-i386

# Default
.PHONY: all
all:

		# Make out directory
		mkdir -p $(SRC_DIR)
		mkdir -p $(OUT_DIR)
		$$TARGET-as $(BOOT_DIR)/boot.s -o $(OUT_DIR)/boot.o

		# Compile objects
		$$TARGET-gcc -I $(INC_DIR) -c $(SRC_DIR)/kernel.c -o $(OUT_DIR)/kernel.o $(CFLAGS)
		$$TARGET-gcc -I $(INC_DIR) -c $(ARCH_DIR)/descriptors.c -o $(OUT_DIR)/descriptors.o $(CFLAGS)

		$$TARGET-gcc -I $(INC_DIR) -c $(UTI_DIR)/display.c -o $(OUT_DIR)/display.o $(CFLAGS)
		$$TARGET-gcc -I $(INC_DIR) -c $(UTI_DIR)/utility.c -o $(OUT_DIR)/utility.o $(CFLAGS)

		$$TARGET-gcc -I $(INC_DIR) -c $(INT_DIR)/interrupts.c -o $(OUT_DIR)/interrupts.o $(CFLAGS)
		$$TARGET-gcc -I $(INC_DIR) -c $(INT_DIR)/flush.s -o $(OUT_DIR)/flush.o $(CFLAGS)
		$$TARGET-gcc -I $(INC_DIR) -c $(INT_DIR)/isr.s -o $(OUT_DIR)/isr.o $(CFLAGS)

		$$TARGET-gcc -I $(INC_DIR) -c $(DRV_DIR)/keyboard.c -o $(OUT_DIR)/keyboard.o $(CFLAGS)
		$$TARGET-gcc -I $(INC_DIR) -c $(DRV_DIR)/pic.c -o $(OUT_DIR)/pic.o $(CFLAGS)
		$$TARGET-gcc -I $(INC_DIR) -c $(DRV_DIR)/pit.c -o $(OUT_DIR)/pit.o $(CFLAGS)

		$$TARGET-gcc -I $(INC_DIR) -c $(MEM_DIR)/paging.c -o $(OUT_DIR)/paging.o $(CFLAGS)
		$$TARGET-gcc -I $(INC_DIR) -c $(MEM_DIR)/heap.c -o $(OUT_DIR)/heap.o $(CFLAGS)
		$$TARGET-gcc -I $(INC_DIR) -c $(MEM_DIR)/heap_table.c -o $(OUT_DIR)/heap_table.o $(CFLAGS)
		$$TARGET-gcc -I $(INC_DIR) -c $(MEM_DIR)/frame.c -o $(OUT_DIR)/frame.o $(CFLAGS)

		$$TARGET-gcc -I $(INC_DIR) -c $(SCH_DIR)/scheduler.c -o $(OUT_DIR)/scheduler.o $(CFLAGS)
		$$TARGET-gcc -I $(INC_DIR) -c $(SCH_DIR)/thread.c -o $(OUT_DIR)/thread.o $(CFLAGS)

		# Link objects
		$$TARGET-gcc -T $(BOOT_DIR)/linker.ld -o $(BOOT_DIR)/benos.bin $(LFLAGS) $(OUT_DIR)/boot.o \
			$(OUT_DIR)/kernel.o $(OUT_DIR)/display.o $(OUT_DIR)/descriptors.o $(OUT_DIR)/flush.o \
			$(OUT_DIR)/utility.o $(OUT_DIR)/isr.o $(OUT_DIR)/interrupts.o $(OUT_DIR)/pic.o \
			$(OUT_DIR)/pit.o $(OUT_DIR)/keyboard.o $(OUT_DIR)/paging.o $(OUT_DIR)/heap.o \
            $(OUT_DIR)/frame.o $(OUT_DIR)/heap_table.o $(OUT_DIR)/scheduler.o \
			$(OUT_DIR)/thread.o -lgcc

		# Build the bootable image
		mkdir -p dir/boot/grub
		cp $(BOOT_DIR)/benos.bin dir/boot/benos.bin
		cp grub.cfg dir/boot/grub/grub.cfg
		grub-mkrescue -o benos.iso dir


.PHONY: test
test:
		$(QEMU) $(QFLAGS) benos.iso

.PHONY: clean
clean:
		-rm $(OUT_DIR)/*.o
