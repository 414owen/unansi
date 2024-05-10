# unansi

Strips out ANSI control sequences.

## Building

```sh
$ cc unansi.c -o unansi
```

## Usage

```sh
$ <something> | ./unansi
```

## Performance

With escapes: ~750MiB/s
Without escapes: ~1.3GiB/s

This was calculated by user process time, not wall time, but both input and output is buffered.
