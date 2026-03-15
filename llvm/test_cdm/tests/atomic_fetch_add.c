// CHECK reg(r0) 123

_Atomic(int) var = 100;

__attribute__((noinline))
void fetch_and_add(_Atomic(int) *p, int val) {
    (*p) += val;
}

int main() {
    fetch_and_add(&var, 23);
    return var;
}
