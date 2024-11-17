#!/bin/bash

# Create a blank 1.44 MB FAT image
dd if=/dev/zero of=fat.img bs=1k count=1440

# Format the image as FAT12
mformat -i fat.img -f 1440 ::

# Create necessary directories in the image
mmd -i fat.img ::/EFI
mmd -i fat.img ::/EFI/BOOT

# Copy the UEFI bootloader into the image
mcopy -i fat.img BOOTx64.efi ::/EFI/BOOT

# Copy the kernel binary into the root directory of the image
mcopy -i fat.img kernel/kernel.bin ::

echo "Disk image created successfully with BOOTx64.efi and kernel.bin!"
