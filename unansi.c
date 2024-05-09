#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

static inline char
get(void) {
  int c = getchar();
  if (c == EOF) {
    if (feof(stdin)) {
      exit(0);
    }
  }
  return c;
}

int main(void) {
  // here
  while (true) {
    int c = get();
    switch (c) {
      case 0x07: // bel
      case 0x08: // backspace
      case 0x0c: // form feed
      case 0x7f: // delete
        break;

      // escape
      case 0x1b:
        c = get();
        switch (c) {
          case '[':
            c = get();
            while (!isalpha(c)) {
              c = get();
            }
            break;
        }
        break;
      default:
        putchar(c);
        break;
    }
  }
  return 1;
}
