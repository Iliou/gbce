#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#include "cpu.h"

[[unsequenced]] uint16_t bytes_to_16bits_int(const byte arr[2])
{
    return ((uint16_t) arr[1] << 8) | arr[0];
}

static int load_cart(char const *rom_path, byte **mem)
{
    int fd = open(rom_path, O_RDONLY);
    int rd_cnt = 0x150;
    ssize_t n;

    if (fd == -1)
        return 1;
    *mem = malloc(rd_cnt);
    n = read(fd, *mem, rd_cnt);
    if (n != rd_cnt) {
        printf("Read %ld bytes insted of %d\n", n, rd_cnt);
        return 1;
    }
    return 0;
}

static void dump_section(byte const *cartridge, char const *sec_name, int start, int length)
{
    printf("%s:", sec_name);
    for (int i = 0; i < length; ++i) {
        if (i % 8 == 0)
            printf("\n");
        else
            printf(" - ");
        printf("%.2X", cartridge[i + start]);
    }
    printf("\n\n");
}

static void dump_text(byte const *rom, char const *sec_name, int start, int sec_length)
{
    printf("%s:\n"
           "%.*s\n\n", sec_name, sec_length, rom + start);
}

static void dump_rom_header(byte const *rom)
{
    uint16_t jmp_addr = bytes_to_16bits_int(rom + 0x102);

    dump_section(rom, "Start", 0, 0x100);
    dump_section(rom, "Entry point", 0x100, 4);
    dump_section(rom, "Nintendo logo", 0x104, 48);
    dump_text(rom, "Title", 0x134, 16);
    dump_section(rom, "SGB flag", 0x146, 1);
    dump_section(rom, "Cartridge type", 0x147, 1);
    dump_section(rom, "ROM size", 0x148, 1);
    dump_section(rom, "RAM size", 0x149, 1);
    uint8_t checksum = 0;
    for (uint16_t address = 0x0134; address <= 0x014C; address++) {
        checksum = checksum - rom[address] - 1;
    }
    printf("Checksum: %.2X - %.2X\n", checksum, rom[0x14D]);
    printf("jump addres: %#.4X (%d)\n", jmp_addr, jmp_addr);
}

int main(int ac, char **av)
{
    byte *cartridge = NULL;

    if (ac != 2) {
        printf("No cartridge\n");
        return 1;
    }
    if (load_cart(av[1], &cartridge))
        return 1;
    dump_rom_header(cartridge);
    free(cartridge);
    return 0;
}
