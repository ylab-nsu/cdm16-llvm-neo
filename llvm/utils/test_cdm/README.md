# Testing tool for CdM-16 target clang
## Requirements

- Python
- Java

## Usage
`test_cdm [tests_search_point..]`

See `test_cdm --help` for available options

## Tests format

### Single-source test

Every regular file located in directory which doesn't contain `.test_dummy` file is treated as a single-source test

### Multi-source test

Every directory, which contain `.test_dummy` file is treated as a multi-source test

Directives are gathered from each C source file from this directory

### Source file format

```c
// CHECK prod(<some_producer> <producer_arg..>)
// CHECK <prod_directive1>
// CHECK <prod_directive2>

// CHECK prod(<another_producer> <producer_arg..>)
...

<any c code>

int main(){
  return <result>;
}
```

## Test producers

### End-to-end

To use this producer on test - include directive `// CHECK prod(end_to_end [cocas] [elf])` in some of its' source files and specify assertions in directives below

This producer will produce end-to-end test cases for all targets specified with all possible optimization levels ('O0', 'O1', 'O2', 'O3', 'Os')

#### Directives
- `reg(<reg>) <value>` check if register \<reg\> contains \<value\> at the end of simulation
  - `<reg>` - `r0`, `r1`, `r2`, `r3`, ...
  - `<value>` - int16 value (signed or unsigned)
- `sym(<symbol>) <content>` create memory region with size equal size of `<content>` with label `<symbol>` at the start of this region. Check if at the end of simulation this memory region contans `<content>`
- `mem(<address>) <content>` check if at the end of simulation memory region at address `<address>` contains `<content>`
  - `<address>` - unsigned int16
  - `<content>` - sequence of bytes written in hex (e.g. `FF 00 A2`)

### Driver only

To use this producer on test - include directive `// CHECK prod(driver_only)` in some of its' source files

Test-cases, produced by this producer check that calling clang with '-c' flag specified result in same object files as calling clang with '-S' flag specified and then assembling generated file with cocas
