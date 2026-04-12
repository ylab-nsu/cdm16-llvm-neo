# Testing tool for CdM-16 target clang
## Requirements

- Python
- Java

## Usage
`test-cdm [tests_search_point..]`

See `test-cdm --help` for available options

## Tests format

### Single-source test

Every regular file located in directory which doesn't contain `multi_source` file is treated as a single-source test

### Multi-source test

Every directory, which contain `multi_source` file is treated as a multi-source test

Directives are gathered from `multi_source` file (since it's not source file they shouldn't be commented e.g. `CHECK reg(r0) 0`, not `// CHECK reg(r0) 0`)

### Source file format

```c
// CHECK <directive1>
// CHECK <directive2>
...

<any c code>

int main(){
  return <result>;
}
```

### Directives
- `reg(<reg>) <value>` check if register \<reg\> contains \<value\> at the end of simulation
  - `<reg>` - `r0`, `r1`, `r2`, `r3`, ...
  - `<value>` - int16 value (signed or unsigned)
- `sym(<symbol>) <content>` create memory region with size equal size of `<content>` with label `<symbol>` at the start of this region. Check if at the end of simulation this memory region contans `<content>`
- `mem(<address>) <content>` check if at the end of simulation memory region at address `<address>` contains `<content>`
  - `<address>` - unsigned int16
  - `<content>` - sequence of bytes written in hex (e.g. `FF 00 A2`)

