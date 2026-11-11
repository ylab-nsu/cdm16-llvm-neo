# RUN: llvm-mc -triple=cdm -filetype=obj %s -o %t.o
# RUN: llvm-objdump -d %t.o | FileCheck %s

halt
add r0, r1, r2
push r5
ldi r0, 1000
jsr 1000
br 2000
beq 1000
bne 2000
halt

# CHECK: halt
# CHECK-NEXT: {{.*}}add{{[[:space:]]+}}r0, r1, r2
# CHECK-NEXT: {{.*}}push{{[[:space:]]+}}r5
# CHECK-NEXT: {{.*}}ldi{{[[:space:]]+}}r0, 1000
# CHECK-NEXT: {{.*}}jsr{{[[:space:]]+}}1000
# CHECK-NEXT: {{.*}}br{{[[:space:]]+}}2000
# CHECK-NEXT: {{.*}}beq{{[[:space:]]+}}1000
# CHECK-NEXT: {{.*}}bne{{[[:space:]]+}}2000
# CHECK-NEXT: {{.*}}halt
