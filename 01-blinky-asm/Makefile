ARCHFLAGS=-march=rv64g -mabi=lp64

all: main.o ../main.ld
	riscv-none-elf-ld -melf64lriscv -T ../main.ld -o main.elf main.o
	riscv-none-elf-objcopy -O binary main.elf tmp.bin
	python3 ../gen_checksum.py tmp.bin main.bin

run:
	xfel ddr f133 && xfel write 0x40000000 main.bin && xfel exec 0x40000000

clean:
	rm -rf main.bin main.o main.elf tmp.bin

%.o: %.s
	riscv-none-elf-as $(ARCHFLAGS) -o $@ $<

.PHONY: clean run
