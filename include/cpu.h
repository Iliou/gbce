#ifndef GBCE_CPU_H_
#define GBCE_CPU_H_

#include <stdint.h>

#define WRAM (1<<13)
#define VRAM (1<<13)
#define HRAM 127

#define ROM 256

#define REG_COUNT 6

typedef uint8_t byte;

typedef struct {
    uint8_t data_bus;
    uint16_t address_bus;
    uint16_t registers[REG_COUNT];
} proc_t;

typedef struct {
    uint8_t rom[ROM];
    uint8_t hram[HRAM];
    proc_t process;
} cpu_t;

typedef struct {
    cpu_t cpu;
    uint8_t wram[WRAM];
    uint8_t vram[VRAM];
    // cartridge goes here
} motherboard;

#endif
