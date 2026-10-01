#ifndef INSTRUCTIONS_H_
    #define INSTRUCTIONS_H_

    #include "tools.h"

    #define UPDATE_Z(eval) ((eval) == 0 ? 1 : 0)

typedef void ins_t(byte opcode, motherboard *mb, registers *regs);
typedef void prefix_ins_t(byte opcode, motherboard *mb, registers *regs, byte rdata);

// load
ins_t ld8_regs; // OK
ins_t ld8; // OK
ins_t ld16; //OK
ins_t ldh; // OK
ins_t push; // OK
ins_t pop; // OK

// arithmetic/logic
ins_t add; //OK
ins_t adc;
ins_t sub;
ins_t sbc;
ins_t cp;
ins_t inc8; //OK
ins_t inc16; //OK
ins_t dec8; //OK
ins_t dec16; //OK
ins_t and;
ins_t or;
ins_t xor;
ins_t ccf;
ins_t scf;
ins_t daa;
ins_t cpl;

// Bitwise ops
ins_t rlca;
ins_t rrca;
ins_t rla;
ins_t rra;

// Control flow
ins_t cond_jp; // OK
ins_t jp; // OK
ins_t jr; // OK
ins_t call; // OK
ins_t ret; // OK
ins_t reti;
ins_t rst;

// misc
ins_t nop; // OK
ins_t stop;
ins_t halt;
ins_t di;
ins_t ei;

ins_t prefix;
ins_t invalid;

static ins_t *const INSTRUCTIONS_PTR[] = {
    nop, ld16, ld8, inc16, inc8, dec8, ld8, rlca, ld16, add, ld8, dec16, inc8, dec8, ld8, rrca,
    stop, ld16, ld8, inc16, inc8, dec8, ld8, rla, jr, add, ld8, dec16, inc8, dec8, ld8, rra,
    jr, ld16, ld8, inc16, inc8, dec8, ld8, daa, jr, add, ld8, dec16, inc8, dec8, ld8, cpl,
    jr, ld16, ld8, inc16, inc8, dec8, ld8, scf, jr, add, ld8, dec16, inc8, dec8, ld8, ccf,
    ld8_regs, ld8_regs, ld8_regs, ld8_regs, ld8_regs, ld8_regs, ld8_regs, ld8_regs, ld8_regs, ld8_regs, ld8_regs, ld8_regs, ld8_regs, ld8_regs, ld8_regs, ld8_regs,
    ld8_regs, ld8_regs, ld8_regs, ld8_regs, ld8_regs, ld8_regs, ld8_regs, ld8_regs, ld8_regs, ld8_regs, ld8_regs, ld8_regs, ld8_regs, ld8_regs, ld8_regs, ld8_regs,
    ld8_regs, ld8_regs, ld8_regs, ld8_regs, ld8_regs, ld8_regs, ld8_regs, ld8_regs, ld8_regs, ld8_regs, ld8_regs, ld8_regs, ld8_regs, ld8_regs, ld8_regs, ld8_regs,
    ld8_regs, ld8_regs, ld8_regs, ld8_regs, ld8_regs, ld8_regs, halt, ld8_regs, ld8_regs, ld8_regs, ld8_regs, ld8_regs, ld8_regs, ld8_regs, ld8_regs, ld8_regs,
    add, add, add, add, add, add, add, add, adc, adc, adc, adc, adc, adc, adc, adc,
    sub, sub, sub, sub, sub, sub, sub, sub, sbc, sbc, sbc, sbc, sbc, sbc, sbc, sbc,
    and, and, and, and, and, and, and, and, xor, xor, xor, xor, xor, xor, xor, xor,
    or, or, or, or, or, or, or, or, cp, cp, cp, cp, cp, cp, cp, cp,
    ret, pop, cond_jp, jp, call, push, add, rst, ret, ret, cond_jp, prefix, call, call, adc, rst,
    ret, pop, cond_jp, invalid, call, push, sub, rst, ret, reti, cond_jp, invalid, call, invalid, sbc, rst,
    ldh, pop, ldh, invalid, invalid, push, and, rst, add, jp, ld8, invalid, invalid, invalid, xor, rst,
    ldh, pop, ldh, di, invalid, push, or, rst, ld16, ld16, ld8, ei, invalid, invalid, cp, rst,
};

prefix_ins_t rlc, rrc, rl, rr, sla, sra, swap, srl, bit, res, set;

static prefix_ins_t *const PREFIX_INSTRUCTIONS_PTR[] = {
    [0x00 ... 0x07] = rlc, [0x08 ... 0x0F] = rrc,
    [0x10 ... 0x17] = rl, [0x18 ... 0x1F] = rr,
    [0x20 ... 0x27] = sla, [0x28 ... 0x2F] = sra,
    [0x30 ... 0x37] = swap, [0x38 ... 0x3F] = srl,
    [0x40 ... 0x7F] = bit,
    [0x80 ... 0xBF] = res,
    [0xC0 ... 0xFF] = set
};

#endif
