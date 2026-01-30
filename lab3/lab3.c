#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HISTORY_SIZE 5

static void free_history(char *history[], size_t n) {
  for (size_t i = 0; i < n; i++) {
    free(history[i]);
    history[i] = NULL;
  }
}

static char *dup_string(const char *s) {
  size_t len = strlen(s);
  char *copy = (char *)malloc(len + 1);
  if (!copy)
    return NULL;
  memcpy(copy, s, len + 1);
  return copy;
}

static void add_line(char *history[], size_t *count, const char *line) {
  size_t idx = (*count) % HISTORY_SIZE;

  free(history[idx]);
  history[idx] = NULL;

  history[idx] = dup_string(line);
  if (!history[idx]) {
    fprintf(stderr, "Error: out of memory while storing input.\n");
    exit(1);
  }

  (*count)++;
}

static void print_last_lines(char *history[], size_t count) {
  size_t to_print = (count < HISTORY_SIZE) ? count : HISTORY_SIZE;

  if (to_print == 0)
    return;

  size_t start = (count < HISTORY_SIZE) ? 0 : (count % HISTORY_SIZE);

  for (size_t i = 0; i < to_print; i++) {
    size_t idx = (start + i) % HISTORY_SIZE;
    if (history[idx]) {
      fputs(history[idx], stdout);
    }
  }
}

int main(void) {
  char *history[HISTORY_SIZE] = {0};
  size_t count = 0;

  char *line = NULL;
  size_t cap = 0;

  while (1) {
    printf("Enter input: ");
    fflush(stdout);

    ssize_t nread = getline(&line, &cap, stdin);
    if (nread == -1) {
      putchar('\n');
      break;
    }

    add_line(history, &count, line);

    if (strcmp(line, "print\n") == 0 || strcmp(line, "print") == 0) {
      print_last_lines(history, count);
    }
  }

  free(line);
  free_history(history, HISTORY_SIZE);
  return 0;
}
