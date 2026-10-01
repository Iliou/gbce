#ifndef INSTRUCTIONS_H_
    #define INSTRUCTIONS_H_

    #include "tools.h"

    #define UPDATE_Z(eval) ((eval) == 0 ? 1 : 0)

// load
void ld8_regs(byte opcode, motherboard *mb, registers *regs); // OK
void ld8(byte opcode, motherboard *mb, registers *regs); // OK
void ld16(byte opcode, motherboard *mb, registers *regs); //OK
void ldh(byte opcode, motherboard *mb, registers *regs); // OK
void push(byte opcode, motherboard *mb, registers *regs); // OK
void pop(byte opcode, motherboard *mb, registers *regs); // OK

// arithmetic/logic
void add(byte opcode, motherboard *mb, registers *regs); //OK
void adc(byte opcode, motherboard *mb, registers *regs);
void sub(byte opcode, motherboard *mb, registers *regs);
void sbc(byte opcode, motherboard *mb, registers *regs);
void cp(byte opcode, motherboard *mb, registers *regs);
void inc8(byte opcode, motherboard *mb, registers *regs); //OK
void inc16(byte opcode, motherboard *mb, registers *regs); //OK
void dec8(byte opcode, motherboard *mb, registers *regs); //OK
void dec16(byte opcode, motherboard *mb, registers *regs); //OK
void and(byte opcode, motherboard *mb, registers *regs);
void or(byte opcode, motherboard *mb, registers *regs);
void xor(byte opcode, motherboard *mb, registers *regs);
void ccf(byte opcode, motherboard *mb, registers *regs);
void scf(byte opcode, motherboard *mb, registers *regs);
void daa(byte opcode, motherboard *mb, registers *regs);
void cpl(byte opcode, motherboard *mb, registers *regs);

// Bitwise ops
void rlca(byte opcode, motherboard *mb, registers *regs);
void rrca(byte opcode, motherboard *mb, registers *regs);
void rla(byte opcode, motherboard *mb, registers *regs);
void rra(byte opcode, motherboard *mb, registers *regs);

// Control flow
void cond_jp(byte opcode, motherboard *mb, registers *regs); // OK
void jp(byte opcode, motherboard *mb, registers *regs); // OK
void jr(byte opcode, motherboard *mb, registers *regs); // OK
void call(byte opcode, motherboard *mb, registers *regs); // OK
void ret(byte opcode, motherboard *mb, registers *regs); // OK
void reti(byte opcode, motherboard *mb, registers *regs);
void rst(byte opcode, motherboard *mb, registers *regs);

// misc
void nop(byte opcode, motherboard *mb, registers *regs); // OK
void stop(byte opcode, motherboard *mb, registers *regs);
void halt(byte opcode, motherboard *mb, registers *regs);
void di(byte opcode, motherboard *mb, registers *regs);
void ei(byte opcode, motherboard *mb, registers *regs);

void prefix(byte opcode, motherboard *mb, registers *regs);
void invalid(byte opcode, motherboard *mb, registers *regs);

static void (* const INSTRUCTIONS_PTR[])(byte opcode, motherboard *mb, registers *regs) = {
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


void rlc(byte opcode, registers *regs, byte *data, uint8_t value);
void rrc(byte opcode, registers *regs, byte *data, uint8_t value);
void rl(byte opcode, registers *regs, byte *data, uint8_t value);
void rr(byte opcode, registers *regs, byte *data, uint8_t value);
void sla(byte opcode, registers *regs, byte *data, uint8_t value);
void sra(byte opcode, registers *regs, byte *data, uint8_t value);
void swap(byte opcode, registers *regs, byte *data, uint8_t value);
void srl(byte opcode, registers *regs, byte *data, uint8_t value);
void bit(byte opcode, registers *regs, byte *data, uint8_t value);
void res(byte opcode, registers *regs, byte *data, uint8_t value);
void set(byte opcode, registers *regs, byte *data, uint8_t value);

static void (* const PREFIX_INSTRUCTIONS_PTR[])(byte opcode, registers *regs, byte *data, uint8_t value) = {
    [0x00 ... 0x07] = rlc, [0x08 ... 0x0F] = rrc,
    [0x10 ... 0x17] = rl, [0x18 ... 0x1F] = rr,
    [0x20 ... 0x27] = sla, [0x28 ... 0x2F] = sra,
    [0x30 ... 0x37] = swap, [0x38 ... 0x3F] = srl,
    [0x40 ... 0x7F] = bit,
    [0x80 ... 0xBF] = res,
    [0xC0 ... 0xFF] = set
};

#endif
