#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void *sbrk(intptr_t increment);

#define BUF_SIZE 128
#define BLOCK_SIZE 128

struct header {
  uint64_t size;
  struct header *next;
};

void handle_error(const char *msg) {
  perror(msg);
  _exit(1);
}

void print_uint64(const char *fmt, uint64_t val) {
  char buf[BUF_SIZE];
  ssize_t len = snprintf(buf, BUF_SIZE, fmt, val);
  write(STDOUT_FILENO, buf, len);
}

void print_ptr(const char *fmt, void *ptr) {
  char buf[BUF_SIZE];
  ssize_t len = snprintf(buf, BUF_SIZE, fmt, ptr);
  write(STDOUT_FILENO, buf, len);
}

int main(void) {
  void *heap_start = sbrk(256);
  if (heap_start == (void *)-1) {
    handle_error("sbrk");
  }

  struct header *first = (struct header *)heap_start;
  struct header *second = (struct header *)((char *)heap_start + BLOCK_SIZE);

  first->size = BLOCK_SIZE;
  first->next = NULL;

  second->size = BLOCK_SIZE;
  second->next = first;

  size_t header_size = sizeof(struct header);
  size_t data_size = BLOCK_SIZE - header_size;

  unsigned char *first_data = (unsigned char *)first + header_size;
  unsigned char *second_data = (unsigned char *)second + header_size;

  memset(first_data, 0, data_size);
  memset(second_data, 1, data_size);

  print_ptr("first block: %p\n", first);
  print_ptr("second block: %p\n", second);

  print_uint64("first block size: %lu\n", first->size);
  print_ptr("first block next: %p\n", first->next);

  print_uint64("second block size: %lu\n", second->size);
  print_ptr("second block next: %p\n", second->next);

  for (size_t i = 0; i < data_size; i++) {
    char c = first_data[i] ? '1' : '0';
    write(STDOUT_FILENO, &c, 1);
    write(STDOUT_FILENO, "\n", 1);
  }

  for (size_t i = 0; i < data_size; i++) {
    char c = second_data[i] ? '1' : '0';
    write(STDOUT_FILENO, &c, 1);
    write(STDOUT_FILENO, "\n", 1);
  }

  return 0;
}
