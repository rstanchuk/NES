CFLAGS = -std=c++17 -stdlib=libc++
OLCFLAGS = -w -I/usr/X11/include -L/usr/X11/lib -lX11 -lGL -lpng -lpthread

all:
	g++ ${CFLAGS} ${OLCFLAGS} main.cpp  ./6502/bus.cpp ./6502/cpu6502.cpp \
	./2C02/ppu2C02.cpp ./cartridge/cartridge.cpp ./mapper/mapper.cpp \
	./mapper/mapper000.cpp -o nes

clean:
	find . -name '*.o' -delete