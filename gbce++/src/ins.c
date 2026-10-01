#include <stdio.h>
#include <stdint.h>

#include "tools.h"
#include "cpu.h"
#include "ins.h"

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

static inline a16_t memory_load16(byte const *memory)
{
    return (memory[1] << 8) | memory[0];
}

// load
void ld8_regs(byte opcode, motherboard *mb, registers *regs)
{
    uint8_t low_op = opcode & 0x0F;
    uint8_t hi_op = opcode >> 4;
    uint8_t data = low_op == 0x6 || low_op == 0xE ? *map_address(mb, regs->HL, READ) : regs->general_regs8[(2 + (low_op & 7)) % 9];
    byte *dest = hi_op == 7 && low_op < 8 ? map_address(mb, regs->HL, WRITE) : &regs->general_regs8[hi_op - 2 + low_op / 8];

    fprintf(stderr, "ld8 (regs) OK\n");
    *dest = data;
}

void ld8(byte opcode, motherboard *mb, registers *regs)
{
    uint8_t low_op = opcode & 0x0F;
    uint8_t hi_op = opcode >> 4;
    uint8_t data;
    byte *dest;

    fprintf(stderr, "ld8 OK\n");
    switch (opcode) {
    case 0xEA:
        dest = map_address(mb, memory_load16(map_address(mb, regs->PC, READ)), WRITE);
        regs->PC += 2;
        *dest = regs->A;
        return;
    case 0xFA:
        dest = map_address(mb, memory_load16(map_address(mb, regs->PC, READ)), READ);
        regs->PC += 2;
        regs->A = *dest;
        return;
    case 0x36:
        dest = map_address(mb, regs->HL, WRITE);
        data = *map_address(mb, regs->PC, READ);
        *dest = data;
        return;
    }
    switch (low_op) {
    case 0x2:
        data = regs->A;
        switch (hi_op) {
        case 0:
            dest = map_address(mb, regs->BC, WRITE);
            break;
        case 1:
            dest = map_address(mb, regs->DE, WRITE);
            break;
        case 2:
            printf("before\n");
            dest = map_address(mb, regs->HL++, WRITE);
            printf("after\n");
            break;
        case 3:
            dest = map_address(mb, regs->HL--, WRITE);
            break;
        }
        break;
    case 0xA:
        dest = &regs->A;
        switch (hi_op) {
        case 0:
            data = *map_address(mb, regs->BC, READ);
            break;
        case 1:
            data = *map_address(mb, regs->DE, READ);
            break;
        case 2:
            data = *map_address(mb, regs->HL++, READ);
            break;
        case 3:
            data = *map_address(mb, regs->HL--, READ);
            break;
        }
        break;
    case 0x6:
    case 0xE:
        data = *map_address(mb, regs->PC++, READ);
        switch (hi_op) {
        case 0 ... 2:
            dest = &regs->general_regs8[2 * (hi_op + 1) + (low_op >> 3)];
            break;
        case 3:
            dest = &regs->A; //special case 0x36 handled specifically earlier
            break;
        }
    }
    *dest = data;
}

