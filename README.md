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

Without escapes: ~165MiB/s
It's actually faster with escapes, because it has to output less.
