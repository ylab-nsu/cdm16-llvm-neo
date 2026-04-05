// CHECK reg(r0) 0

struct lol {
    int arr[228];
};

int main() {
    volatile struct lol kek = {{0}};
    volatile int a = kek.arr[69];
}
