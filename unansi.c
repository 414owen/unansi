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

#if defined(__GNUC__) || defined(__clang__)
# define UNLIKELY(x)     (__builtin_expect(!!(x),false))
# define LIKELY(x)       (__builtin_expect(!!(x),true))
# define fwrite          fwrite_unlocked
# define fflush          fflush_unlocked
#else
# define UNLIKELY(x)     (x)
# define LIKELY(x)       (x)
#endif

struct state {
  size_t offset;
  size_t amt;
  bool dirty;
  char buffer[BUF_SIZE];
  int stdin_fd;
  int stdout_fd;
};

static inline void
fill_buffer(struct state *restrict state) {
  state->offset = 0;
  state->amt = read(state->stdin_fd, state->buffer, BUF_SIZE);
  if (UNLIKELY(state->amt == 0)) {
    exit(0);
  }
}

static inline void
maybe_flush(struct state *restrict state) {
  if (LIKELY(state->dirty)) {
    fflush(stdout);
    state->dirty = false;
  }
}

static inline void
write_out(struct state *restrict state, size_t start) {
  if (LIKELY(state->offset - start < HALF_BUF_SIZE)) {
    fwrite(&state->buffer[start], 1, state->offset - start, stdout);
    state->dirty = true;
  } else {
    // This branch avoids the excess buffering of using stdout as a
    // FILE*, generally in the cases where there's lots of input with
    // no ANSI codes.
    maybe_flush(state);
    size_t written = 0;
    size_t amt = state->offset - start;
    do {
      ssize_t res = write(state->stdout_fd, &state->buffer[start], amt);
      if (UNLIKELY(res < 0)) {
        perror("Can't write data out");
        exit(1);
      } else {
        written += res;
      }
    } while (written < amt);
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
  maybe_flush(&state);
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
  maybe_flush(&state);
  fill_buffer(&state);
start_escape:
  for (; state.offset < state.amt; state.offset++) {
    if (UNLIKELY(isalpha(state.buffer[state.offset]))) {
      state.offset++;
      goto start_normal;
    }
  }
  goto start_escape_chunk;
}
