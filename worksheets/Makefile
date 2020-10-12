CXX = clang++
CXXFLAGS = -I.. -std=c++20 -pedantic -ggdb

SRCS = $(wildcard worksheet_?.cpp)
EXES = $(patsubst %.cpp,%,${SRCS})

${EXES}: ${SRCS}
	${CXX} $< ${CXXFLAGS} -o $@

all: ${EXES}

clean:
	rm ${EXES}

.PHONY: all clean
