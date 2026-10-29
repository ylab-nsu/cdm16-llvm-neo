# RUN: llvm-mc -triple=cdm -filetype=obj %s -o %t.o
# RUN: llvm-objdump -d %t.o | FileCheck %s

# --- 16-bit instructions ---

halt
wait

push r0
push r5
pop r2

add r0, r1, r2
add r3, r4, r5

# --- 32-bit immediate instructions ---

ldi r0, 1000
ldi r3, 1234

# --- 32-bit calls and branches ---

jsr 1000
br 2000

# --- 32-bit conditional branches ---

beq 1000
bne 2000
bgt 3000
bhs 1000
blo 1000
bmi 1000
bpl 1000
bge 1000
blt 1000
ble 1000

# --- Mixed instruction lengths ---

halt
ldi r0, 1000
push r5
br 2000
pop r2
wait
halt

# --- FileCheck expectations ---

# CHECK: {{.*}}halt
# CHECK-NEXT: {{.*}}wait
# CHECK-NEXT: {{.*}}push{{[[:space:]]+}}r0
# CHECK-NEXT: {{.*}}push{{[[:space:]]+}}r5
# CHECK-NEXT: {{.*}}pop{{[[:space:]]+}}r2
# CHECK-NEXT: {{.*}}add{{[[:space:]]+}}r0, r1, r2
# CHECK-NEXT: {{.*}}add{{[[:space:]]+}}r3, r4, r5
# CHECK-NEXT: {{.*}}ldi{{[[:space:]]+}}r0, 1000
# CHECK-NEXT: {{.*}}ldi{{[[:space:]]+}}r3, 1234
# CHECK-NEXT: {{.*}}jsr{{[[:space:]]+}}1000
# CHECK-NEXT: {{.*}}br{{[[:space:]]+}}2000
# CHECK-NEXT: {{.*}}beq{{[[:space:]]+}}1000
# CHECK-NEXT: {{.*}}bne{{[[:space:]]+}}2000
# CHECK-NEXT: {{.*}}bgt{{[[:space:]]+}}3000
# CHECK-NEXT: {{.*}}bhs{{[[:space:]]+}}1000
# CHECK-NEXT: {{.*}}blo{{[[:space:]]+}}1000
# CHECK-NEXT: {{.*}}bmi{{[[:space:]]+}}1000
# CHECK-NEXT: {{.*}}bpl{{[[:space:]]+}}1000
# CHECK-NEXT: {{.*}}bge{{[[:space:]]+}}1000
# CHECK-NEXT: {{.*}}blt{{[[:space:]]+}}1000
# CHECK-NEXT: {{.*}}ble{{[[:space:]]+}}1000
# CHECK-NEXT: {{.*}}halt
# CHECK-NEXT: {{.*}}ldi{{[[:space:]]+}}r0, 1000
# CHECK-NEXT: {{.*}}push{{[[:space:]]+}}r5
# CHECK-NEXT: {{.*}}br{{[[:space:]]+}}2000
# CHECK-NEXT: {{.*}}pop{{[[:space:]]+}}r2
# CHECK-NEXT: {{.*}}wait
# CHECK-NEXT: {{.*}}halt
