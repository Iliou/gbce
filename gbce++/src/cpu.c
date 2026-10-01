#include <stdio.h>

#include "cpu.h"

byte *map_address(motherboard *mb, a16_t address, io_operation mode)
{
    printf("Mapping address %x\n", address);
    switch (address) {
    case 0x0000 ... 0x00FF: // Boot/cartridge ROM
    case 0x0200 ... 0x08FF: // Boot/cartridge ROM
        if (mb->hw_regs.BOOT_OFF == 0b0) // Boot ROM intercepts address
            return &mb->boot_rom[address];
        [[fallthrough]];
    case 0x0100 ... 0x0199: // cartridge ROM
    case 0x0900 ... 0x7FFF: // cartridge ROM
    case 0xA000 ... 0xBFFF: // cartridge RAM
        return mb->cart.mapper(&mb->cart, address, mode); //MBC
    case 0x8000 ... 0x9FFF:
        return &mb->cpu.vram[address - 0x8000 + mb->hw_regs.VBK * VRAM / 2]; //VRAM bank 0-1
    case 0xC000 ... 0xCFFF:
        return &mb->wram[address - 0xC000]; // WRAM bank 0
    case 0xD000 ... 0xDFFF:
        return &mb->wram[address - 0xC000 + (mb->hw_regs.SVBK == 0 ? 1 : mb->hw_regs.SVBK) * WRAM / 8]; // WRAM bank 1-7
    case 0xE000 ... 0xFDFF:
        fprintf(stderr, "Error: trying to access forbidden Echo RAM\n");
        return NULL;
    case 0xFE00 ... 0xFE9F:
        return &mb->cpu.ppu.oam[address - 0xFE00];
    case 0xFEA0 ... 0xFEFF:
        fprintf(stderr, "Error: trying to read to restriced area\n");
        return NULL; // Prohibited area
    case 0xFF00 ... 0xFF7F:
        return &mb->hw_regs.regs[address - 0xFF00];
    case 0xFF80 ... 0xFFFE:
        return &mb->cpu.hram[address - 0xFF80]; // HRAM
    case 0xFFFF:
        return &mb->cpu.regs.IE;
    }
    return NULL;
}

void load_next_instruction(motherboard *mb)
{
    mb->cpu.regs.IR = *map_address(mb, mb->cpu.regs.PC, READ);
    printf("Next instruction: PC = %X, IR = %X\n", mb->cpu.regs.PC, mb->cpu.regs.IR);
    mb->cpu.regs.PC++;
}

void run()
{
    return;
}
