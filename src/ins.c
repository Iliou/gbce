#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "tools.h"
#include "cpu.h"
#include "ins.h"

#define MAP_REGS_SRC(opcode) ((opcode & 7) + 2)

//TODO URGENT: replace general_regs8 tricks with regular register names

static void dump_regs(registers const *regs)
{
    fprintf(stderr, "A = %d   F = %d\n"
           "B = %d   C = %d\n"
           "D = %d   E = %d\n"
           "H = %d   L = %d\n"
           "SP = %d  PC = %d\n\n",
           regs->A, regs->F,
           regs->B, regs->C,
           regs->D, regs->E,
           regs->H, regs->L,
           regs->SP, regs->PC);
}

static void push_data16_on_stack(byte *wram, registers *regs, uint16_t data)
{
    fprintf(stderr, "Pushing data 0x%x%x (0x%x) at address SP: 0x%x\n", data >> 4, data & 0x0f, data, regs->SP);
    wram[regs->SP - 1] = data >> 4; // MSB
    wram[regs->SP - 2] = data & 0x0F; // LSB
    regs->SP -= 2;
}

static inline void pop_data16_on_stack(byte *wram, registers *regs, reg16 *dest)
{
    *dest = wram[regs->SP]; // LSB
    *dest |= wram[regs->SP + 1] << 4; // MSB
    regs->SP += 2;
}

static inline a16_t memory_load16(motherboard *mb, a16_t load_address)
{
    return ((read_address(mb, load_address + 1) << 8) | read_address(mb, load_address));
}

static byte read_data_from_low_op(uint8_t low_op, motherboard const *mb, registers const *regs)
{
    switch (low_op) {
    case 0x0:
    case 0x8:
        return regs->B;
    case 0x1:
    case 0x9:
        return regs->C;
    case 0x2:
    case 0xa:
        return regs->D;
    case 0x3:
    case 0xb:
        return regs->E;
    case 0x4:
    case 0xc:
        return regs->H;
    case 0x5:
    case 0xd:
        return regs->L;
    case 0x6:
    case 0xe:
        return read_address(mb, regs->HL);
    case 0x7:
    case 0xf:
        return regs->A;
    }
    return -1;
}

static void write_data_from_low_op(uint8_t low_op, motherboard *mb, registers *regs, byte to_write)
{
    switch (low_op) {
    case 0x0:
    case 0x8:
        regs->B = to_write;
        break;
    case 0x1:
    case 0x9:
        regs->C = to_write;
        break;
    case 0x2:
    case 0xa:
        regs->D = to_write;
        break;
    case 0x3:
    case 0xb:
        regs->E = to_write;
        break;
    case 0x4:
    case 0xc:
        regs->H = to_write;
        break;
    case 0x5:
    case 0xd:
        regs->L = to_write;
        break;
    case 0x6:
    case 0xe:
        write_address(mb, regs->HL, to_write);
        break;
    case 0x7:
    case 0xf:
        regs->A = to_write;
        break;
    }
}

// load
void ld8_regs(byte opcode, motherboard *mb, registers *regs)
{
    fprintf(stderr, "ld8 (regs) OK\n");
    uint8_t low_op = opcode & 0x0F;
    uint8_t hi_op = opcode >> 4;
    uint8_t data = read_data_from_low_op(low_op, mb, regs);

    switch (hi_op) {
    case 0x4:
        if (low_op < 8)
            regs->B = data;
        else
            regs->C = data;
        break;
    case 0x5:
        if (low_op < 8)
            regs->D = data;
        else
            regs->E = data;
        break;
    case 0x6:
        if (low_op < 8)
            regs->H = data;
        else
            regs->L = data;
        break;
    case 0x7:
        if (low_op < 8)
            write_address(mb, regs->HL, data);
        else
            regs->A = data;
        break;
    }
}

