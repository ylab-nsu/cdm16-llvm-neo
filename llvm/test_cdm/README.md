# Testing tool for cross-compiling for CdM-16 with clang
## Requirements

- Python
- Java

## Usage
`make CLANG=<path_to_clang>`

`python3 tests_runner.py -c <path to clang> [-I <headers search path>]... [-p <cocoemu-server port>] <path to tests dir>`

## Tests format

If file in `tests` directory is regular it's treated as a single-source test, if it's a directory, it's treated as a multi-source test.

### Source file format

```c
// CHECK <directive>

<any c code>

int main(){
  return <result>;
}
```

### Single-source test

Test consisting of the only one source file. All files from `commons` will be linked to it.

### Multi-source test

Test consisting of several source files. Files from `commons` will be linked only if there is no source file with the same name in this test (e.g. if you want to have custom IVT, create file `ivt.asm` in test directory)

## Directives
- `reg(<reg>) <value>` check if register \<reg\> contains \<value\> at the end of simulation
  - `<reg>` - `r0`, `r1`, `r2`, `r3`, ...
  - `<value>` - int16 value (signed or unsigned)
- `sym(<symbol>) <content>` create memory region with size equal size of `<content>` with label `<symbol>` at the start of this region. Check if at the end of simulation this memory region contans `<content>`
- `mem(<address>) <content>` check if at the end of simulation memory region at address `<address>` contains `<content>`
  - `<address>` - unsigned int16
  - `<content>` - sequence of bytes written in hex (e.g. `FF 00 A2`)
