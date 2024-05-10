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

With escapes: ~570MiB/s  
Without escapes: ~1.55GiB/s

Benchmarks performed on a laptop with a AMD Ryzen 5 5625U.  
This is running through a shell pipeline with multiple stages.  
See benchmark.sh for details.
