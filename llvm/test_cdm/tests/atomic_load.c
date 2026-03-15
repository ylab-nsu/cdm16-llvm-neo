// CHECK reg(r0) 42

_Atomic(int) var = 42;

int main() {
    return var;
}
