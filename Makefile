
CFLAGS=-std=c99 -g -O0 -fsanitize=address -c -Wall
COMPILE=gcc -std=c99 -g -O0 -fsanitize=address -c -Wall
LINK=gcc -std=c99 -g -O0 -fsanitize=address -o


HEADERS=cbigint.h cvector_long.h
SOURCES=main.c main_simple.c main_arithmetic.c cbigint.c
OBJECTS=main.o main_simple.o main_arithmetic.o cbigint.o

all: test_bigint arithmetic simple_tests

test_bigint: main.o
	$(LINK) test_bigint main.o cbigint.o -lcunit

arithmetic: main_arithmetic.o
	$(LINK) arithmetic main_arithmetic.o cbigint.o

simple_tests: main_simple.o
	$(LINK) simple_tests main_simple.o cbigint.o

$(OBJECTS): $(SOURCES) $(HEADERS)
	$(COMPILE) $(SOURCES)

clean:
	rm test_bigint arithmetic simple_tests *.o
