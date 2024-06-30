#include "s21_grep.h"

int main(int argc, char *argv[]) {
  options opts = {0};
  char *files[argc], *templates[argc], *templates_files[argc];
  int count_files = 0, templates_count = 0, template_files_count = 0,
      match_count = 0;
  get_templates(&templates_count, templates, &template_files_count,
                templates_files, argc, argv);
  get_options(argc, argv, &opts);
  get_files(argc, argv, files, &count_files, templates, templates_count,
            template_files_count, templates_files);
  if (!templates_count && !template_files_count) {
    fprintf(stderr, "s21_grep: No template\n");
    exit(1);
  }
  if (!count_files) {
    file_handling(stdin, &opts, template_files_count, templates,
                  template_files_count, templates_files, 0, 0, &match_count);
  } else {
    FILE *file = NULL;
    for (int i = 0; i < count_files; i++) {
      file = fopen(files[i], "r");
      if (file) {
        file_handling(file, &opts, templates_count, templates,
                      template_files_count, templates_files, files[i],
                      count_files, &match_count);
        fclose(file);
        match_count = 0;
      } else {
        if (!opts.s)
          fprintf(stderr, "s21_grep: %s: No such file or directory\n",
                  files[i]);
      }
    }
  }
  return 0;
}

void file_handling(FILE *file, options *opts, int templates_count,
                   char **templates, int template_files_count,
                   char **template_files, char *filename, int files_count,
                   int *match_count) {
  size_t length;
  int line_number = 1, end_line;
  char *line_from_file = NULL;
  while ((end_line = getline(&line_from_file, &length, file)) != -1) {
    int check_match = 0;
    char *match_part = pattern_search_in_file(
        line_from_file, opts, templates_count, templates, template_files_count,
        template_files, line_number, filename, files_count, match_count);
    if (match_part == NULL)
      check_match = 0;
    else {
      check_match = 1;
    }
    if (((check_match && !opts->v && !opts->o) || (!check_match && opts->v))) {
      *match_count += 1;
      if (!opts->c && !opts->l) {
        output_line(line_from_file, filename, line_number, files_count, *opts);
      }
    }
    line_number++;
  }
  free(line_from_file);

  if (opts->c) {
    if (opts->l) *match_count = (*match_count > 0) ? 1 : 0;
    if (files_count == 1 || opts->h)
      fprintf(stdout, "%d\n", *match_count);
    else
      fprintf(stdout, "%s:%d\n", filename, *match_count);
  }
  if (opts->l) {
    if (*match_count) {
      fprintf(stdout, "%s\n", filename);
    }
  }
}

void output_line(char *source_line, char *filename, int line_number,
                 int files_count, options opts) {
  if (!opts.c && !opts.l) {
    if (files_count == 1 || opts.h) {
      if (opts.n)
        fprintf(stdout, "%d:%s", line_number, source_line);
      else
        fprintf(stdout, "%s", source_line);
    } else {
      if (opts.n)
        fprintf(stdout, "%s:%d:%s", filename, line_number, source_line);
      else
        fprintf(stdout, "%s:%s", filename, source_line);
    }
    if (source_line[strlen(source_line) - 1] != '\n') printf("\n");
  }
}

char *pattern_search_in_file(char *source_line, options *opts,
                             int templates_count, char **templates,
                             int template_files_count, char **template_files,
                             int line_number, char *filename, int files_count,
                             int *match_count) {
  char *result = NULL;
  size_t length;
  regmatch_t pmatch[strlen(source_line)];
  char *template_from_file = NULL;
  int switch_n = 0;
  char *template_position = NULL;
  int mass[2] = {-1, -1};
  for (int i = 0; i < templates_count; i++) {
    result = pattern_search(source_line, templates[i], *opts, pmatch);
    if ((result != NULL && !opts->o) || (result != NULL && opts->v))
      break;
    else if (result != NULL && opts->o) {
      mass[0] = mass[1];
      mass[1] = i;
      if (template_position == NULL) *match_count += 1;
      char temp[strlen(source_line)];
      int len = pmatch[0].rm_eo - pmatch[0].rm_so;
      memcpy(temp, source_line + pmatch[0].rm_so, len);
      temp[len] = 0;
      if ((template_position != NULL && strstr(template_position, temp)) ||
          template_position == NULL) {
        int flag = 0;
        if (templates[i][0] == '^') flag = 1;
        template_position =
            output_with_flag_o(result, opts, line_number, filename, files_count,
                               &switch_n, temp, flag, template_position);
      }
    }
  }
  if (result == NULL) {
    for (int i = 0; i < template_files_count; i++) {
      FILE *file = fopen(template_files[i], "r");
      int c;
      while ((c = getline(&template_from_file, &length, file)) != EOF) {
        if (template_from_file[strlen(template_from_file) - 1] == '\n') {
          template_from_file[strlen(template_from_file) - 1] = '\0';
        }
        result = pattern_search(source_line, template_from_file, *opts, pmatch);
        if ((result != NULL && !opts->o) || (result != NULL && opts->v)) {
          break;
        } else if (result != NULL && opts->o) {
          if (template_position == NULL) *match_count += 1;
          char temp[strlen(source_line)];
          int len = pmatch[0].rm_eo - pmatch[0].rm_so;
          memcpy(temp, source_line + pmatch[0].rm_so, len);
          temp[len] = 0;
          if ((template_position != NULL && strstr(template_position, temp)) ||
              template_position == NULL) {
            int flag = 0;
            if (template_from_file[0] == '^') flag = 1;
            template_position = output_with_flag_o(
                result, opts, line_number, filename, files_count, &switch_n,
                temp, flag, template_position);
          }
        }
      }
      free(template_from_file);
      fclose(file);
    }
  }

  if (switch_n == 1 && opts->n == 0) {
    opts->n = 1;
    switch_n = 0;
  }
  return result;
}

