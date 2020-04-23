${CC:=gcc} -fsanitize=address -std=c99 -g -O0 main.c cbigint.c -o test_bigint -lm -Wall
