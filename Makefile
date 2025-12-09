OBJ=boot.o kernel.o solve.o

run: kernel.iso
	qemu-system-i386 -cdrom $<

kernel.iso: kernel.bin grub.cfg
	mkdir -p isodir/boot/grub
	cp $< isodir/boot
	cp grub.cfg isodir/boot/grub
	grub-mkrescue -o $@ isodir

boot.o: boot.s
	i686-elf-as $< -o $@

%.o: %.c
	i686-elf-gcc -c $< -o $@ -std=gnu99 -ffreestanding -O2 -Wall -Wextra

kernel.bin: linker.ld $(OBJ)
	i686-elf-gcc -T $< -o $@ -ffreestanding -O2 -nostdlib $(OBJ) -lgcc

clean:
	rm -rf $(OBJ) kernel.bin kernel.iso isodir
