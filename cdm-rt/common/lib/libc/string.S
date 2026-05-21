#include "cdm_asm.inc"

RSECT(memcpy)
GLOBAL(memcpy)
tst r2
bz memcpy_end
push r0
memcpy_beg:
ldb r1, r3
stb r0, r3
inc r0
inc r1
dec r2
bnz memcpy_beg
pop r0
memcpy_end:
rts

RSECT(memccpy)
GLOBAL(memccpy)
tst r3
bnz memccpy_beg
clr r0
rts
memccpy_beg:
push r4
scl r2, r2
memccpy_loop_beg:
ldb r1, r4
stb r0, r4
inc r0
inc r1
cmp r2, r4
beq memccpy_loop_end
dec r3
bnz memccpy_loop_beg
clr r0
memccpy_loop_end:
pop r4
memccpy_end:
rts

RSECT(memmove)
GLOBAL(memmove)
tst r2
bz memmove_end
cmp r0, r1
blt memmove_fwd
bgt memmove_bwd
memmove_end:
rts

memmove_fwd:
push r0
memmove_fwd_beg:
ldb r1, r3
stb r0, r3
inc r0
inc r1
dec r2
bnz memmove_fwd_beg
pop r0
rts

memmove_bwd:
push r0
add r0, r2, r0
add r1, r2, r1
dec r0
dec r1
memmove_bwd_beg:
ldb r1, r3
stb r0, r3
dec r0
dec r1
dec r2
bnz memmove_bwd_beg
pop r0
rts

RSECT(strcpy)
GLOBAL(strcpy)
move r0, r3
strcpy_beg:
ldb r1, r2
stb r3, r2
inc r3
inc r1
tst r2
bnz strcpy_beg
strcpy_end:
rts

RSECT(strncpy)
GLOBAL(strncpy)
push r0
tst r2
bz strncpy_end
strncpy_beg:
ldb r1, r3
stb r0, r3
inc r0
inc r1
dec r2
bz strncpy_end
tst r3
bnz strncpy_beg

strncpy_pad_beg:
stb r0, r3
inc r0
dec r2
bnz strncpy_pad_beg
strncpy_end:
pop r0
rts

RSECT(strcat)
GLOBAL(strcat)
push r0
strcat_beg:
ldb r0, r2
inc r0
tst r2
bnz strcat_beg
dec r0
jsr strcpy
pop r0
rts

RSECT(strncat)
GLOBAL(strncat)
push r0
tst r2
bz strncat_end
strncat_beg:
ldb r0, r3
inc r0
tst r3
bnz strncat_beg
dec r0
strncat_cpy_beg:
ldb r1, r3
stb r0, r3
inc r1
inc r0
tst r3
bz strncat_end
dec r2
bnz strncat_cpy_beg
stb r0, r2
strncat_end:
pop r0
rts

RSECT(memcmp)
GLOBAL(memcmp)
push r4
move r0, r3
clr r0
tst r2
bz memcmp_end
memcmp_beg:
ldb r3, r0
ldb r1, r4
sub r0, r4, r0
bnz memcmp_end
inc r3
inc r1
dec r2
bnz memcmp_beg
memcmp_end:
pop r4
rts

RSECT(strcmp)
GLOBAL(strcmp)
move r0, r2
strcmp_beg:
ldb r2, r0
ldb r1, r3
inc r2
inc r1
sub r0, r3, r0
bnz strcmp_end
tst r3
bnz strcmp_beg
strcmp_end:
rts

RSECT(strncmp)
GLOBAL(strncmp)
push r4
move r0, r3
clr r0
tst r2
bz strncmp_end
strncmp_beg:
ldb r3, r0
ldb r1, r4
sub r0, r4, r0
bnz strncmp_end
inc r3
inc r1
tst r4
bz strncmp_end
dec r2
bnz strncmp_beg
strncmp_end:
pop r4
rts

RSECT(memchr)
GLOBAL(memchr)
tst r2
bz memchr_null_end
scl r1, r1
memchr_beg:
ldb r0, r3
cmp r1, r3
beq memchr_end
inc r0
dec r2
bnz memchr_beg
memchr_null_end:
clr r0
memchr_end:
rts

RSECT(strchr)
GLOBAL(strchr)
scl r1, r1
strchr_beg:
ldb r0, r2
cmp r1, r2
beq strchr_end
inc r0
tst r2
bnz strchr_beg
clr r0
strchr_end:
rts

RSECT(strcspn)
GLOBAL(strcspn)
push r4
push r0
br strcspn_test
strcspn_beg:
move r1, r3
strcspn_cmp_beg:
ldb r3, r4
cmp r2, r4
beq strcspn_end
inc r3
tst r4
bnz strcspn_cmp_beg
inc r0
strcspn_test:
ldb r0, r2
tst r2
bnz strcspn_beg
strcspn_end:
pop r1
pop r4
sub r0, r1, r0
rts

RSECT(strpbrk)
GLOBAL(strpbrk)
push r0
jsr strcspn
pop r1
add r0, r1, r0
ldb r0, r1
tst r1
bnz strpbrk_end
clr r0
strpbrk_end:
rts

RSECT(strrchr)
GLOBAL(strrchr)
move r0, r2
clr r0
scl r1, r1
strrchr_beg:
ldb r2, r3
cmp r3, r1
bne strrchr_not_equal
move r2, r0
strrchr_not_equal:
inc r2
tst r3
bnz strrchr_beg
strrchr_end:
rts

RSECT(strspn)
GLOBAL(strspn)
push r4
push r0
br strspn_test
strspn_beg:
move r1, r3
strspn_cmp_beg:
ldb r3, r4
tst r4
bz strspn_end
inc r3
cmp r2, r4
bne strspn_cmp_beg
inc r0
strspn_test:
ldb r0, r2
tst r2
bnz strspn_beg
strspn_end:
pop r1
pop r4
sub r0, r1, r0
rts

RSECT(strstr)
GLOBAL(strstr)
strstr_beg:
push r0
push r1

strstr_cmp_beg:
ldb r0, r2
ldb r1, r3
inc r0
inc r1
tst r3
bz strstr_found_end
sub r2, r3, r2
bz strstr_cmp_beg
strstr_cmp_end:

pop r1
pop r0
inc r0
ldb r0, r2
tst r2
bnz strstr_beg
clr r0
rts

strstr_found_end:
pop r1
pop r0
rts

RSECT(memset)
GLOBAL(memset_explicit)
GLOBAL(memset)
move r0, r3
tst r2
bz memset_end
memset_beg:
stb r3, r1
inc r3
dec r2
bnz memset_beg
memset_end:
rts

RSECT(strlen)
GLOBAL(strlen)
move r0, r1
strlen_beg:
ldb r1, r2
inc r1
tst r2
bnz strlen_beg
dec r1
sub r1, r0, r0
rts

END
