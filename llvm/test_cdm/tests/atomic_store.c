// CHECK reg(r0) 123

_Atomic(int) var = 0;

__attribute__((noinline))
void atomic_store(_Atomic(int) *p, int val) {
    *p = val;
}

int main() {
    atomic_store(&var, 123);
    return var;
}
