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

struct state {
  size_t offset;
  size_t amt;
  char buffer[BUF_SIZE];
  int stdin_fd;
  int stdout_fd;
};

static inline void
fill_buffer(struct state *restrict state) {
  state->offset = 0;
  state->amt = read(state->stdin_fd, state->buffer, BUF_SIZE);
  if (state->amt == 0) {
    exit(0);
  }
}

static inline void
write_out(struct state *restrict state, size_t start) {
  if (state->offset - start >= HALF_BUF_SIZE) {
    // This branch avoids the excess buffering of using stdout as a
    // FILE*, generally in the cases where there's lots of input with
    // no ANSI codes.
    // I've tried making the `fflush` conditional on a `dirty` flag,
    // but that seems to have no effect on performance. Presumably
    // this is happening under the hood anyway.
    fflush(stdout);
    write(state->stdout_fd, &state->buffer[start], state->offset - start);
  } else {
    fwrite(&state->buffer[start], 1, state->offset - start, stdout);
  }
}

int main(void) {
  struct state state = {
    .stdin_fd = fileno(stdin),
    .stdout_fd = fileno(stdout),
    .offset = 0,
    .amt = 0,
  };

start_normal_chunk:
  fflush(stdout);
  fill_buffer(&state);
start_normal:
  {
    size_t start = state.offset;
    for (; state.offset < state.amt; state.offset++) {
      char c = state.buffer[state.offset];
      switch (c) {
        case 0x07: // bel
        case 0x08: // backspace
        case 0x0c: // form feed
        case 0x7f: // delete
          write_out(&state, start);
          goto start_normal;
        case 0x1b: // escape
          write_out(&state, start);
          goto start_escape;
        default:
          continue;
      }
    }
    write_out(&state, start);
    goto start_normal_chunk;
  }

start_escape_chunk:
  fflush(stdout);
  fill_buffer(&state);
start_escape:
  for (; state.offset < state.amt; state.offset++) {
    if (isalpha(state.buffer[state.offset])) {
      state.offset++;
      goto start_normal;
    }
  }
  goto start_escape_chunk;
}
