# Directories
SRC_DIR ?= $(CURDIR)/src
OUT_DIR ?= $(CURDIR)/src/out
CFLAGS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra
LFLAGS = -ffreestanding -O2 -nostdlib

# Default
.PHONY: all
all:

		# Make out directory
		mkdir -p $(SRC_DIR)
		mkdir -p $(OUT_DIR)
		$$TARGET-as $(SRC_DIR)/boot.s -o $(OUT_DIR)/boot.o

		# Compile objects
		$$TARGET-gcc -c $(SRC_DIR)/kernel.c -o $(OUT_DIR)/kernel.o $(CFLAGS)
		$$TARGET-gcc -c $(SRC_DIR)/display.c -o $(OUT_DIR)/display.o $(CFLAGS)
		$$TARGET-gcc -c $(SRC_DIR)/descriptors.c -o $(OUT_DIR)/descriptors.o $(CFLAGS)
		$$TARGET-gcc -c $(SRC_DIR)/utility.c -o $(OUT_DIR)/utility.o $(CFLAGS)
		$$TARGET-gcc -c $(SRC_DIR)/interrupts.c -o $(OUT_DIR)/interrupts.o $(CFLAGS)
		$$TARGET-gcc -c $(SRC_DIR)/keyboard.c -o $(OUT_DIR)/keyboard.o $(CFLAGS)
		$$TARGET-gcc -c $(SRC_DIR)/pic.c -o $(OUT_DIR)/pic.o $(CFLAGS)
		$$TARGET-gcc -c $(SRC_DIR)/pit.c -o $(OUT_DIR)/pit.o $(CFLAGS)
		$$TARGET-gcc -c $(SRC_DIR)/flush.s -o $(OUT_DIR)/flush.o $(CFLAGS)
		$$TARGET-gcc -c $(SRC_DIR)/isr.s -o $(OUT_DIR)/isr.o $(CFLAGS)

		# Link objects
		$$TARGET-gcc -T $(SRC_DIR)/linker.ld -o benos.bin $(LFLAGS) $(OUT_DIR)/boot.o \
			$(OUT_DIR)/kernel.o $(OUT_DIR)/display.o $(OUT_DIR)/descriptors.o $(OUT_DIR)/flush.o \
			$(OUT_DIR)/utility.o $(OUT_DIR)/isr.o $(OUT_DIR)/interrupts.o $(OUT_DIR)/pic.o \
			$(OUT_DIR)/pit.o $(OUT_DIR)/keyboard.o -lgcc

		# Build the bootable image
		mkdir -p dir/boot/grub
		cp benos.bin dir/boot/benos.bin
		cp grub.cfg dir/boot/grub/grub.cfg
		grub-mkrescue -o benos.iso dir
		

