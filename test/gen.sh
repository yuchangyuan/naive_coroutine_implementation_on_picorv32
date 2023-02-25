#!/bin/sh

T=$1

PREFIX=riscv32-none-elf
CC=$PREFIX-gcc
OBJDUMP=$PREFIX-objdump
OBJCOPY=$PREFIX-objcopy

RUNCSOP=../run_c_snippet_on_picorv32

# build elf

# NOTE:
# 1. below '-march' should be 'rv32imc', picorv32 not support 'D' extension
# 2. default NixOS cross compiler not support 'rv32imc', so here use 'rv32imdc'
# 3. all test code contain will not emmit instruction in 'D' extension, so wrong march is harmless
$CC -march=rv32imdc -I../src -Os -c ${T}.c ../src/nc.c $RUNCSOP/sw/init.c $RUNCSOP/sw/start.S
$CC -march=rv32imdc -nostartfiles ${T}.o init.o start.o nc.o -Wl,-T,$RUNCSOP/sw/s3.lds -o ${T}.elf

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
