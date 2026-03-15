// CHECK reg(r0) 77

_Atomic(int) var = 100;

__attribute__((noinline))
void fetch_and_sub(_Atomic(int) *p, int val) {
    (*p) -= val;
}

int main() {
    fetch_and_sub(&var, 23);
    return var;
}
