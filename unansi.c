#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
# define WIN32_LEAN_AND_MEAN
# include <windows.h>
#else
# include <unistd.h>
#endif

#define HALF_BUF_SIZE (4096 * 4)
#define BUF_SIZE (HALF_BUF_SIZE * 2)

int stdin_fd;
int stdout_fd;
size_t offset;
size_t amt;
char buffer[BUF_SIZE];

static void
fill_buffer() {
  offset = 0;
  amt = read(stdin_fd, buffer, BUF_SIZE);
  if (amt == 0) {
    exit(0);
  }
}

static void
write_out(size_t start) {
  if (offset - start >= HALF_BUF_SIZE) {
    fflush(stdout);
    write(stdout_fd, &buffer[start], offset - start);
  } else {
    fwrite(&buffer[start], 1, offset - start, stdout);
  }
}

int main(void) {
  stdin_fd = fileno(stdin);
  stdout_fd = fileno(stdout);

start_normal_chunk:
  fflush(stdout);
  fill_buffer();
start_normal:
  {
    size_t start = offset;
    for (; offset < amt; offset++) {
      char c = buffer[offset];
      switch (c) {
        case 0x07: // bel
        case 0x08: // backspace
        case 0x0c: // form feed
        case 0x7f: // delete
          write_out(start);
          goto start_normal;
        case 0x1b: // escape
          write_out(start);
          goto start_escape;
        default:
          continue;
      }
    }

    write_out(start);
    goto start_normal_chunk;
  }

start_escape_chunk:
  fflush(stdout);
  fill_buffer();
start_escape:
  {
    for (; offset < amt; offset++) {
      if (isalpha(buffer[offset])) {
        offset++;
        goto start_normal;
      }
    }
    goto start_escape_chunk;
  }
}
