#include <stdio.h>
#include <stddef.h>

#include "cartridge.h"

byte no_mapper(struct cartridge *cart, a16_t address, io_operation mode, byte data)
{
    byte *dest = address >= 0xA000 ? &cart->ram[address - 0xA000] : &cart->rom[address];

    if (mode == WRITE) {
        *dest = data;
        return 0;
    } else {
        return *dest;
    }
}

byte mbc1(struct cartridge *cart, a16_t address, io_operation mode, byte data)
{
    static struct {
        byte ram_enable:1;
        byte bank1:5;
        byte bank2:2;
        byte bank_mode:1;
    } regs = {false, 1, 0, 0};

    fprintf(stderr, "Mapper MBC1 called\n");
    if (mode == WRITE) {
        switch (address) {
        case 0x0000 ... 0x1fff:
            regs.ram_enable = ((data & 0x0f) == 0xa);
            break;
        case 0x2000 ... 0x3fff:
            regs.bank1 = (data & 0x1f) == 0 ? 1 : (data & 0x1f);
            break;
        case 0x4000 ... 0x5fff:
            regs.bank2 = data & 0x11;
            break;
        case 0x6000 ... 0x7fff:
            regs.bank_mode = data & 1;
            break;
        case 0xa000 ... 0xbfff:
            if (regs.ram_enable)
                cart->ram[((regs.bank_mode * regs.bank2) << 13) | (address & 0x1fff)] = data;
        }
        return 0;
    } else {
        switch (address) {
        case 0x0000 ... 0x3fff:
            //fprintf(stderr, "DEBUG MBC1: accessing address: %hu, bank mode = %d and bank2 = %d. ROM addressed at: %d\n", address, regs.bank_mode, regs.bank2, ((regs.bank_mode * regs.bank2) << 19) | address);
            return cart->rom[((regs.bank_mode * regs.bank2) << 19) | address];
        case 0x4000 ... 0x7fff:
            return cart->rom[((cart->header.rom_size / 5) * (regs.bank2 << 19)) | ((regs.bank1 & ((1 << (cart->header.rom_size + 1)) - 1)) << 14) | (address & 0x3fff)];
        case 0xa000 ... 0xbfff:
            return regs.ram_enable ? cart->ram[((regs.bank_mode * regs.bank2) << 13) | (address & 0x1fff)] : 0xff;
        }
    }
    fprintf(stderr, "Mapper Error: this should not happen\n");
    return -1;
}
