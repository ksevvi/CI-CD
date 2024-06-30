#include "s21_cat.h"

int main(int argc, char *argv[]) {
  options opts = {0};
  get_options(argc, argv, &opts);
  char *files[argc];
  int count_files = 0;
  get_files(argc, argv, files, &count_files);
  if (count_files == 0) {
    output(opts, stdin);
  } else {
    FILE *file = NULL;
    for (int i = 0; i < count_files; i++) {
      file = fopen(files[i], "r");
      if (!file)
        fprintf(stderr, "s21_cat: %s: No such file or directory\n", argv[i]);
      else {
        output(opts, file);
        fclose(file);
      }
    }
  }
  return 0;
}

void get_options(int argc, char *argv[], options *opts) {
  int opt;
  static struct option long_options[] = {{"number-nonblank", 0, 0, 'b'},
                                         {"number", 0, 0, 'n'},
                                         {"squeeze-blank", 0, 0, 's'},
                                         {0, 0, 0, 0}};
  int options_index;
  while ((opt = getopt_long(argc, argv, "+benstvTE", long_options,
                            &options_index)) != -1) {
    switch (opt) {
      case 'b':
        opts->b = 1;
        break;
      case 'e':
        opts->e = 1;
        opts->v = 1;
        break;
      case 'n':
        opts->n = 1;
        break;
      case 's':
        opts->s = 1;
        break;
      case 't':
        opts->t = 1;
        opts->v = 1;
        break;
      case 'v':
        opts->v = 1;
        break;
      case 'E':
        opts->e = 1;
        break;
      case 'T':
        opts->t = 1;
        break;
      default:
        printf("usage: ./s21_cat [-benstuv] [file ...]\n");
        exit(1);
        break;
    }
  }
}

void output(options opts, FILE *file) {
  char temp[2] = {-1, -1};
  int count_enter = 0;
  int moment_s = 0;
  char c;
  int line_number = 1;
  int count = 0;
  while ((c = getc(file)) != -1) {
    count++;
    temp[0] = temp[1];
    temp[1] = c;
    if (opts.s) {
      if (temp[0] == -1 && temp[1] == '\n' && count_enter == 0)
        moment_s = 1;
      else if (temp[0] == '\n' && !moment_s)
        moment_s = 1;
      else if (temp[1] != '\n')
        moment_s = 0;
      else if (temp[0] == '\n' && moment_s)
        continue;
    }
    if (opts.b) {
      if (count_enter == 0 && line_number == 1 && c != '\n') {
        printf("%6d\t", line_number);
        line_number += 1;
      } else if (c != '\n' && temp[0] == '\n') {
        printf("%6d\t", line_number);
        line_number += 1;
      }
    } else if (opts.n) {
      if (count_enter == 0 && line_number == 1) {
        printf("%6d\t", line_number);
        line_number += 1;
      }
      if (temp[0] == '\n') {
        printf("%6d\t", line_number);
        line_number += 1;
      }
    }
    if (opts.v) {
      if ((c >= 0 && c < 32 && c != 10 && c != 9) || c == 127) {
        if (c == 127) {
          printf("^?");
          continue;
        } else {
          printf("^%c", (c + 64));
          continue;
        }
      }
    }
    if (opts.e) {
      if (c == '\n') printf("$");
    }
    if (opts.t) {
      if (c == '\t') {
        printf("^I");
        continue;
      }
    }
    if (c == '\n') count_enter++;
    printf("%c", c);
    if (count == 2) break;
  }
}

void get_files(int argc, char *argv[], char **files, int *count_files) {
  for (int i = 1; i < argc; i++) {
    if (argv[i][0] != '-') {
      files[*count_files] = argv[i];
      *count_files += 1;
    }
  }
}