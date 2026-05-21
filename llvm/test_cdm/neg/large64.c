// CHECK reg(r0) 0xEDCC
// CHECK reg(r1) 0x6543
// CHECK reg(r2) 0xFFFF
// CHECK reg(r3) 0xFFFF

__attribute__((noinline))
long long neg64(long long a) {
    return -a;
}

long long main() {
    volatile long long a = 0x000000009ABC1234LL;
    return neg64(a);
}
