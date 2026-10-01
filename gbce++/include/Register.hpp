typedef uint8_t byte;
typedef uint8_t reg8;
typedef uint16_t reg16;
typedef uint8_t n8_t;
typedef uint16_t n16_t;
typedef uint8_t a8_t;
typedef uint16_t a16_t;
typedef int8_t e8_t;
typedef uint16_t color_pixel;

struct Register {
    Register() = default;
    ~Register() = default;

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
                reg16 AF = 0x1180;
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
                reg16 DE = 0xFF56;
                struct {
                    reg8 D;
                    reg8 E;
                };
            };
            union {
                reg16 HL = 0x000D;
                struct {
                    reg8 H;
                    reg8 L;
                };
            };
            reg16 SP; // Stack pointer
            reg16 PC; // Program counter
        };
    };
};