void ld8(byte opcode, motherboard *mb, registers *regs)
{
    fprintf(stderr, "ld8 OK\n");
    switch (opcode) {
    case 0x02:
        write_address(mb, regs->BC, regs->A);
        return;
    case 0x12:
        write_address(mb, regs->DE, regs->A);
        return;
    case 0x22:
        write_address(mb, regs->HL, regs->A);
        regs->HL++;
        return;
    case 0x32:
        write_address(mb, regs->HL, regs->A);
        regs->HL--;
        return;
    case 0x06:
        regs->B = read_address(mb, regs->PC++);
        return;
    case 0x16:
        regs->D = read_address(mb, regs->PC++);
        return;
    case 0x26:
        regs->H = read_address(mb, regs->PC++);
        return;
    case 0x36:
        write_address(mb, regs->HL, read_address(mb, regs->PC));
        return;
    case 0x0a:
        regs->A = read_address(mb, regs->BC);
        return;
    case 0x1a:
        regs->A = read_address(mb, regs->DE);
        return;
    case 0x2a:
        regs->A = read_address(mb, regs->HL++);
        return;
    case 0x3a:
        regs->A = read_address(mb, regs->HL--);
        return;
    case 0x0e:
        regs->C = read_address(mb, regs->PC++);
        return;
    case 0x1e:
        regs->E = read_address(mb, regs->PC++);
        return;
    case 0x2e:
        regs->L = read_address(mb, regs->PC++);
        return;
    case 0x3e:
        regs->A = read_address(mb, regs->PC++);
        return;
    case 0xea:
        write_address(mb, memory_load16(mb, regs->PC), regs->A);
        regs->PC += 2;
        return;
    case 0xfa:
        regs->A = read_address(mb, memory_load16(mb, regs->PC));
        regs->PC += 2;
        return;
    }
}

void ld16(byte opcode, motherboard *mb, registers *regs)
{
    byte *address;

    fprintf(stderr, "ld16 OK\n");
    switch (opcode & 0x0F) {
    case 0x1:
        regs->general_regs[1 + (opcode >> 4)] = memory_load16(mb, regs->PC);
        regs->PC += 2;
        break;
    case 0x8:
        switch (opcode >> 4) {
        case 0x0:
            /*
            address = map_address(mb, memory_load16(mb, regs->PC), WRITE);
            address[0] = regs->SP & 0xFF;
            address[1] = regs->SP >> 8;
            */
            a16_t ld_address = memory_load16(mb, regs->PC);
            write_address(mb, ld_address, regs->SP & 0xFF);
            write_address(mb, ld_address + 1, regs->SP >> 8);
            regs->PC += 2;
            break;
        case 0xF:
            e8_t e = (e8_t) mb->wram[regs->PC++];
            regs->HL = regs->SP + e;
            regs->z = 0;
            regs->n = 0;
            regs->h = ((regs->SP ^ e ^ regs->HL) & 0x10) != 0;
            regs->c = ((regs->SP ^ e ^ regs->HL) & 0x100) != 0;
            break;
        }
        break;
    case 0x9:
        regs->SP = regs->HL;
        break;
    }
}

void ldh(byte opcode, motherboard *mb, registers *regs)
{
    byte *address;
    a16_t load_address = 0xFF00;

    fprintf(stderr, "ldh OK\n");
    switch (opcode) {
    case 0xE0:
        load_address |= read_address(mb, regs->PC++);
        write_address(mb, load_address, regs->A);
        if (load_address == 0xFF50)
            exit(0);
        break;
    case 0xF0:
        load_address |= read_address(mb, regs->PC++);
        regs->A = read_address(mb, load_address);
        break;
    case 0xE2:
        load_address |= regs->C;
        write_address(mb, load_address, regs->A);
        break;
    case 0xF2:
        load_address |= regs->C;
        regs->A = read_address(mb, load_address);
        break;
    }
}

