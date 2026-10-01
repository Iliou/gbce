#include <stddef.h>

#include "cartridge.h"

byte *no_mapper(struct cartridge *cart, a16_t address, io_operation mode)
{
    return address >= 0xA000 ? &cart->ram[address - 0xA000] : &cart->rom[address];
}

byte *mbc1(struct cartridge *cart, a16_t address, io_operation mode)
{
    // TODO
    switch (address) {
    case 0x0000 ... 0x3FFF:
        return &cart->rom[address];
    }
    return NULL;
}
