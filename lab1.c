#define _POSIX_C_SOURCE 200809L
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
  char *line = NULL;
  size_t capacity = 0;
  for (;;) {
    printf("Please enter some text: ");
    ssize_t chars_read = getline(&line, &capacity, stdin);
    if (chars_read == -1) {
      printf("error reading line");
      break;
    }

    char *state = NULL;
    const char *seperators = " \t\n\r";

    for (char *part = strtok_r(line, seperators, &state); part != NULL;
         part = strtok_r(NULL, seperators, &state)) {
      printf("Token: %s\n", part);
    }
  }
  free(line);
  return 0;
}