void ld16(byte opcode, motherboard *mb, registers *regs)
{
    byte *address;

    fprintf(stderr, "ld16 OK\n");
    switch (opcode & 0x0F) {
    case 0x1:
        fprintf(stderr, "opcode: %x LOAD VALUE TO REGISTER\n", opcode);
        regs->general_regs[1 + (opcode >> 4)] = memory_load16(map_address(mb, regs->PC, READ));
        regs->PC += 2;
        break;
    case 0x8:
        switch (opcode >> 4) {
        case 0x0:
            address = map_address(mb, memory_load16(map_address(mb, regs->PC, READ)), WRITE);
            address[0] = regs->SP & 0xFF;
            address[1] = regs->SP >> 8;
            regs->PC += 2;
            break;
        case 0xF:
            regs->HL = regs->SP + mb->wram[regs->PC++];
            //TODO set flags h et c
            regs->z = 0;
            regs->n = 0;
            regs->h;
            regs->c;
            break;
        default:
            fprintf(stderr, "Opcode error: %X should not be a ld16", opcode);
            break;
        }
        break;
    case 0x9:
        regs->SP = regs->HL;
        fprintf(stderr, "Changed SP to HL value: %hu\n", regs->HL);
        break;
    default:
        fprintf(stderr, "Opcode error: %X should not be a ld16", opcode);
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
        load_address |= *map_address(mb, regs->PC++, READ);
        address = map_address(mb, load_address, WRITE);
        *address = regs->A;
        break;
    case 0xF0:
        load_address |= *map_address(mb, regs->PC++, READ);
        address = map_address(mb, load_address, READ);
        regs->A = *address;
        break;
    case 0xE2:
        load_address |= regs->C;
        address = map_address(mb, load_address, WRITE);
        *address = regs->A;
        break;
    case 0xF2:
        load_address |= regs->C;
        address = map_address(mb, load_address, READ);
        regs->A = *address;
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
    uint8_t low_op = opcode & 0x0F;
    n8_t data;

    fprintf(stderr, "add: %X OK\n", opcode);
    switch (low_op) {
    case 0x08:
        regs->SP += *map_address(mb, regs->PC++, READ);
        regs->z = 0;
        break;
    case 0x09:
        regs->HL += regs->general_regs[1 + (opcode >> 4)];
        break;
    default:
        switch (opcode >> 4) {
        case 0x8:
            switch (low_op) {
            case 0 ... 5:
                data = regs->general_regs8[2 + low_op];
                break;
            case 6:
                data = *map_address(mb, regs->HL, READ);
                break;
            case 7:
                data = regs->A;
                break;
            default:
                fprintf(stderr, "Opcode error: %X\n", opcode);
            }
            break;
        case 0xc:
            data = *map_address(mb, regs->PC++, READ);
            break;
        default:
            fprintf(stderr, "Opcode error: %X\n", opcode);
        }
        regs->A += data;
        regs->z = UPDATE_Z(regs->A);
    }
    regs->n = 0;
    regs->h; //TODO
    regs->c; //TODO
}

void adc(byte opcode, motherboard *mb, registers *regs)
{
    fprintf(stderr, "adc KO\n");
}

void sub(byte opcode, motherboard *mb, registers *regs)
{
    fprintf(stderr, "sub KO\n");
}

void sbc(byte opcode, motherboard *mb, registers *regs)
{
    fprintf(stderr, "sbc KO\n");
}

void cp(byte opcode, motherboard *mb, registers *regs)
{
    fprintf(stderr, "cp KO\n");
}

void inc8(byte opcode, motherboard *mb, registers *regs)
{
    fprintf(stderr, "inc8 OK\n");
    switch (opcode) {
    case 0x34:
        regs->z = UPDATE_Z(++*map_address(mb, regs->HL, WRITE));
        break;
    case 0x3C:
        regs->z = UPDATE_Z(++regs->A);
        break;
    default:
        regs->z = UPDATE_Z(++regs->general_regs8[2 + (opcode >> 3)]);
    }
    regs->n = 0;
    regs->h; //TODO
}

void inc16(byte opcode, motherboard *mb, registers *regs)
{
    fprintf(stderr, "inc16 OK\n");
    regs->general_regs[1 + (opcode >> 4)]++;
}

void dec8(byte opcode, motherboard *mb, registers *regs)
{
    fprintf(stderr, "dec8 OK\n");
    switch (opcode) {
    case 0x35:
        regs->z = UPDATE_Z(--*map_address(mb, regs->HL, WRITE));
        break;
    case 0x3D:
        regs->z = UPDATE_Z(--regs->A);
        break;
    default:
        regs->z = UPDATE_Z(--regs->general_regs8[2 + (opcode >> 3)]);
    }
    regs->n = 1;
    regs->h; //TODO
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
    case 0xA0 ... 0xA5:
        data = regs->general_regs8[(opcode & 0xF) + 2];
        break;
    case 0xA6:
        data = *map_address(mb, regs->HL, READ);
        break;
    case 0xA7:
        data = regs->A;
        break;
    case 0xE6:
        data = *map_address(mb, regs->PC++, READ);
        break;
    }
    regs->A ^= data;
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
    case 0xB0 ... 0xB5:
        data = regs->general_regs8[(opcode & 0xF) + 2];
        break;
    case 0xB6:
        data = *map_address(mb, regs->HL, READ);
        break;
    case 0xB7:
        data = regs->A;
        break;
    case 0xF6:
        data = *map_address(mb, regs->PC++, READ);
        break;
    }
    regs->A ^= data;
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
    case 0xA8 ... 0xAD:
        data = regs->general_regs8[(opcode & 0xF) - 6];
        break;
    case 0xAE:
        data = *map_address(mb, regs->HL, READ);
        break;
    case 0xAF:
        data = regs->A;
        break;
    case 0xEE:
        data = *map_address(mb, regs->PC++, READ);
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
    fprintf(stderr, "ccf KO\n");
}

void scf(byte opcode, motherboard *mb, registers *regs)
{
    fprintf(stderr, "scf KO\n");
}

void daa(byte opcode, motherboard *mb, registers *regs)
{
    fprintf(stderr, "daa KO\n");
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
    fprintf(stderr, "rlca KO\n");
}

void rrca(byte opcode, motherboard *mb, registers *regs)
{
    fprintf(stderr, "rrca KO\n");
}

void rla(byte opcode, motherboard *mb, registers *regs)
{
    fprintf(stderr, "rla KO\n");
}

void rra(byte opcode, motherboard *mb, registers *regs)
{
    fprintf(stderr, "rra KO\n");
}


