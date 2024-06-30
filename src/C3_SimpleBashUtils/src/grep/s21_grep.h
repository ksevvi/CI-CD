#ifndef S21_GREP_H
#define S21_GREP_H
#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  int e;
  int i;
  int v;
  int c;
  int l;
  int n;
  int h;
  int s;
  int f;
  int o;
} options;

void get_options(int argc, char *argv[], options *opts);
void output(options opts, FILE *file);
void get_files(int argc, char *argv[], char **files, int *count_files,
               char **templates, int templates_count, int template_files_count,
               char **template_files);
void get_templates(int *templates_count, char **templates,
                   int *template_files_count, char **template_files, int argc,
                   char **argv);
void file_handling(FILE *file, options *opts, int templates_count,
                   char **templates, int template_files_count,
                   char **template_files, char *filename, int files_count,
                   int *match_count);

char *pattern_search(char *source_line, char *expression, options opts,
                     regmatch_t *pmatch);
char *pattern_search_in_file(char *source_line, options *opts,
                             int templates_count, char **templates,
                             int template_files_count, char **template_files,
                             int line_number, char *filename, int files_count,
                             int *match_count);
void output_line(char *source_line, char *filename, int line_number,
                 int files_count, options opts);
char *output_with_flag_o(char *line, options *opts, int line_number,
                         char *filename, int files_count, int *switch_n,
                         char *temp, int flag, char *template_position);
void parsing_teplates(int *templates_count, char **templates, int argc,
                      char **argv, int i, int j, char flag);
int check_file_for_a_templates(char *argv[], char **templates,
                               int templates_count, int i,
                               int template_files_count, char **template_files);
#endif  // S21_GREP_H