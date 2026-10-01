#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#include "cartridge.h"

uint16_t bytes_to_16bits_int(const byte arr[2]) [[unsequenced]]
{
    return ((uint16_t) arr[1] << 8) | arr[0];
}

static byte *(*get_mapper(byte type))(cartridge *, a16_t, io_operation)
{
    switch (type) {
    case 0x00:
        fprintf(stderr, "DEBUG: using no mapper\n");
        return no_mapper;
    case 0x01 ... 0x03:
        fprintf(stderr, "DEBUG: using mbc1 mapper\n");
        return mbc1;
    default:
        fprintf(stderr, "Error: mapper %x not implemented\n", type);
        return NULL;
    }
}

static int read_data(int fd, void *dest, ssize_t len)
{
    ssize_t n = read(fd, dest, len);

    if (n != len) {
        if (n == -1) {
            perror("read");
            return 2;
        }
        fprintf(stderr, "Cartridge error: read %ld bytes instead of expected %lu bytes\n", n, len);
        return 0;
    }
    return 0;
}

int create_cartridge(char const * restrict rom_path, cartridge * restrict cart)
{
    int fd = open(rom_path, O_RDONLY);
    size_t rom_size;
    int ret = 0;

    if (fd == -1) {
        perror("open");
        return 2;
    }
    if (read_data(fd, &(cart->header), sizeof(cart->header)) != 0) {
        close(fd);
        return 1;
    }
    if (cart->header.rom_size > 8) {
        fprintf(stderr, "Header error: ROM size not supported: %hhu\n", cart->header.rom_size);
        close(fd);
        return 1;
    }
    rom_size = 1 << (15 + cart->header.rom_size);
    cart->rom = malloc(sizeof(*cart->rom) * rom_size);
    if (read_data(fd, cart->rom, rom_size - HEADER_SIZE))
        ret = 1;
    close(fd);
    cart->mapper = get_mapper(cart->header.cartridge_type);
    return ret;
}

static void dump_section(char const * restrict sec_name, byte const *area, int length)
{
    printf("%s:", sec_name);
    for (int i = 0; i < length; ++i) {
        if (i % 8 == 0)
            printf("\n");
        else
            printf(" - ");
        printf("%.2X", area[i]);
    }
    printf("\n\n");
}

static inline void dump_text(char const * restrict sec_name, byte const *area, int sec_length)
{
    printf("%s:\n"
           "%.*s\n\n", sec_name, sec_length, area);
}

void dump_rom_header(struct header const *hdr)
{
    uint16_t jmp_addr = bytes_to_16bits_int(&(hdr->entry_point[2]));

    dump_section("Start", hdr->start, 0x100);
    dump_section("Entry point", hdr->entry_point, 4);
    dump_section("Nintendo logo", hdr->nintendo_logo, 48);
    dump_text("Title", hdr->title, 16);
    dump_section("SGB flag", &hdr->sgb_flag, 1);
    dump_section("Cartridge type", &hdr->cartridge_type, 1);
    dump_section("ROM size", &hdr->rom_size, 1);
    dump_section("RAM size", &hdr->ram_size, 1);
    printf("jump address: %#.4X (%d)\n", jmp_addr, jmp_addr);
    //dump_section("Full Header", 0, HEADER_SIZE);
}
