CC = gcc

all: s21_cat

s21_cat: s21_cat.c s21_cat.h
	$(CC) s21_cat.c -o s21_cat

rebuild:
	make clean
	make s21_cat

clean:
	rm -rf s21_cat 