#include <fstream>
#include <exception>

#include "Motherboard.hpp"

Motherboard::Motherboard(const std::string &bootrom_path)
    :_bootrom(bootrom_path)
{
    std::ifstream bootrom_stream(bootrom_path, std::ios::in | std::ios::binary);
    if (!bootrom_stream.is_open())
        throw std::runtime_error("Failed to open bootrom");

    bootrom_stream.read(_bootrom.memory.data, BOOT_ROM_SIZE);
    if (bootrom_stream.gcount() != BOOT_ROM_SIZE) {
        std::cerr << "Boot ROM corrupted: expected " << BOOT_ROM_SIZE << "bytes, read " << n << std::endl;
        throw std::runtime_error("Invalid bootrom");
    }
}
