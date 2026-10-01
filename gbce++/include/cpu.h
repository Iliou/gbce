#ifndef GBCE_CPU_H_
#define GBCE_CPU_H_

#include <stdint.h>

#include "tools.h"
#include "cartridge.h"

#define WRAM (1<<15) // Work RAM: 32 KiB
#define VRAM (1<<14) // Video RAM: 16 KiB
#define HRAM 127 // High RAM: 127B
#define OAM 160 // OAM: 160B

#define ROM (1<<11) // 2 KiB

#define BOOT_ROM_SIZE (0x0900)

#define SCREEN_HEIGHT 160
#define SCREEN_WIDTH 144
#define WRAM_REGISTERS_START 0xFF00

// TODO: handle endianness;
typedef struct {
    // Special-purpose registers
    reg8 IR; // Instruction register
    union {
        reg8 IE;
        struct {
            byte IE_UNUSED:3;
            byte IE_JOYPAD:1;
            byte IE_SERIAL:1;
            byte IE_TIMER:1;
            byte IE_STAT:1;
            byte IE_VBLANK:1;
        };
    };

    // General-purpose registers
    union {
        reg8 general_regs8[12]; // general-purpose 8-bits registers direct access;
        reg16 general_regs[6]; // general-purpose 16-bits registers direct access;
        struct {
            union {
                reg16 AF;
                struct {
                    reg8 A; // Accumulator
                    union {
                        reg8 F; // Flags
                        struct {
                            byte _unused:4; // flags 0-3 unused
                            byte c:1; // Carry
                            byte h:1; // Half Carry
                            byte n:1; // Substraction
                            byte z:1; // Zero
                        };
                    };
                };
            };
            union {
                reg16 BC;
                struct {
                    reg8 B;
                    reg8 C;
                };
            };
            union {
                reg16 DE;
                struct {
                    reg8 D;
                    reg8 E;
                };
            };
            union {
                reg16 HL;
                struct {
                    reg8 H;
                    reg8 L;
                };
            };
            reg16 SP; // Stack pointer
            reg16 PC; // Program counter
        };
    };
} registers;

typedef struct {
    union {
        byte regs[128];
        struct {
            union {
                byte P1;
                struct {
                    byte _P1_unused:2;
                    byte P15:1;
                    byte P14:1;
                    byte P13:1;
                    byte P12:1;
                    byte P11:1;
                    byte P10:1;
                };
            };
            byte SB;
            union {
                byte SC;
                struct {
                    byte SIO_EN:1;
                    byte _SC_unused:5;
                    byte SIO_FAST:1;
                    byte SIO_CLK:1;
                };
            };
            byte _unused_0xFF03;
            byte DIV;
            byte TIMA;
            byte TMA;
            union {
                byte TAC;
                struct {
                    byte _TAC_unused:5;
                    byte TAC_EN:1;
                    byte TAC_CLK:2;
                };
            };
            byte _unused_0xFF08_0xFF0E[7];
            union {
                byte IF;
                struct {
                    byte _IF_unused:3;
                    byte IF_JOYPAD:1;
                    byte IF_SERIAL:1;
                    byte IF_TIMER:1;
                    byte IF_STAT:1;
                    byte IF_VBLANK:1;
                };
            };
            byte NR[23];
            byte _unused_0xFF27_0xFF2F;
            byte WAV[16];
            union {
                byte LCDC;
                struct {
                    byte LCD_EN:1;
                    byte WIN_MAP:1;
                    byte WIN_EN:1;
                    byte TILE_SEL:1;
                    byte BG_MAP:1;
                    byte OBJ_SIZE:1;
                    byte OBJ_EN:1;
                    byte BG_EN:1;
                };
            };
            union {
                byte STAT;
                struct {
                    byte _STAT_unused:1;
                    byte INTR_LYC:1;
                    byte INTR_M2:1;
                    byte INTR_M1:1;
                    byte INTR_M0:1;
                    byte LYC_STAT:1;
                    byte LCD_MODE:2;
                };
            };
            byte SCX;
            byte SCY;
            byte LY;
            byte LYC;
            byte DMA;
            byte BGP;
            byte OBP0;
            byte OBP1;
            byte WY;
            byte WX;
            byte _unknown_0xFF4C;
            union {
                byte KEY1;
                struct {
                    byte KEY1_FAST:1;
                    byte _KEY1_unused:6;
                    byte KEY1_EN:1;
                };
            };
            byte _unused_0xFF4E;
            union {
                byte VBK_;
                struct {
                    byte _VBK_unused:7;
                    byte VBK:1;
                };
            };
            union {
                byte BOOT;
                struct {
                    byte _BOOT_unused:7;
                    byte BOOT_OFF:1;
                };
            };
            byte _unimplemented[0xFF70 - 0xFF51]; // TODO implement them maybe ?
            union {
                byte SVBK_;
                struct {
                    byte _SVBK_unused:5;
                    byte SVBK:3;
                };
            };
            byte _unimplemented2[0xFFFF - 0xFF71]; // TODO implement them maybe ?
        };
    };
} hw_registers;

typedef struct {
    byte oam[OAM];
} ppu_t;

typedef struct {
    byte rom[ROM];
    byte vram[VRAM]; // video/display RAM
    byte hram[HRAM]; // high RAM
    ppu_t ppu;
    registers regs;
    bool ime; // interrupt master enable flag
} cpu_t;

typedef struct {
    cpu_t cpu;
    byte wram[WRAM]; // work RAM
    hw_registers hw_regs;
    color_pixel screen[SCREEN_HEIGHT][SCREEN_WIDTH];
    cartridge cart;
    byte boot_rom[BOOT_ROM_SIZE];
} motherboard;

byte *map_address(motherboard *mb, a16_t address, io_operation mode);
void load_next_instruction(motherboard *mb);

#endif
