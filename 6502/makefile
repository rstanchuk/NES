CFLAGS = -std=c++17 -stdlib=libc++
OLCFLAGS = -I/usr/X11/include -L/usr/X11/lib -lX11 -lGL -lpng -lpthread

all:
	clang++ ${CFLAGS} -c cpu6502.cpp
	clang++ ${CFLAGS} -c bus.cpp
test:
	g++ ${CFLAGS} ${OLCFLAGS} cpu6502.cpp bus.cpp cpuTest.cpp -o test
clean:
	rm *.o test