char *output_with_flag_o(char *line, options *opts, int line_number,
                         char *filename, int files_count, int *switch_n,
                         char *result, int flag, char *template_position) {
  char *new_line = NULL;
  char *this_line = NULL;
  if (template_position != NULL)
    this_line = template_position;
  else
    this_line = line;
  while ((template_position = strstr(this_line, result)) != NULL) {
    if (strstr(template_position, result) != NULL) *template_position = '.';
    output_line(result, filename, line_number, files_count, *opts);
    if (*switch_n == 0 && opts->n) {
      opts->n = 0;
      *switch_n = 1;
    }
    new_line = template_position;
    if (flag) break;
  }
  return new_line;
}

char *pattern_search(char *source_line, char *expression, options opts,
                     regmatch_t *pmatch) {
  char *result = NULL;
  if (source_line[0] != 0 && (source_line[0] != '\n' || opts.f)) {
    regex_t regex_expression;
    int error = 0;
    char message[128];
    int len = strlen(source_line);
    if (opts.i) {
      error = regcomp(&regex_expression, expression, REG_EXTENDED | REG_ICASE);
    } else {
      error = regcomp(&regex_expression, expression, REG_EXTENDED);
    }
    if (error != 0) {
      regerror(error, &regex_expression, message, 128);
      printf("%s\n", message);
      exit(1);
    }
    if ((error = regexec(&regex_expression, source_line, len, pmatch, 0)) ==
        0) {
      result = source_line;
    } else if (error != REG_NOMATCH) {
      regerror(error, &regex_expression, message, 128);
      exit(1);
    }
    regfree(&regex_expression);
  }
  return result;
}

void get_options(int argc, char *argv[], options *opts) {
  int opt, options_index;
  static struct option long_options[] = {{0, 0, 0, 0}};
  while ((opt = getopt_long(argc, argv, "e:ivclnhsf:o", long_options,
                            &options_index)) != -1) {
    switch (opt) {
      case 'e':
        opts->e = 1;
        break;
      case 'i':
        opts->i = 1;
        break;
      case 'v':
        opts->v = 1;
        break;
      case 'c':
        opts->c = 1;
        break;
      case 'l':
        opts->l = 1;
        break;
      case 'n':
        opts->n = 1;
        break;
      case 'h':
        opts->h = 1;
        break;
      case 's':
        opts->s = 1;
        break;
      case 'f':
        opts->f = 1;
        break;
      case 'o':
        opts->o = 1;
        break;
      default:
        fprintf(stdin, "s21_grep: invalid option -- '%c'", opt);
        exit(1);
        break;
    }
  }
}

void parsing_teplates(int *templates_count, char **templates, int argc,
                      char **argv, int i, int j, char flag) {
  if (argv[i][j + 1] == 0) {
    if (i + 1 >= argc) {
      fprintf(stderr, "s21_grep: option requires an argument -- '%c'", flag);
      exit(1);
    } else {
      templates[*templates_count] = argv[i + 1];
      *templates_count += 1;
    }
  } else {
    templates[*templates_count] = 0;
    templates[*templates_count] = argv[i] + 1 + j;
    *templates_count += 1;
  }
}

void get_templates(int *templates_count, char **templates,
                   int *template_files_count, char **template_files, int argc,
                   char **argv) {
  int j;
  char flag;
  for (int i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {
      j = 1;
      while (argv[i][j] != 'e' && argv[i][j] != 'f' && argv[i][j] != '\0') j++;
      if (argv[i][j] == 'e') {
        flag = 'e';
        parsing_teplates(templates_count, templates, argc, argv, i, j, flag);
      } else if (argv[i][j] == 'f') {
        flag = 'f';
        parsing_teplates(template_files_count, template_files, argc, argv, i, j,
                         flag);
      }
    }
  }
  if (*templates_count == 0 && *template_files_count == 0) {
    for (int i = 1; i < argc; i++) {
      if (argv[i][0] != '-' && argv[i][0] != 0) {
        templates[*templates_count] = argv[i];
        *templates_count += 1;
        break;
      }
    }
  }
  for (int i = 0; i < *template_files_count; i++) {
    FILE *file = fopen(template_files[i], "r");
    if (file == NULL) {
      fprintf(stderr, "grep: %s: No such file or directory\n",
              template_files[i]);
      exit(1);
    }
    fclose(file);
  }
}

void get_files(int argc, char *argv[], char **files, int *count_files,
               char **templates, int templates_count, int template_files_count,
               char **template_files) {
  for (int i = 1; i < argc; i++) {
    if (argv[i][0] != '-') {
      int check_on_template =
          check_file_for_a_templates(argv, templates, templates_count, i,
                                     template_files_count, template_files);
      if (check_on_template == 0) {
        files[*count_files] = argv[i];
        *count_files += 1;
      }
    }
  }
}
int check_file_for_a_templates(char *argv[], char **templates,
                               int templates_count, int i,
                               int template_files_count,
                               char **template_files) {
  int check_on_template = 0;
  for (int j = 0; j < templates_count; j++) {
    if (argv[i] == templates[j]) {
      check_on_template = 1;
      break;
    }
  }
  for (int j = 0; j < template_files_count; j++) {
    if (argv[i] == template_files[j]) {
      check_on_template = 1;
      break;
    }
  }
  return check_on_template;
}
