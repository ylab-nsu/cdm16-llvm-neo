// CHECK reg(r0) 65025

__attribute__((noinline))
int mulhi(int a, int b) {
    return a * b; 
}

int main() {
    volatile int a = 255;
    volatile int b = 255;
    return mulhi(a, b); 
}