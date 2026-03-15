// CHECK reg(r0) 0xFF

_Atomic(int) var = 0xF0;

__attribute__((noinline))
void fetch_and_or(_Atomic(int) *p, int val) {
    (*p) |= val;
}

int main() {
    fetch_and_or(&var, 0x0F);
    return var;
}
