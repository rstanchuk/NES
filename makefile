CFLAGS = -w -arch x86_64 -std=c++17 -mmacosx-version-min=10.15 -Wall -framework OpenGL \
		-framework GLUT -framework Carbon -lpng

all:
	clang++ ${CFLAGS} main.cpp  ./6502/bus.cpp ./6502/cpu6502.cpp \
	./2C02/ppu2C02.cpp ./cartridge/cartridge.cpp ./mapper/mapper.cpp \
	./mapper/mapper000.cpp -o nes

clean:
	find . -name '*.o' -delete && rm nes