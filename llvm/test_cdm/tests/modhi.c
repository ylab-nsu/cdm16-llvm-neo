// CHECK reg(r0) 2

__attribute__((noinline))
int modhi(int a, int b) {
    return a % b; 
}

int main() {
    volatile int a = 80;
    volatile int b = 3;
    return modhi(a, b); 
}