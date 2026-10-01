#include <stdio.h>

#include "cpu.h"

void load_next_instruction(motherboard *mb)
{
    mb->cpu.regs.IR = *map_address(mb, mb->cpu.regs.PC, READ);
    printf("Instruction nb %d: PC = %X, IR = %X\n", i, mb->cpu.regs.PC, mb->cpu.regs.IR);
    mb->cpu.regs.PC++;
}
