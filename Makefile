include config.mk

SRC = formula.cpp
OBJ = ${SRC:.cpp=.o}

all: options libformula.a 

options:
	@echo libformula build options:
	@echo "CXX      = ${CXX}"
	@echo "CXXFLAGS = ${CXXFLAGS}"
	@echo "AR       = ${AR}"
	@echo "RANLIB   = ${RANLIB}"

.cpp.o:
	${CXX} -c ${CXXFLAGS} $< 

${OBJ}: formula.hpp config.mk

libformula.a: ${OBJ}
	$(AR) rc $@ $?
	$(RANLIB) $@

clean:
	rm libformula.a ${OBJ}

.PHONY: all clean
