#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include "tools.h"
#include "cpu.h"
#include "cartridge.h"
#include "ins.h"

static void dump_regs(registers const *regs)
{
    fprintf(stderr,
            "A = %02x   F = %02x\n"
            "B = %02x   C = %02x\n"
            "D = %02x   E = %02x\n"
            "H = %02x   L = %02x\n"
            "SP = %04x  PC = %04x\n\n",
            regs->A, regs->F,
            regs->B, regs->C,
            regs->D, regs->E,
            regs->H, regs->L,
            regs->SP, regs->PC);
}

/*
int checksum(byte const *hdr)
{
    uint8_t checksum = 0;

    for (uint16_t address = 0x0134; address <= 0x014C; address++) {
        checksum = checksum - hdr[address] - 1;
    }
    fprintf(stderr, "Checksum: %.2X (Expected: %.2X)\n", checksum, hdr[0x14D]);
    return checksum == hdr[0x14D];
}
*/

void execute(motherboard *mb)
{
    while (mb->cpu.regs.PC != 0x00FE) {
        //dump_regs(&mb->cpu.regs);
        load_next_instruction(mb);
        INSTRUCTIONS_PTR[mb->cpu.regs.IR](mb->cpu.regs.IR, mb, &mb->cpu.regs);
        fprintf(stderr, "\n");
    }
}

static int load_boot_rom(byte * restrict memory, char const * restrict boot_rom_path)
{
    int fd = open(boot_rom_path, O_RDONLY);
    ssize_t n;

    if (fd == -1) {
        perror("open");
        return 1;
    }
    n = read(fd, memory, BOOT_ROM_SIZE);
    if (n != BOOT_ROM_SIZE) {
        fprintf(stderr, "Boot ROM corrupted: expected %u bytes, read %ld", BOOT_ROM_SIZE, n);
        close(fd);
        return 1;
    }
    close(fd);
    return 0;
}

static void init_registers(registers *regs)
{
    regs->AF = 0x1180;
    regs->DE = 0xFF56;
    regs->HL = 0x000D;
}

int main(int ac, char **av)
{
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
    printf("--- DUMP ROM ---\n");
    dump_rom_header(&cart.header);
    printf("--- END OF DUMP ROM ---\n");
    mb.cart = &cart;
    //memcpy(mb.wram + 0x100, cart.raw_hdr, sizeof(cart.raw_hdr));
    execute(&mb);
    free(cart.rom);
    return 0;
}