void push(byte opcode, motherboard *mb, registers *regs)
{
    fprintf(stderr, "push OK\n");
    switch (opcode) {
    case 0xC5:
        push_data16_on_stack(mb->wram, regs, regs->BC);
        break;
    case 0xD5:
        push_data16_on_stack(mb->wram, regs, regs->DE);
        break;
    case 0xE5:
        push_data16_on_stack(mb->wram, regs, regs->HL);
        break;
    case 0xF5:
        push_data16_on_stack(mb->wram, regs, regs->AF);
        break;
    }
}

void pop(byte opcode, motherboard *mb, registers *regs)
{
    fprintf(stderr, "pop OK\n");
    switch (opcode) {
    case 0xC1:
        pop_data16_on_stack(mb->wram, regs, &regs->BC);
        break;
    case 0xD1:
        pop_data16_on_stack(mb->wram, regs, &regs->DE);
        break;
    case 0xE1:
        pop_data16_on_stack(mb->wram, regs, &regs->HL);
        break;
    case 0xF1:
        pop_data16_on_stack(mb->wram, regs, &regs->AF);
        break;
    }
}


// arithmetic/logic
void add(byte opcode, motherboard *mb, registers *regs)
{
    fprintf(stderr, "add: %X OK\n", opcode);
    regs->n = 0;
    switch (opcode & 0x0f) {
    case 0x08:
        e8_t e = (e8_t) read_address(mb, regs->PC++);
        regs->z = 0;
        regs->h = ((regs->SP & 0x0f) + (e & 0x0f)) > 0x0f;
        regs->c = ((regs->SP & 0xff) + e) > 0xff;
        regs->SP += e;
        return;
    case 0x09:
        reg16 reg = regs->general_regs[1 + (opcode >> 4)];
        regs->h = ((regs->HL & 0x0f) + (reg & 0x0f)) > 0x0f;
        regs->c = ((regs->HL & 0xff) + (reg & 0xff)) > 0xff;
        regs->HL += reg;
        return;
    default:
        n8_t data;
        switch (opcode) {
        case 0x80:
            data = regs->B;
            break;
        case 0x81:
            data = regs->C;
            break;
        case 0x82:
            data = regs->D;
            break;
        case 0x83:
            data = regs->E;
            break;
        case 0x84:
            data = regs->H;
            break;
        case 0x85:
            data = regs->L;
            break;
        case 0x86:
            data = read_address(mb, regs->HL);
            break;
        case 0x87:
            data = regs->A;
            break;
        case 0xc6:
            data = read_address(mb, regs->PC);
            regs->PC++;
            break;
        }
        regs->h = ((regs->A & 0x0f) + (data & 0x0f)) > 0x0f;
        regs->c = (((reg16) regs->A) + data) > 0xff;
        regs->A += data;
        regs->z = UPDATE_Z(regs->A);
        break;
    }
}

void adc(byte opcode, motherboard *mb, registers *regs)
{
    fprintf(stderr, "adc KO\n");
    exit(0);
}

void sub(byte opcode, motherboard *mb, registers *regs)
{
    fprintf(stderr, "sub KO\n");
    exit(0);
}

void sbc(byte opcode, motherboard *mb, registers *regs)
{
    fprintf(stderr, "sbc KO\n");
    exit(0);
}

void cp(byte opcode, motherboard *mb, registers *regs)
{
    byte data;

    fprintf(stderr, "cp OK\n");
    switch (opcode) {
    case 0xb8:
        data = regs->B;
        break;
    case 0xb9:
        data = regs->C;
        break;
    case 0xba:
        data = regs->D;
        break;
    case 0xbb:
        data = regs->E;
        break;
    case 0xbc:
        data = regs->H;
        break;
    case 0xbd:
        data = regs->L;
        break;
    case 0xbe:
        data = read_address(mb, regs->HL);
        break;
    case 0xbf:
        regs->z = 1;
        regs->n = 1;
        regs->h = 0;
        regs->c = 0;
        return;
    case 0xfe:
        data = read_address(mb, regs->PC);
        regs->PC++;
        break;
    }
    regs->z = UPDATE_Z(regs->A - data);
    regs->n = 1;
    regs->h = (regs->A & 0x0f) < (data & 0x0f) ? 1 : 0;
    regs->c = regs->A < data ? 1 : 0;
}

