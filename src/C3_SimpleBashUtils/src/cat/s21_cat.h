#ifndef S21_CAT_H
#define S21_CAT_H
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct {
  int b;
  int e;
  int n;
  int s;
  int t;
  int v;
} options;

void get_options(int argc, char *argv[], options *opts);
void output(options opts, FILE *file);
void get_files(int argc, char *argv[], char **files, int *count_files);

#endif  // S21_CAT_H