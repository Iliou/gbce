#include <stdio.h>

#include "cpu.h"

byte read_address(motherboard const *mb, a16_t address)
{
    fprintf(stderr, "Try to read data at address 0x%04x\n", address);
    switch (address) {
    case 0x0000 ... 0x00ff: // Boot/cartridge ROM
    case 0x0200 ... 0x08ff: // Boot/cartridge ROM
        if (mb->hw_regs.BOOT_OFF == 0b0) // Boot ROM intercepts address
            return mb->boot_rom[address];
        [[fallthrough]];
    case 0x0100 ... 0x01ff: // cartridge ROM
    case 0x0900 ... 0x7fff: // cartridge ROM
    case 0xa000 ... 0xbfff: // cartridge RAM
        return mb->cart->mapper(mb->cart, address, READ, 0); //MBC
    case 0x8000 ... 0x9fff:
        return mb->cpu.vram[(address & 0x1fff) + mb->hw_regs.VBK * VRAM / 2]; //VRAM bank 0-1
    case 0xc000 ... 0xcfff:
        return mb->wram[address & 0xfff]; // WRAM bank 0
    case 0xd000 ... 0xdfff:
        return mb->wram[(address & 0xfff) + (mb->hw_regs.SVBK == 0 ? 1 : mb->hw_regs.SVBK) * WRAM / 8]; // WRAM bank 1-7
    case 0xe000 ... 0xfdff:
        fprintf(stderr, "Error: trying to access forbidden Echo RAM\n");
        return -1;
    case 0xfe00 ... 0xfe9f:
        return mb->cpu.ppu.oam[address & 0xff];
    case 0xfea0 ... 0xfeff:
        fprintf(stderr, "Error: trying to read to restricted area\n");
        return -1; // Prohibited area
    case 0xff00 ... 0xff7f:
        return mb->hw_regs.regs[address & 0xff];
    case 0xff80 ... 0xfffe:
        return mb->cpu.hram[address & 0x7f]; // HRAM
    case 0xffff:
        return mb->cpu.regs.IE;
    }
    fprintf(stderr, "Bus addressing error: this should not happen\n");
    return -1;
}

void write_address(motherboard *mb, a16_t address, byte data)
{
    fprintf(stderr, "Try to write data 0x%02x at address 0x%04x\n", data, address);
    switch (address) {
    case 0x0000 ... 0x00ff: // Boot/cartridge ROM
    case 0x0200 ... 0x08ff: // Boot/cartridge ROM
        if (mb->hw_regs.BOOT_OFF == 0b0) { // Boot ROM intercepts address
            mb->boot_rom[address] = data;
            break;
        }
        [[fallthrough]];
    case 0x0100 ... 0x01ff: // cartridge ROM
    case 0x0900 ... 0x7fff: // cartridge ROM
    case 0xa000 ... 0xbfff: // cartridge RAM
        mb->cart->mapper(mb->cart, address, WRITE, data); //MBC
        break;
    case 0x8000 ... 0x9fff:
        mb->cpu.vram[(address & 0x1fff) + mb->hw_regs.VBK * VRAM / 2] = data; //VRAM bank 0-1
        break;
    case 0xc000 ... 0xcfff:
        mb->wram[address & 0xfff] = data; // WRAM bank 0
        break;
    case 0xd000 ... 0xdfff:
        mb->wram[(address & 0xfff) + (mb->hw_regs.SVBK == 0 ? 1 : mb->hw_regs.SVBK) * WRAM / 8] = data; // WRAM bank 1-7
        break;
    case 0xe000 ... 0xfdff:
        fprintf(stderr, "Error: trying to write forbidden Echo RAM\n");
        break;
    case 0xfe00 ... 0xfe9f:
        mb->cpu.ppu.oam[address & 0xff] = data;
        break;
    case 0xfea0 ... 0xfeff:
        fprintf(stderr, "Error: trying to write to restricted area\n"); // Prohibited area
        break;
    case 0xff00 ... 0xff7f:
        mb->hw_regs.regs[address & 0xff] = data;
        break;
    case 0xff80 ... 0xfffe:
        mb->cpu.hram[address & 0x7f] = data; // HRAM
        break;
    case 0xffff:
        mb->cpu.regs.IE = data;
        break;
    }
}

void load_next_instruction(motherboard *mb)
{
    mb->cpu.regs.IR = read_address(mb, mb->cpu.regs.PC);
    fprintf(stderr, "Next instruction: PC = %X, IR = %X\n", mb->cpu.regs.PC, mb->cpu.regs.IR);
    mb->cpu.regs.PC++;
}

void run()
{
    return;
}