void inc8(byte opcode, motherboard *mb, registers *regs)
{
    byte res;

    fprintf(stderr, "inc8 OK\n");
    regs->n = 0;
    switch (opcode) {
    case 0x04:
        res = ++regs->B;
        break;
    case 0x0c:
        res = ++regs->C;
        break;
    case 0x14:
        res = ++regs->D;
        break;
    case 0x1c:
        res = ++regs->E;
        break;
    case 0x24:
        res = ++regs->H;
        break;
    case 0x2c:
        res = ++regs->L;
        break;
    case 0x34:
        res = read_address(mb, regs->HL) + 1;
        write_address(mb, regs->HL, res);
        break;
    case 0x3c:
        res = ++regs->A;
        break;
    }
    regs->z = UPDATE_Z(res);
    regs->h = ((res - 1) & 0x0f) == 0x0f;
}

void inc16(byte opcode, motherboard *mb, registers *regs)
{
    fprintf(stderr, "inc16 OK\n");
    regs->general_regs[1 + (opcode >> 4)]++;
}

void dec8(byte opcode, motherboard *mb, registers *regs)
{
    byte res;

    fprintf(stderr, "dec8 OK\n");
    regs->n = 1;
    switch (opcode) {
    case 0x05:
        res = --regs->B;
        break;
    case 0x0d:
        res = --regs->C;
        break;
    case 0x15:
        res = --regs->D;
        break;
    case 0x1d:
        res = --regs->E;
        break;
    case 0x25:
        res = --regs->H;
        break;
    case 0x2d:
        res = --regs->L;
        break;
    case 0x35:
        res = read_address(mb, regs->HL) - 1;
        write_address(mb, regs->HL, res);
        break;
    case 0x3d:
        res = --regs->A;
        break;
    }
    regs->z = UPDATE_Z(res);
    regs->h = ((res + 1) & 0x0f) == 0;
}

void dec16(byte opcode, motherboard *mb, registers *regs)
{
    fprintf(stderr, "dec16 OK\n");
    regs->general_regs[1 + (opcode >> 4)]--;
}

void and(byte opcode, motherboard *mb, registers *regs)
{
    reg8 data;

    fprintf(stderr, "and OK\n");
    switch (opcode) {
    case 0xa0:
        data = regs->B;
        break;
    case 0xa1:
        data = regs->C;
        break;
    case 0xa2:
        data = regs->D;
        break;
    case 0xa3:
        data = regs->E;
        break;
    case 0xa4:
        data = regs->H;
        break;
    case 0xa5:
        data = regs->L;
        break;
    case 0xa6:
        data = read_address(mb, regs->HL);
        break;
    case 0xa7:
        data = regs->A;
        break;
    case 0xe6:
        data = read_address(mb, regs->PC);
        regs->PC++;
        break;
    }
    regs->A &= data;
    regs->z = UPDATE_Z(regs->A);
    regs->n = 0;
    regs->h = 1;
    regs->c = 0;
}

void or(byte opcode, motherboard *mb, registers *regs)
{
    reg8 data;

    fprintf(stderr, "or OK\n");
    switch (opcode) {
    case 0xb0:
        data = regs->B;
        break;
    case 0xb1:
        data = regs->C;
        break;
    case 0xb2:
        data = regs->D;
        break;
    case 0xb3:
        data = regs->E;
        break;
    case 0xb4:
        data = regs->H;
        break;
    case 0xb5:
        data = regs->L;
        break;
    case 0xb6:
        data = read_address(mb, regs->HL);
        break;
    case 0xb7:
        data = regs->A;
        break;
    case 0xf6:
        data = read_address(mb, regs->PC);
        regs->PC++;
        break;
    }
    regs->A |= data;
    regs->z = UPDATE_Z(regs->A);
    regs->n = 0;
    regs->h = 0;
    regs->c = 0;
}

