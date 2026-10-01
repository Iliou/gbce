#include "Ppu.hpp"
#include "Register.hpp"
#include "Memory.hpp"

class Cpu {
public:
    Cpu() = default;
    ~Cpu() = default;

private:
    Memory _rom;
    Memory _vram;
    Memory _hram;
    Ppu _ppu;
    Register _regs;
    bool _ime;
};
