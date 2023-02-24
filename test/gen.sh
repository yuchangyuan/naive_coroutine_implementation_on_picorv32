#!/bin/sh

T=$1

PREFIX=riscv32-none-elf
CC=$PREFIX-gcc
OBJDUMP=$PREFIX-objdump
OBJCOPY=$PREFIX-objcopy

RUNCSOP=../run_c_snippet_on_picorv32

# build elf
$CC -march=rv32imdc -I../src -Os -c ${T}.c ../src/nc.c $RUNCSOP/sw/init.c $RUNCSOP/sw/start.S
$CC -march=rv32imdc -nostartfiles ${T}.o init.o start.o nc.o -lm -lgcc -Wl,-T,$RUNCSOP/sw/s3.lds -o ${T}.elf

# dump list
$OBJDUMP -D ${T}.elf > ${T}.list

# make rom
$OBJCOPY -j .init -j .text -j .rodata -O binary ${T}.elf ${T}.rom0.bin
$OBJCOPY -j .data -O binary ${T}.elf ${T}.rom1.bin

gcc -o rom_gen $RUNCSOP/hw/rom_gen.c

./rom_gen rom0 < ${T}.rom0.bin > ${T}.rom0.v
./rom_gen rom1 < ${T}.rom1.bin > ${T}.rom1.v

# generate sim
iverilog ${T}.rom0.v \
         ${T}.rom1.v \
         $RUNCSOP/hw/ram.v \
         $RUNCSOP/hw/top.v \
         $RUNCSOP/hw/tb.v \
         $RUNCSOP/picorv32/picorv32.v \
         -o ${T}.sim

# do sim
./${T}.sim
