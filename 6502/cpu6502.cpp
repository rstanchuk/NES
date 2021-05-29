#include <cstdint>

#include "cpu6502.hpp"
#include "bus.hpp"

cpu6502::cpu6502() {}

cpu6502::~cpu6502() {}

uint8_t cpu6502::read(uint16_t a) {
    return bus->read(a, false);
}

void cpu6502::write(uint16_t a, uint8_t d) {
    bus->write(a, d);
}