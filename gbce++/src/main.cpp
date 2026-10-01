#include <iostream>

#include "tools.h"
#include "cpu.h"
#include "cartridge.h"
#include "ins.h"

/*
int checksum(byte const *hdr)
{
    uint8_t checksum = 0;

    for (uint16_t address = 0x0134; address <= 0x014C; address++) {
        checksum = checksum - hdr[address] - 1;
    }
    printf("Checksum: %.2X (Expected: %.2X)\n", checksum, hdr[0x14D]);
    return checksum == hdr[0x14D];
}
*/

void execute(motherboard *mb, cartridge *cart)
{
    for (int i = 0; i < 1000; ++i) {
        printf("Instruction nb %d\n", i);
        load_next_instruction(mb);
        INSTRUCTIONS_PTR[mb->cpu.regs.IR](mb->cpu.regs.IR, mb, &mb->cpu.regs);
        printf("\n");
    }
}

int main(int ac, char **av)
{
    if (ac < 2) {
        std::cerr << "Error: No cartridge" << std::endl;
        return 1;
    }

    Motherboard motherboard("cgb.bin");
    Cartridge cart;

    motherboard.run(cart);

    /* C code
    motherboard mb = {0};
    cartridge cart;

    if (ac < 2) {
        fprintf(stderr, "Error: No cartridge\n");
        return 1;
    }
    init_registers(&mb.cpu.regs);
    if (load_boot_rom(mb.boot_rom, "cgb.bin") != 0)
        return 1;
    if (create_cartridge(av[1], &cart) != 0)
        return 1;
    // DEBUG
    dump_rom_header(&cart.header);
    memcpy(mb.wram + 0x100, cart.raw_hdr, sizeof(cart.raw_hdr));
    execute(&mb, &cart);
    free(cart.rom);
    */
    return 0;
}