void xor(byte opcode, motherboard *mb, registers *regs)
{
    reg8 data;

    fprintf(stderr, "xor OK\n");
    switch (opcode) {
    case 0xa8:
        data = regs->B;
        break;
    case 0xa9:
        data = regs->C;
        break;
    case 0xaa:
        data = regs->D;
        break;
    case 0xab:
        data = regs->E;
        break;
    case 0xac:
        data = regs->H;
        break;
    case 0xad:
        data = regs->L;
        break;
    case 0xae:
        data = read_address(mb, regs->HL);
        break;
    case 0xaf:
        data = regs->A;
        break;
    case 0xee:
        data = read_address(mb, regs->PC);
        regs->PC++;
        break;
    }
    regs->A ^= data;
    regs->z = UPDATE_Z(regs->A);
    regs->n = 0;
    regs->h = 0;
    regs->c = 0;
}

void ccf(byte opcode, motherboard *mb, registers *regs)
{
    regs->n = 0;
    regs->h = 0;
    regs->c = ~regs->c;
    fprintf(stderr, "ccf OK\n");
}

void scf(byte opcode, motherboard *mb, registers *regs)
{
    regs->n = 0;
    regs->h = 0;
    regs->c = 1;
    fprintf(stderr, "scf OK\n");
}

void daa(byte opcode, motherboard *mb, registers *regs)
{
    fprintf(stderr, "daa KO\n");
    exit(0);
}

void cpl(byte opcode, motherboard *mb, registers *regs)
{
    fprintf(stderr, "cpl OK\n");
    regs->A = ~regs->A;
    regs->n = 1;
    regs->h = 1;
}


// Bitwise ops
void rlca(byte opcode, motherboard *mb, registers *regs)
{
    byte carry = regs->A >> 7;

    regs->A = (regs->A << 1) & carry;
    regs->z = 0;
    regs->n = 0;
    regs->h = 0;
    regs->c = carry;
    fprintf(stderr, "rlca OK\n");
}

void rrca(byte opcode, motherboard *mb, registers *regs)
{
    byte carry = regs->A & 1;

    regs->A = (regs->A >> 1) & (carry << 7);
    regs->z = 0;
    regs->n = 0;
    regs->h = 0;
    regs->c = carry;
    fprintf(stderr, "rrca OK\n");
}

void rla(byte opcode, motherboard *mb, registers *regs)
{
    byte carry = regs->A >> 7;

    fprintf(stderr, "rla OK\n");
    fprintf(stderr, "Before A = %d\n", regs->A);
    regs->A = (regs->A << 1) & regs->c;
    regs->z = 0;
    regs->n = 0;
    regs->h = 0;
    regs->c = carry;
    fprintf(stderr, "After A = %d\n", regs->A);
}

void rra(byte opcode, motherboard *mb, registers *regs)
{
    byte carry = regs->A & 1;

    regs->A = (regs->A >> 1) & (regs->c << 7);
    regs->z = 0;
    regs->n = 0;
    regs->h = 0;
    regs->c = carry;
    fprintf(stderr, "rra OK\n");
}


// Control flow
void cond_jp(byte opcode, motherboard *mb, registers *regs)
{
    a16_t address = memory_load16(mb, regs->PC);
    bool eval = opcode & 8;

    regs->PC += 2;
    fprintf(stderr, "jp (cond) address: %X OK\n", address);
    if (opcode >> 4 == 0x0C) {
        if (regs->z == eval)
            regs->PC = address;
    } else { // 0x0D
        if (regs->c == eval)
            regs->PC = address;
    }
}

