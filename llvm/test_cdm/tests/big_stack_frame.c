// CHECK reg(r0) 0

int main() {
    volatile int a = 69;
    volatile int b[228] = {1337};
    volatile int c = b[42];
    volatile char d[52] = {'a'};
    volatile char e = d[0];
    volatile signed char f[42] = {-1};
    volatile signed int g = f[13];
}
