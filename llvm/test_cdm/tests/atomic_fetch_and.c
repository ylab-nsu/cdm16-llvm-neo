// CHECK reg(r0) 0x0F00

_Atomic(int) var = 0x7F0F;

__attribute__((noinline))
void fetch_and_and(_Atomic(int) *p, int val) {
    (*p) &= val;
}

int main() {
    fetch_and_and(&var, 0x0FF0);
    return var;
}
