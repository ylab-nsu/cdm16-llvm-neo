// CHECK reg(r0) 0

__attribute__((noinline))
int divhi(int a, int b) {
    return a / b; 
}

int main() {
    volatile int a = 0;
    volatile int b = 5;
    return divhi(a, b);
}