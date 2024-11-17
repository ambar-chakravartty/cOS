CC = clang
LD = lld-link
CFLAGS=-target x86_64-unknown-windows -ffreestanding -fshort-wchar -mno-red-zone -I/usr/include/efi -I/usr/include/efi/x86_64 -I/usr/include/efi/protocol

LDFLAGS=-target x86_64-unknown-windows -nostdlib -Wl,-entry:efi_main -Wl,-subsystem:efi_application -fuse-ld=lld-link



BOOTLOADER = BOOTX64.EFI

all: $(BOOTLOADER)

$(BOOTLOADER): bootloader.c
	$(CC) $(CFLAGS) -c bootloader.c -o bootloader.o
	clang $(CFLAGS) -c -o data.o data.c
	clang $(LDFLAGS) bootloader.o data.o -o $(BOOTLOADER)

clean:
	rm -f *.o *.EFI
