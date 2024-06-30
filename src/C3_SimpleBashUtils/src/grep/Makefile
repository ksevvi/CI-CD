CC = gcc

all: s21_grep

s21_grep: s21_grep.c s21_grep.h
	$(CC) s21_grep.c -o s21_grep

rebuild:
	make clean
	make s21_grep

clean:
	rm -rf s21_grep 