CXXFLAGS = -o $@ -O0 -g -std=c++11 -lstdc++ -Wall -Wextra

all: gdb-segv gdb-segv-clang

clean:
	rm -f gdb-segv gdb-segv-clang

gdb-segv: gdb-segv.cpp
	g++ $< $(CXXFLAGS)

gdb-segv-clang: gdb-segv.cpp
	clang $< $(CXXFLAGS)
