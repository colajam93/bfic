# bfic

Simple Brainfuck interpreter with visualizing internal state

## Requirement

- CMake >= 3.7
- Clang (with C++17 support)

## Build and Usage

```
$ cmake /path/to/bfic
$ make
$ echo '++++[>+++++<-]>[<+++++>-]<--.++++.+++.------.' | ./bfic (or ./bfic -p)
```

## License

MIT
