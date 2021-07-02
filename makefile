CFLAGS = -arch x86_64 -std=c++17 -mmacosx-version-min=11.0 -Wall -framework OpenGL \
		-framework GLUT -framework Carbon -lpng -lsdl2

all:
	clang++ ${CFLAGS} main.cpp 6502/bus.cpp 6502/cpu6502.cpp cartridge/cartridge.cpp \
	mapper/mapper000.cpp mapper/mapper.cpp 2C02/ppu2C02.cpp -o nes

clean:
	rm nes