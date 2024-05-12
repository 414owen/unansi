# unansi

![language:c99](https://img.shields.io/badge/language-c99-yellow)
![GitHub Release](https://img.shields.io/github/v/release/414owen/unansi)
![GitHub License](https://img.shields.io/github/license/414owen/unansi)

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

With escapes: ~727MiB/s  
Without escapes: ~1.36GiB/s

Benchmarks were performed on a laptop with an AMD Ryzen 5 5625U.  
They were run multiple times, and the best result was taken.  
This is running through a shell pipeline with multiple stages,  
so the real throughput is probably better than the benchmarks say.  
See benchmark.sh for details.
