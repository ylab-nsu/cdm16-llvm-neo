// CHECK reg(r0) 0

__attribute__((noinline))
unsigned int udivhi(unsigned int a, unsigned int b) {
    return a / b; 
}

unsigned int main() {
    volatile unsigned int a = 0;
    volatile unsigned int b = 5;
    return udivhi(a, b); 
}