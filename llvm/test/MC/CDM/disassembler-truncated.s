# RUN: echo "0x10 0x20" | llvm-mc -triple=cdm --disassemble 2>&1 | FileCheck %s --check-prefix=TRUNC
# RUN: echo "0x08 0x00" | llvm-mc -triple=cdm --disassemble 2>&1 | FileCheck %s --check-prefix=TRUNC
# RUN: echo "0x0f 0x08" | llvm-mc -triple=cdm --disassemble 2>&1 | FileCheck %s --check-prefix=TRUNC

# Missing the second word of LDI, JSR or an absolute branch is invalid.
# TRUNC: warning: invalid instruction encoding