void jp(byte opcode, motherboard *mb, registers *regs)
{
    fprintf(stderr, "jp (uncond) address: ");
    if (opcode == 0xC3) {
        regs->PC = memory_load16(mb, regs->PC);
    } else { // opcode == 0xE9
        regs->PC = regs->HL;
    }
    fprintf(stderr, "%X OK\n", regs->PC);
}

void jr(byte opcode, motherboard *mb, registers *regs)
{
    bool eval = opcode & 8;
    e8_t offset = read_address(mb, regs->PC);

    regs->PC++;
    fprintf(stderr, "jr: offset %d OK\n", offset);
    switch (opcode >> 4) {
    case 1:
        regs->PC += offset;
        break;
    case 2:
        fprintf(stderr, "Check if z (%d) == eval (%d)\n", regs->z, eval);
        if (regs->z == eval)
            regs->PC += offset;
        break;
    case 3:
        if (regs->c == eval)
            regs->PC += offset;
        break;
    }
}

void call(byte opcode, motherboard *mb, registers *regs)
{
    a16_t jump_address = memory_load16(mb, regs->PC);

    regs->PC += 2;
    fprintf(stderr, "call OK\n");
    switch (opcode) {
    case 0xC4:
        if (regs->z)
            return;
        break;
    case 0xD4:
        if (regs->c)
            return;
        break;
    case 0xCC:
        if (!regs->z)
            return;
        break;
    case 0xDC:
        if (!regs->c)
            return;
        break;
    }
    push_data16_on_stack(mb->wram, regs, regs->PC);
    regs->PC = jump_address;
}

void ret(byte opcode, motherboard *mb, registers *regs)
{
    fprintf(stderr, "ret OK\n");
    switch (opcode) {
    case 0xC0:
        if (regs->z)
            return;
        break;
    case 0xD0:
        if (regs->c)
            return;
        break;
    case 0xC8:
        if (!regs->z)
            return;
        break;
    case 0xD8:
        if (!regs->c)
            return;
        break;
    }
    pop_data16_on_stack(mb->wram, regs, &regs->PC);
}

void reti(byte opcode, motherboard *mb, registers *regs)
{
    fprintf(stderr, "reti KO\n");
    exit(0);
}

void rst(byte opcode, motherboard *mb, registers *regs)
{
    fprintf(stderr, "rst KO\n");
    exit(0);
}


// misc
void nop(byte opcode, motherboard *mb, registers *regs)
{
    fprintf(stderr, "nop OK\n");
}

void stop(byte opcode, motherboard *mb, registers *regs)
{
    fprintf(stderr, "stop KO\n");
    regs->PC++;
    exit(0);
}

void halt(byte opcode, motherboard *mb, registers *regs)
{
    fprintf(stderr, "halt KO\n");
    exit(0);
}

void di(byte opcode, motherboard *mb, registers *regs)
{
    fprintf(stderr, "di OK\n");
    mb->cpu.ime = 0;
}

void ei(byte opcode, motherboard *mb, registers *regs)
{
    fprintf(stderr, "ei OK\n");
    mb->cpu.ime = 1;
}

void prefix(byte opcode, motherboard *mb, registers *regs)
{
    byte rdata;

    fprintf(stderr, "Prefix\n");
    load_next_instruction(mb);
    opcode = mb->cpu.regs.IR;
    rdata = read_data_from_low_op(opcode & 0xF, mb, regs);
    PREFIX_INSTRUCTIONS_PTR[opcode](opcode, mb, regs, rdata);
}

void invalid(byte opcode, motherboard *, registers *)
{
    fprintf(stderr, "Error: invalid instruction: %X\n", opcode);
}



