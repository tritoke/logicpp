CXX = clang++
CXXFLAGS = -I.. -std=c++20 -pedantic -ggdb
LDFLAGS = -L.. -lformula

SRCS = $(wildcard *.cpp)
EXES = $(patsubst %.cpp,%,${SRCS})

all: ${EXES}

# all executables depend on libformula
${EXES}: ../libformula.a

# each execuable is built from a source C++ file
# this ensures only `a` is built when a.cpp changes
%: %.cpp
	${CXX} $< ${CXXFLAGS} ${LDFLAGS} -o $@

clean:
	rm ${EXES}

.PHONY: all clean
