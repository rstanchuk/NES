CFLAGS = -std=c++17 -stdlib=libc++
OLCFLAGS = -w -I/usr/X11/include -L/usr/X11/lib -lX11 -lGL -lpng -lpthread

all:
	clang++ ${CFLAGS} ${OLCFLAGS} -c ppu2C02.cpp
clean:
	rm *.o