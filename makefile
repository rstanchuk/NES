CFLAGS = -w -arch x86_64 -std=c++17 -mmacosx-version-min=10.15 -Wall -framework OpenGL \
		-framework GLUT -framework Carbon -lpng

all:
	(cd 2C02 && make && cd .. ) && \
	(cd 6502 && make && cd .. ) && \
	(cd cartridge && make && cd .. ) && \
	(cd mapper && make && cd .. ) && \
	clang++ ${CFLAGS} main.cpp  ./6502/bus.o ./6502/cpu6502.o \
	./2C02/ppu2C02.o ./cartridge/cartridge.o ./mapper/mapper.o \
	./mapper/mapper000.o -o nes

clean:
	find . -name '*.o' -delete && rm nes