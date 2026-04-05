// CHECK reg(r0) 65514

struct lol {
    int arr[228];
};

__attribute__((noinline))
int foo(volatile struct lol kek) {
    volatile int gol = kek.arr[42];
    volatile int a[420] = {32, 34, 78, 12};
    kek.arr[69] = a[3] ^ kek.arr[99];
    kek.arr[56] = a[1] + gol - kek.arr[69];
    return a[88] - kek.arr[56];
}
int main() {
    volatile struct lol kek = {{0}};
    return kek.arr[69] + foo(kek);
}