void rlc(byte opcode, motherboard *mb, registers *regs, byte rdata)
{
    uint8_t to_write;

    fprintf(stderr, "RLC OK\n");
    regs->c = rdata >> 7;
    to_write = (rdata << 1) | regs->c;
    write_data_from_low_op(opcode & 0xF, mb, regs, to_write);
    regs->z = UPDATE_Z(to_write);
    regs->n = 0;
    regs->h = 0;
}

void rrc(byte opcode, motherboard *mb, registers *regs, byte rdata)
{
    uint8_t to_write;

    fprintf(stderr, "RRC OK\n");
    regs->c = rdata & 1;
    to_write = (rdata >> 1) | (regs->c << 7);
    write_data_from_low_op(opcode & 0xF, mb, regs, to_write);
    regs->z = UPDATE_Z(to_write);
    regs->n = 0;
    regs->h = 0;
}

void rl(byte opcode, motherboard *mb, registers *regs, byte rdata)
{
    byte carry = (rdata >> 7);
    uint8_t to_write = (rdata << 1) | regs->c;

    fprintf(stderr, "RL OK\n");
    write_data_from_low_op(opcode & 0xF, mb, regs, to_write);
    regs->z = UPDATE_Z(to_write);
    regs->n = 0;
    regs->h = 0;
    regs->c = carry;
}

void rr(byte opcode, motherboard *mb, registers *regs, byte rdata)
{
    byte carry = rdata & 1;
    uint8_t to_write = (rdata >> 1) | (regs->c << 7);

    fprintf(stderr, "RR OK\n");
    write_data_from_low_op(opcode & 0xF, mb, regs, to_write);
    regs->z = UPDATE_Z(to_write);
    regs->n = 0;
    regs->h = 0;
    regs->c = carry;
}

void sla(byte opcode, motherboard *mb, registers *regs, byte rdata)
{
    fprintf(stderr, "SLA KO\n");
    regs->n = 0;
    regs->h = 0;
    exit(0);
}

void sra(byte opcode, motherboard *mb, registers *regs, byte rdata)
{
    fprintf(stderr, "SRA KO\n");
    regs->n = 0;
    regs->h = 0;
    exit(0);
}

void swap(byte opcode, motherboard *mb, registers *regs, byte rdata)
{
    uint8_t to_write = (rdata >> 4) | (rdata << 4);

    fprintf(stderr, "SWAP OK\n");
    write_data_from_low_op(opcode & 0xF, mb, regs, to_write);
    regs->z = to_write == 0;
    regs->n = 0;
    regs->h = 0;
    regs->c = 0;
}

void srl(byte opcode, motherboard *mb, registers *regs, byte rdata)
{
    fprintf(stderr, "SRL KO\n");
    regs->n = 0;
    regs->h = 0;
    exit(0);
}

void bit(byte opcode, motherboard *mb, registers *regs, byte rdata)
{
    uint8_t bit_asked = ((opcode ^ 0x40) >> 3) | ((opcode >> 3) & 1);

    fprintf(stderr, "BIT OK: opcode %d -> bit asked %d of value %d\n", opcode, bit_asked, rdata);
    regs->z = !((rdata >> bit_asked) & 0b1);
    regs->n = 0;
    regs->h = 1;
}

void res(byte opcode, motherboard *mb, registers *regs, byte rdata)
{
    uint8_t bit_to_write = ((opcode ^ 0x80) >> 3) | ((opcode >> 3) & 1);
    uint8_t to_write;

    fprintf(stderr, "RES OK\n");
    to_write = rdata & ((0xFE << bit_to_write) | ((1 << bit_to_write) - 1));
    write_data_from_low_op(opcode & 0xF, mb, regs, to_write);
}

void set(byte opcode, motherboard *mb, registers *regs, byte rdata)
{
    uint8_t bit_to_write = ((opcode ^ 0xc0) >> 3) | ((opcode >> 3) & 1);

    fprintf(stderr, "SET OK\n");
    write_data_from_low_op(opcode & 0xF, mb, regs, rdata | bit_to_write);
}
