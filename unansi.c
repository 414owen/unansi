#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define HALF_BUF_SIZE (4096 * 4)
#define BUF_SIZE (HALF_BUF_SIZE * 2)

int stdin_fd;
size_t offset;
size_t amt;
char buffer[BUF_SIZE];

static char
skip_until_control(void) {
  for (; offset < amt; offset++) {
    char c = buffer[offset];
    switch (c) {
      case 0x07: // bel
      case 0x08: // backspace
      case 0x0c: // form feed
      case 0x7f: // delete
      case 0x1b: // escape
        return c;
      default:
        continue;
    }
  }
  return 0;
}

static bool
skip_until_alpha(void) {
  for (; offset < amt; offset++) {
    if (isalpha(buffer[offset])) {
      offset++;
      return true;
    }
  }
  return false;
}

enum state {
  S_NORMAL,
  S_POST_ESCAPE
};

static void
fill_buffer() {
  offset = 0;
  amt = read(stdin_fd, buffer, BUF_SIZE);
  if (amt == 0) {
    exit(0);
  }
}

int main(void) {
  stdin_fd = fileno(stdin);
  int stdout_fd = fileno(stdout);

start_normal_chunk:
  fflush(stdout);
  fill_buffer();
start_normal:
  {
    size_t start = offset;
    char c = skip_until_control();
    if (offset - start >= HALF_BUF_SIZE) {
      fflush(stdout);
      write(stdout_fd, &buffer[start], offset - start);
    } else {
      fwrite(&buffer[start], 1, offset - start, stdout);
    }
    switch (c) {
      case 0: goto start_normal_chunk;
      case 0x1b: goto start_escape;
      default: goto start_normal;
    }
    goto start_normal_chunk;
  }

start_escape_chunk:
  fflush(stdout);
  fill_buffer();
start_escape:
  {
    size_t start = offset;
    if (skip_until_alpha()) {
      goto start_normal;
    } else {
      goto start_escape_chunk;
    }
  }
}
