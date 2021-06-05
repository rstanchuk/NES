CFLAGS = -w -arch x86_64 -std=c++17 -mmacosx-version-min=10.15 -Wall -framework OpenGL \
		-framework GLUT -framework Carbon -lpng

all:
	clang++ ${CFLAGS} -c cpu6502.cpp
	clang++ ${CFLAGS} -c bus.cpp
clean:
	rm *.o test