// Control flow
void cond_jp(byte opcode, motherboard *mb, registers *regs)
{
    a16_t address = memory_load16(map_address(mb, regs->PC, READ));
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
        regs->PC = memory_load16(map_address(mb, regs->PC, READ));
    } else { // opcode == 0xE9
        regs->PC = regs->HL;
    }
    fprintf(stderr, "%X OK\n", regs->PC);
}

void jr(byte opcode, motherboard *mb, registers *regs)
{
    bool eval = opcode & 8;
    e8_t offset = *map_address(mb, regs->PC, READ);

    regs->PC++;
    fprintf(stderr, "jr: offset %d OK\n", offset);
    switch (opcode >> 4) {
    case 1:
        regs->PC += offset;
        break;
    case 2:
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
    a16_t jump_address = memory_load16(map_address(mb, regs->PC, READ));

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
}

void rst(byte opcode, motherboard *mb, registers *regs)
{
    fprintf(stderr, "rst KO\n");
}


// misc
void nop(byte opcode, motherboard *mb, registers *regs)
{
    fprintf(stderr, "nop OK\n");
}

void stop(byte opcode, motherboard *mb, registers *regs)
{
    fprintf(stderr, "stop KO\n");
}

void halt(byte opcode, motherboard *mb, registers *regs)
{
    fprintf(stderr, "halt KO\n");
}

void di(byte opcode, motherboard *mb, registers *regs)
{
    fprintf(stderr, "di KO\n");
}

void ei(byte opcode, motherboard *mb, registers *regs)
{
    fprintf(stderr, "ei KO\n");
}

void prefix(byte opcode, motherboard *mb, registers *regs)
{
    byte *data;
    byte low_op;

    fprintf(stderr, "Prefix\n");
    load_next_instruction(mb);
    opcode = mb->cpu.regs.IR;
    low_op = opcode & 0xF;
    if (low_op == 0x6 || low_op == 0xE) {
        data = map_address(mb, regs->HL, WRITE);
    } else {
        data = &regs->general_regs8[((low_op & 7) + 2) % 9];
    }
    PREFIX_INSTRUCTIONS_PTR[opcode](opcode, regs, data, ((opcode >> 3) & 0b110) + !!low_op);
}

void invalid(byte opcode, motherboard *mb, registers *regs)
{
    fprintf(stderr, "Error: invalid instruction: %X\n", opcode);
}





void rlc(byte opcode, registers *regs, byte *data, uint8_t)
{
    fprintf(stderr, "RLC OK\n");
    regs->c = (*data) >> 7;
    *data = (*data << 1) | regs->c;
    regs->z = UPDATE_Z(*data);
    regs->n = 0;
    regs->h = 0;
}

void rrc(byte opcode, registers *regs, byte *data, uint8_t)
{
    fprintf(stderr, "RRC OK\n");
    regs->c = (*data) & 1;
    *data = (*data >> 1) | (regs->c << 7);
    regs->z = UPDATE_Z(*data);
    regs->n = 0;
    regs->h = 0;
}

void rl(byte opcode, registers *regs, byte *data, uint8_t)
{
    fprintf(stderr, "RL OK\n");
    byte carry = (*data >> 7);

    *data = (*data << 1) | regs->c;
    regs->z = UPDATE_Z(*data);
    regs->n = 0;
    regs->h = 0;
    regs->c = carry;
}

void rr(byte opcode, registers *regs, byte *data, uint8_t)
{
    fprintf(stderr, "RR OK\n");
    byte carry = (*data) & 1;

    *data = (*data >> 1) | (regs->c << 7);
    regs->z = UPDATE_Z(*data);
    regs->n = 0;
    regs->h = 0;
    regs->c = carry;
}

void sla(byte opcode, registers *regs, byte *data, uint8_t)
{
    fprintf(stderr, "SLA KO\n");
}

void sra(byte opcode, registers *regs, byte *data, uint8_t)
{
    fprintf(stderr, "SRA KO\n");
}

void swap(byte opcode, registers *regs, byte *data, uint8_t)
{
    fprintf(stderr, "SWAP KO\n");
}

void srl(byte opcode, registers *regs, byte *data, uint8_t)
{
    fprintf(stderr, "SRL KO\n");
}

void bit(byte opcode, registers *regs, byte *data, uint8_t value)
{
    fprintf(stderr, "BIT OK\n");
    regs->z = !(((*data) >> value) & 0b1);
    regs->n = 0;
    regs->h = 1;
}

void res(byte opcode, registers *regs, byte *data, uint8_t value)
{
    fprintf(stderr, "RES OK\n");
    *data &= ((0xFE << value) | ((1 << value) - 1));
}

void set(byte opcode, registers *regs, byte *data, uint8_t value)
{
    fprintf(stderr, "SET OK\n");
    *data |= (1 << value);
}
