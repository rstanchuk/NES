CFLAGS = -std=c++17 -lsdl2

all:
	clang++ ${CFLAGS} main.cpp 6502/bus.cpp 6502/cpu6502.cpp cartridge/cartridge.cpp \
	mapper/mapper000.cpp mapper/mapper.cpp 2C02/ppu2C02.cpp -o nes

clean:
	rm nes