// CHECK reg(r0) 2

__attribute__((noinline))
unsigned int umodhi(unsigned int a, unsigned int b) {
    return a % b; 
}

unsigned int main() {
    volatile unsigned int a = 80;
    volatile unsigned int b = 3;
    return umodhi(a, b); 
}