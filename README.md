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

With escapes: ~570MiB/s  
Without escapes: ~1.55GiB/s

Benchmarks performed on a laptop with a AMD Ryzen 5 5625U.  
This is running through a shell pipeline with multiple stages.  
See benchmark.sh for details.
