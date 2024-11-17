all:
	clang -target x86_64-unknown-elf -ffreestanding -mno-red-zone -I/usr/include/efi -I/usr/include/efi/x86_64 -I/usr/include/efi/protocol -c kernel.c -o kernel.o
	ld.lld -T linker.ld  -nostdlib -entry:kmain -o kernel.bin kernel.o --oformat binary

clean:
	rm *.o *.bin


# CC = clang
# CFLAGS = -ffreestanding -fno-stack-protector -mno-red-zone -target x86_64-elf -I/usr/include/efi -I/usr/include/efi/x86_64 -I/usr/include/efi/protocol

# KERNEL = kernel.bin

# all: $(KERNEL)

# $(KERNEL): kernel.c
# 	$(CC) $(CFLAGS) -nostdlib -o $(KERNEL) kernel.c

# clean:
# 	rm -f *.bin
