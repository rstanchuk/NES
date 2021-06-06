#ifndef BUS_HPP
#define BUS_HPP

#include <cstdint>
#include <array>

#include "cpu6502.hpp"
#include "../2C02/ppu2C02.hpp"
#include "../cartridge/cartridge.hpp"

class Bus {
    public:
        Bus();
        ~Bus();

    public:
        cpu6502 cpu;
        ppu2C02 ppu;
        uint8_t cpuRam[2048];
        std::shared_ptr<Cartridge> cart;
        uint8_t controller[2];

    public:
        void cpuWrite(uint16_t addr, uint8_t data);
        uint8_t cpuRead(uint16_t addr, bool bReadOnly = false);

        void insertCartridge(const std::shared_ptr<Cartridge>& cartridge);
        void reset();
        void clock();

    private:
        uint32_t nSystemClockCounter = 0; //counts how many clocks have passed
        uint8_t controller_state[2];
};

#endif