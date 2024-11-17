all:
	clang -target x86_64-unknown-elf -ffreestanding -mno-red-zone -c kernel.c -o kernel.o
	ld.lld -T linker.ld -o kernel.bin kernel.o --oformat binary

clean:
	rm *.o *.bin
