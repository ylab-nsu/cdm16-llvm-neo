// CHECK reg(r0) 0x0F

_Atomic(int) var = 0xFF;

__attribute__((noinline))
void fetch_and_xor(_Atomic(int) *p, int val) {
    (*p) ^= val;
}

int main() {
    fetch_and_xor(&var, 0xF0);
    return var;
}
