#ifndef CARTRIDGE_H_
    #define CARTRIDGE_H_

    #include "tools.h"

    #define HEADER_SIZE 0x150

typedef struct cartridge {
    union {
        byte raw_hdr[HEADER_SIZE];
        struct header {
            byte start[0x100];
            byte entry_point[4];
            byte nintendo_logo[48];
            union {
                byte title[16];
                struct {
                    byte new_title[11];
                    byte manufacturer_code[4];
                    byte cgb_flag;
                };
            };
            byte new_licensee_code[2];
            byte sgb_flag;
            byte cartridge_type;
            byte rom_size;
            byte ram_size;
            byte dest_code;
            byte old_licensee_code;
            byte rom_version;
            byte header_checksum;
            byte global_checksum[2];
        } header;
    };
    byte *rom;
    byte *ram; //Optional
    byte *(*mapper)(struct cartridge *c, a16_t address, io_operation mode);
} cartridge;

void dump_rom_header(struct header const *hdr);
int create_cartridge(char const * restrict rom_path, cartridge * restrict cart);

// Mappers
byte *no_mapper(struct cartridge *cart, a16_t address, io_operation mode);
byte *mbc1(struct cartridge *cart, a16_t address, io_operation mode);

#endif
