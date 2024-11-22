ARCHFLAGS=-march=rv64g -mabi=lp64

all: startup.o main.o ../main.ld
	riscv-none-elf-g++ $(ARCHFLAGS) -Wl,-T ../main.ld -o main.elf startup.o main.o -nostartfiles -nostdlib
	riscv-none-elf-objdump -Drtx main.elf >main.asm
	riscv-none-elf-objcopy -O binary main.elf tmp.bin
	python3 ../gen_checksum.py tmp.bin main.bin

run:
	xfel ddr f133 && xfel write 0x40000000 main.bin && xfel exec 0x40000000

clean:
	rm -rf *.bin *.o *.elf

%.o: %.cpp
	riscv-none-elf-g++ -c $(ARCHFLAGS) -fno-exceptions -Os -o $@ $<
	riscv-none-elf-objdump -Drtx $@ >$@.asm

%.o: %.s
	riscv-none-elf-as $(ARCHFLAGS) -o $@ $<

.PHONY: clean run
