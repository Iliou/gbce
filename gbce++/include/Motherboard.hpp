#ifndef MOTHERBOARD_HPP_
    #define MOTHERBOARD_HPP_

    #include <memory>

    #include "Cpu.hpp"
    #include "Memory.hpp"
    #include "Screen.hpp"
    #include "Cartridge.hpp"

class Motherboard {
public:
    Motherboard(const std::string &boot_rom);
    ~Motherboard() = default;

    void load_cartridge(const std::string &rom);

private:
    Cpu _cpu;
    Memory _wram;
    Screen _screen;
    std::unique_ptr<Cartridge> _cart;
    Memory _bootrom;
};

#endif
