#include <stdio.h>

#include "cpu.h"

void load_next_instruction(motherboard *mb)
{
    mb->cpu.regs.IR = *map_address(mb, mb->cpu.regs.PC, READ);
    fprintf(stderr, "Instruction: PC = %X, IR = %X\n", mb->cpu.regs.PC, mb->cpu.regs.IR);
    mb->cpu.regs.PC++;
}
